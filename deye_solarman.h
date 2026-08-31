#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "app_data.h"
#include "settings.h"

// ==================== CONSTANTES ====================
#define DEYE_PORT 8899
#define DEYE_MODBUS_SLAVE_ID 1

#define BLOCK1_START 3
#define BLOCK1_COUNT 110

#define BLOCK2_START 150
#define BLOCK2_COUNT 100

#define TCP_CONNECT_TIMEOUT_MS 5000
#define RESPONSE_WINDOW_MS 5000
#define FRAME_TIMEOUT_MS 3000

// ==================== STRUCTURES ====================
struct MainData {
  uint16_t pv1_power;
  uint16_t pv2_power;
  uint16_t pv3_power;
  int16_t bat_temperature_raw;
  uint16_t bat_voltage_raw;
  uint16_t bat_soc;
  int16_t bat_power;
  int16_t grid_power;
  int16_t load_power;
  int16_t ups_load_power;
  uint16_t ups_load_voltage_raw;
  uint16_t gen_voltage_raw;
  uint16_t grid_status_raw;
  uint16_t smartload_status_raw;
  int16_t dc_temperature_raw;
  int16_t ac_temperature_raw;
  uint16_t daily_grid_buy;
  uint16_t daily_grid_sell;
  uint16_t daily_load;
  uint16_t daily_solar;
};

// ==================== DONNÉES PARTAGÉES ====================
static MainData main_data;
static bool main_data_valid = false;
bool deye_on_grid_state = true;

static uint16_t daily_solar = 0;
static bool daily_solar_valid = false;
static uint16_t daily_load = 0;
static bool daily_load_valid = false;
static uint16_t daily_grid_buy = 0;
static bool daily_grid_buy_valid = false;
static uint16_t daily_grid_sell = 0;
static bool daily_grid_sell_valid = false;

static uint16_t pv_daily_yield = 0;
static bool pv_daily_yield_valid = false;

// ==================== SYNCHRONISATION ====================
static SemaphoreHandle_t data_mutex = nullptr;
static volatile bool ui_active = false;
static volatile bool reader_running = false;
static volatile bool new_data_available = false;

// ==================== DÉCLARATIONS ====================
static void solarman_reader_task(void *pvParameters);

// ==================== FONCTIONS PROTOCOLE ====================

static uint16_t modbus_crc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
      else crc >>= 1;
    }
  }
  return crc;
}

static uint8_t solarman_checksum(const uint8_t *data, size_t len_without_trailer) {
  uint16_t sum = 0;
  for (size_t i = 1; i < len_without_trailer; i++) sum += data[i];
  return (uint8_t)(sum & 0xFF);
}

static void write_u32_le(uint8_t *dst, uint32_t value) {
  dst[0] = (uint8_t)(value & 0xFF);
  dst[1] = (uint8_t)((value >> 8) & 0xFF);
  dst[2] = (uint8_t)((value >> 16) & 0xFF);
  dst[3] = (uint8_t)((value >> 24) & 0xFF);
}

static void build_modbus_rtu_read(uint16_t start_reg, uint16_t count, uint8_t *out) {
  out[0] = DEYE_MODBUS_SLAVE_ID;
  out[1] = 0x03;
  out[2] = (uint8_t)(start_reg >> 8);
  out[3] = (uint8_t)(start_reg & 0xFF);
  out[4] = (uint8_t)(count >> 8);
  out[5] = (uint8_t)(count & 0xFF);
  uint16_t crc = modbus_crc16(out, 6);
  out[6] = (uint8_t)(crc & 0xFF);
  out[7] = (uint8_t)(crc >> 8);
}

static size_t build_solarman_v5_request(const uint8_t *rtu, size_t rtu_len, uint8_t *out) {
  size_t pos = 0;
  uint16_t payload_len = 15 + rtu_len;
  out[pos++] = 0xA5;
  out[pos++] = (uint8_t)(payload_len & 0xFF);
  out[pos++] = (uint8_t)(payload_len >> 8);
  out[pos++] = 0x10;
  out[pos++] = 0x45;
  static uint8_t sequence = 0;
  out[pos++] = sequence++;
  out[pos++] = 0x00;
  write_u32_le(&out[pos], cfg_logger_serial);
  pos += 4;
  out[pos++] = 0x02;
  for (uint8_t i = 0; i < 14; i++) out[pos++] = 0x00;
  for (size_t i = 0; i < rtu_len; i++) out[pos++] = rtu[i];
  out[pos++] = solarman_checksum(out, pos);
  out[pos++] = 0x15;
  return pos;
}

static bool read_exact(WiFiClient &client, uint8_t *buffer, size_t wanted, uint32_t timeout_ms) {
  size_t received = 0;
  uint32_t started = millis();
  while (received < wanted && millis() - started < timeout_ms) {
    while (client.available() && received < wanted) {
      buffer[received++] = client.read();
    }
    delay(1);
  }
  return received == wanted;
}

static bool receive_one_v5_frame(WiFiClient &client, uint8_t *frame, size_t *frame_len) {
  uint8_t header[11];
  if (!read_exact(client, header, sizeof(header), FRAME_TIMEOUT_MS)) return false;
  if (header[0] != 0xA5) return false;
  uint16_t payload_len = (uint16_t)header[1] | ((uint16_t)header[2] << 8);
  size_t total = 11 + payload_len + 2;
  if (total > 256 || payload_len == 0) return false;
  memcpy(frame, header, 11);
  if (!read_exact(client, frame + 11, payload_len + 2, FRAME_TIMEOUT_MS)) return false;
  *frame_len = total;
  return true;
}

static uint16_t modbus_get_u16_be(const uint8_t *data, uint8_t index) {
  size_t p = 3 + (size_t)index * 2;
  return ((uint16_t)data[p] << 8) | data[p + 1];
}

static bool get_rtu_from_v5_frame(const uint8_t *frame, size_t frame_len, uint16_t wanted_count, uint8_t *rtu_out) {
  size_t wanted_rtu_len = 5 + (size_t)wanted_count * 2;
  uint8_t expected_byte_count = wanted_count * 2;
  for (size_t i = 0; i + wanted_rtu_len <= frame_len - 2; i++) {
    if (frame[i] == DEYE_MODBUS_SLAVE_ID && frame[i + 1] == 0x03 && frame[i + 2] == expected_byte_count) {
      uint16_t received_crc = (uint16_t)frame[i + wanted_rtu_len - 2] | ((uint16_t)frame[i + wanted_rtu_len - 1] << 8);
      if (received_crc != modbus_crc16(frame + i, wanted_rtu_len - 2)) continue;
      memcpy(rtu_out, frame + i, wanted_rtu_len);
      return true;
    }
  }
  return false;
}

// ==================== LECTURE D'UN BLOC ====================
static bool solarman_read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response) {
  WiFiClient client;
  uint8_t rtu_request[8];
  uint8_t v5_request[64];
  uint8_t frame[256];
  size_t frame_len = 0;

  build_modbus_rtu_read(first_reg, count, rtu_request);
  size_t request_len = build_solarman_v5_request(rtu_request, sizeof(rtu_request), v5_request);

  if (client.connected()) {
    client.stop();
    delay(50);
  }

  if (!client.connect(cfg_deye_host.c_str(), DEYE_PORT, TCP_CONNECT_TIMEOUT_MS)) {
    return false;
  }

  client.write(v5_request, request_len);

  uint32_t deadline = millis() + RESPONSE_WINDOW_MS;
  bool found = false;

  while ((int32_t)(deadline - millis()) > 0) {
    if (!client.available()) {
      delay(2);
      continue;
    }

    if (!receive_one_v5_frame(client, frame, &frame_len)) {
      break;
    }

    if (get_rtu_from_v5_frame(frame, frame_len, count, rtu_response)) {
      found = true;
      break;
    }
  }

  client.stop();
  return found;
}

// ==================== DÉCODAGE ====================

static void decode_block1(const uint8_t *rtu) {
  int offset = BLOCK1_START;
  
  daily_grid_buy = modbus_get_u16_be(rtu, 76 - offset);
  daily_grid_buy_valid = true;
  
  daily_grid_sell = modbus_get_u16_be(rtu, 77 - offset);
  daily_grid_sell_valid = true;
  
  daily_load = modbus_get_u16_be(rtu, 84 - offset);
  daily_load_valid = true;
  
  uint16_t dc_raw = modbus_get_u16_be(rtu, 90 - offset);
  if (dc_raw != 0 && dc_raw != 1000) {
    main_data.dc_temperature_raw = (int16_t)dc_raw;
  }
  
  uint16_t ac_raw = modbus_get_u16_be(rtu, 91 - offset);
  if (ac_raw != 0 && ac_raw != 1000) {
    main_data.ac_temperature_raw = (int16_t)ac_raw;
  }
  
  daily_solar = modbus_get_u16_be(rtu, 108 - offset);
  daily_solar_valid = true;
  pv_daily_yield = daily_solar;
  pv_daily_yield_valid = true;
}

static void decode_block2(const uint8_t *rtu) {
  int offset = BLOCK2_START;
  
  main_data.gen_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  main_data.ups_load_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  main_data.grid_power = (int16_t)modbus_get_u16_be(rtu, 169 - offset);
  main_data.ups_load_power = (int16_t)modbus_get_u16_be(rtu, 172 - offset);
  main_data.load_power = (int16_t)modbus_get_u16_be(rtu, 178 - offset);
  main_data.bat_temperature_raw = (int16_t)modbus_get_u16_be(rtu, 182 - offset);
  main_data.bat_voltage_raw = modbus_get_u16_be(rtu, 183 - offset);
  main_data.bat_soc = modbus_get_u16_be(rtu, 184 - offset);
  main_data.pv1_power = modbus_get_u16_be(rtu, 186 - offset);
  main_data.pv2_power = modbus_get_u16_be(rtu, 187 - offset);
  main_data.pv3_power = modbus_get_u16_be(rtu, 188 - offset);
  main_data.bat_power = (int16_t)modbus_get_u16_be(rtu, 190 - offset);
  main_data.grid_status_raw = modbus_get_u16_be(rtu, 194 - offset);
  main_data.smartload_status_raw = modbus_get_u16_be(rtu, 195 - offset);
  
  main_data_valid = true;
}

static void update_dashboard_from_data() {
  if (!main_data_valid) return;

  dashboard_data.valid = true;
  dashboard_data.pv1_w = main_data.pv1_power;
  dashboard_data.pv2_w = main_data.pv2_power;
  dashboard_data.pv3_w = main_data.pv3_power;
  dashboard_data.battery_soc = main_data.bat_soc;
  dashboard_data.battery_voltage = main_data.bat_voltage_raw * 0.01f;
  dashboard_data.battery_power = main_data.bat_power;
  dashboard_data.battery_temperature = (main_data.bat_temperature_raw - 1000) * 0.1f;
  dashboard_data.grid_power = main_data.grid_power * 10;
  dashboard_data.load_power = main_data.load_power * 10;
  dashboard_data.ups_power = main_data.ups_load_power;
  dashboard_data.ups_voltage = main_data.ups_load_voltage_raw * 0.1f;
  dashboard_data.dc_temperature = (main_data.dc_temperature_raw - 1000) * 0.1f;
  dashboard_data.ac_temperature = (main_data.ac_temperature_raw - 1000) * 0.1f;
  dashboard_data.smartload_on = (main_data.smartload_status_raw & 0x01) == 0x01;
  deye_on_grid_state = (main_data.grid_status_raw == 1);
  
  if (daily_solar_valid) {
    pv_daily_yield = daily_solar;
    pv_daily_yield_valid = true;
  }
}

// ==================== TÂCHE DE LECTURE (CŒUR 1) ====================
static void solarman_reader_task(void *pvParameters) {
  int current_block = 0;
  uint32_t last_read_time = 0;
  bool first_read_done = false;
  
  while (true) {
    if (ui_active) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    if (WiFi.status() != WL_CONNECTED) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    uint32_t now = millis();
    
    if (!first_read_done) {
      if (now - last_read_time < 2000) {
        vTaskDelay(pdMS_TO_TICKS(50));
        continue;
      }
      first_read_done = true;
      DBG.println("=== PREMIERE LECTURE (thread) ===");
    } else {
      if (now - last_read_time < 5000) {
        vTaskDelay(pdMS_TO_TICKS(50));
        continue;
      }
    }

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      bool ok = false;
      
      if (current_block == 0) {
        uint8_t b1_rtu[5 + BLOCK1_COUNT * 2];
        if (solarman_read_block(BLOCK1_START, BLOCK1_COUNT, b1_rtu)) {
          decode_block1(b1_rtu);
          DBG.println("BLOC1 OK (thread)");
          ok = true;
        } else {
          DBG.println("BLOC1 echec (thread)");
        }
      } else {
        uint8_t b2_rtu[5 + BLOCK2_COUNT * 2];
        if (solarman_read_block(BLOCK2_START, BLOCK2_COUNT, b2_rtu)) {
          decode_block2(b2_rtu);
          DBG.println("BLOC2 OK (thread)");
          ok = true;
        } else {
          DBG.println("BLOC2 echec (thread)");
        }
      }

      if (ok) {
        last_read_time = millis();
        update_dashboard_from_data();
        new_data_available = true;
      }

      current_block = (current_block + 1) % 2;
      xSemaphoreGive(data_mutex);
    }
    
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ==================== FONCTIONS PUBLIQUES ====================

void deye_solarman_set_ui_active(bool active) {
  ui_active = active;
}

static void deye_solarman_begin() {
  DBG.println("=== DEYE SOLARMAN V5 (Multithread) ===");
  DBG.printf("Bloc1: %d-%d (%d regs)\n", BLOCK1_START, BLOCK1_START + BLOCK1_COUNT - 1, BLOCK1_COUNT);
  DBG.printf("Bloc2: %d-%d (%d regs)\n", BLOCK2_START, BLOCK2_START + BLOCK2_COUNT - 1, BLOCK2_COUNT);
  DBG.printf("Logger Serial: %lu\n", cfg_logger_serial);
  
  main_data_valid = false;
  dashboard_data.valid = false;
  pv_daily_yield_valid = false;

  data_mutex = xSemaphoreCreateMutex();
  if (data_mutex == NULL) {
    DBG.println("ERREUR: creation mutex");
    return;
  }

  xTaskCreatePinnedToCore(
    solarman_reader_task,
    "SolarmanReader",
    8192,
    NULL,
    1,
    NULL,
    1
  );

  DBG.println("Tâche SolarmanReader démarrée sur le cœur 1");
  reader_running = true;
}

static void deye_solarman_process() {
  // Rien à faire - la tâche tourne en arrière-plan
}

// ==================== FONCTIONS POUR L'INTERFACE ====================
static bool deye_is_connected() { return main_data_valid; }
static bool deye_is_on_grid() { return deye_on_grid_state; }
static uint16_t deye_get_pv_daily() { return pv_daily_yield; }
static uint16_t deye_get_daily_load() { return daily_load; }
static uint16_t deye_get_daily_grid_buy() { return daily_grid_buy; }
static uint16_t deye_get_daily_grid_sell() { return daily_grid_sell; }
