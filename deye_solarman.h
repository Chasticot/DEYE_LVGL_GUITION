// deye_solarman.h - Version corrigée et unifiée

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
#define DEYE_MODBUS_SLAVE_ID 1

// ==================== VARIABLES DYNAMIQUES (valeurs par défaut, écrasées par settings) ====================
uint16_t REG_PV1_POWER = 186;
uint16_t REG_PV2_POWER = 187;
uint16_t REG_PV3_POWER = 188;
uint16_t REG_PV_DAILY = 108;
uint16_t REG_BATTERY_SOC = 184;
uint16_t REG_BATTERY_VOLTAGE = 183;
uint16_t REG_BATTERY_POWER = 190;
uint16_t REG_BATTERY_TEMP = 182;
uint16_t REG_GRID_POWER = 169;
uint16_t REG_GRID_STATUS = 194;
uint16_t REG_GRID_BUY_DAY = 76;
uint16_t REG_GRID_SELL_DAY = 77;
uint16_t REG_LOAD_POWER = 178;
uint16_t REG_UPS_POWER = 172;
uint16_t REG_LOAD_DAY = 84;
uint16_t REG_DC_TEMP = 90;
uint16_t REG_AC_TEMP = 91;
uint16_t REG_SMARTLOAD = 195;

uint16_t BLOCK1_START = 76;
uint16_t BLOCK1_COUNT = 37;
uint16_t BLOCK2_START = 169;
uint16_t BLOCK2_COUNT = 27;

uint32_t TCP_CONNECT_TIMEOUT_MS = 10000;
uint32_t RESPONSE_WINDOW_MS = 10000;
uint32_t FRAME_TIMEOUT_MS = 7000;
uint32_t BLOCK_INTERVAL_MS = 100;

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

// ==================== DÉCLARATIONS ====================
static void solarman_reader_task(void *pvParameters);

// ==================== FONCTIONS PROTOCOLE (LSW - Solarman V5) ====================

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

// ==================== LECTURE MODBUS TCP (LSE) ====================

static bool modbus_tcp_read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response) {
  WiFiClient client;
  uint8_t tcp_frame[12];
  static uint16_t trans_id = 0;
  trans_id++;
  tcp_frame[0] = trans_id >> 8;
  tcp_frame[1] = trans_id & 0xFF;
  tcp_frame[2] = 0x00;
  tcp_frame[3] = 0x00;
  tcp_frame[4] = 0x00;
  tcp_frame[5] = 0x06;
  tcp_frame[6] = DEYE_MODBUS_SLAVE_ID;
  tcp_frame[7] = 0x03;
  tcp_frame[8] = (uint8_t)(first_reg >> 8);
  tcp_frame[9] = (uint8_t)(first_reg & 0xFF);
  tcp_frame[10] = (uint8_t)(count >> 8);
  tcp_frame[11] = (uint8_t)(count & 0xFF);

  if (!client.connect(cfg_deye_host.c_str(), cfg_deye_port, TCP_CONNECT_TIMEOUT_MS)) {
    DBG.printf("❌ LSE: connexion échouée à %s:%d\n", cfg_deye_host.c_str(), cfg_deye_port);
    return false;
  }

  client.write(tcp_frame, sizeof(tcp_frame));

  uint8_t response[256];
  uint32_t deadline = millis() + RESPONSE_WINDOW_MS;
  size_t received = 0;
  while (received < 9 && millis() < deadline) {
    while (client.available() && received < 9) {
      response[received++] = client.read();
    }
    delay(1);
  }
  if (received < 9) {
    DBG.println("❌ LSE: réponse trop courte");
    client.stop();
    return false;
  }
  if (response[7] != 0x03) {
    DBG.printf("❌ LSE: code fonction incorrect %02X\n", response[7]);
    client.stop();
    return false;
  }
  uint8_t byte_count = response[8];
  size_t expected = 9 + byte_count;
  while (received < expected && millis() < deadline) {
    while (client.available() && received < expected) {
      response[received++] = client.read();
    }
    delay(1);
  }
  client.stop();
  if (received < expected) {
    DBG.println("❌ LSE: données incomplètes");
    return false;
  }

  // Convertir en format RTU factice pour les fonctions de décodage
  rtu_response[0] = DEYE_MODBUS_SLAVE_ID;
  rtu_response[1] = 0x03;
  rtu_response[2] = byte_count;
  for (int i = 0; i < byte_count; i++) {
    rtu_response[3 + i] = response[9 + i];
  }
  rtu_response[3 + byte_count] = 0x00;
  rtu_response[4 + byte_count] = 0x00;
  return true;
}

// ==================== LECTURE SOLARMAN V5 (LSW) ====================

static bool solarman_read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response) {
  WiFiClient client;
  uint8_t rtu_request[8];
  uint8_t v5_request[64];
  uint8_t frame[256];
  size_t frame_len = 0;

  build_modbus_rtu_read(first_reg, count, rtu_request);
  size_t request_len = build_solarman_v5_request(rtu_request, sizeof(rtu_request), v5_request);

  if (client.connected()) client.stop();
  delay(50);

  if (!client.connect(cfg_deye_host.c_str(), cfg_deye_port, TCP_CONNECT_TIMEOUT_MS)) {
    DBG.printf("❌ LSW: connexion échouée à %s:%d\n", cfg_deye_host.c_str(), cfg_deye_port);
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

// ==================== LECTURE GÉNÉRIQUE (selon mode LSE/LSW) ====================

static bool read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response) {
  bool use_lse = settings_get_deye_mode_lse();
  DBG.printf("🔍 READ_BLOCK: mode %s (LSE=%d)\n", use_lse ? "LSE" : "LSW", use_lse);
  if (use_lse) {
    return modbus_tcp_read_block(first_reg, count, rtu_response);
  } else {
    return solarman_read_block(first_reg, count, rtu_response);
  }
}

// ==================== CHARGEMENT DES REGISTRES ====================

static void load_custom_registers() {
  CustomRegisters regs = get_custom_registers();
  REG_PV1_POWER = regs.pv1_power;
  REG_PV2_POWER = regs.pv2_power;
  REG_PV3_POWER = regs.pv3_power;
  REG_PV_DAILY = regs.pv_daily;
  REG_BATTERY_SOC = regs.battery_soc;
  REG_BATTERY_VOLTAGE = regs.battery_voltage;
  REG_BATTERY_POWER = regs.battery_power;
  REG_BATTERY_TEMP = regs.battery_temp;
  REG_GRID_POWER = regs.grid_power;
  REG_GRID_STATUS = regs.grid_status;
  REG_GRID_BUY_DAY = regs.grid_buy_daily;
  REG_GRID_SELL_DAY = regs.grid_sell_daily;
  REG_LOAD_POWER = regs.load_power;
  REG_UPS_POWER = regs.ups_power;
  REG_LOAD_DAY = regs.load_daily;
  REG_DC_TEMP = regs.dc_temp;
  REG_AC_TEMP = regs.ac_temp;
  REG_SMARTLOAD = regs.smartload;
  TCP_CONNECT_TIMEOUT_MS = regs.connect_timeout;
  RESPONSE_WINDOW_MS = regs.response_window;
  FRAME_TIMEOUT_MS = regs.frame_timeout;
  BLOCK_INTERVAL_MS = regs.block_interval;

  // Calcul des blocs (inchangé)
  uint16_t min_reg1 = 999, max_reg1 = 0;
  uint16_t regs1[] = {REG_GRID_BUY_DAY, REG_GRID_SELL_DAY, REG_LOAD_DAY, REG_DC_TEMP, REG_AC_TEMP, REG_PV_DAILY};
  for (int i = 0; i < 6; i++) {
    if (regs1[i] < min_reg1) min_reg1 = regs1[i];
    if (regs1[i] > max_reg1) max_reg1 = regs1[i];
  }
  BLOCK1_START = min_reg1;
  BLOCK1_COUNT = max_reg1 - min_reg1 + 1;

  uint16_t min_reg2 = 999, max_reg2 = 0;
  uint16_t regs2[] = {REG_GRID_POWER, REG_UPS_POWER, REG_LOAD_POWER, REG_BATTERY_TEMP,
                      REG_BATTERY_VOLTAGE, REG_BATTERY_SOC, REG_PV1_POWER, REG_PV2_POWER,
                      REG_PV3_POWER, REG_BATTERY_POWER, REG_GRID_STATUS, REG_SMARTLOAD};
  for (int i = 0; i < 12; i++) {
    if (regs2[i] < min_reg2) min_reg2 = regs2[i];
    if (regs2[i] > max_reg2) max_reg2 = regs2[i];
  }
  BLOCK2_START = min_reg2;
  BLOCK2_COUNT = max_reg2 - min_reg2 + 1;

  DBG.println("=== REGISTRES PERSONNALISES CHARGES ===");
  DBG.printf("PV1=%d PV2=%d PV3=%d\n", REG_PV1_POWER, REG_PV2_POWER, REG_PV3_POWER);
  DBG.printf("Bloc1: %d-%d (%d regs)\n", BLOCK1_START, BLOCK1_START + BLOCK1_COUNT - 1, BLOCK1_COUNT);
  DBG.printf("Bloc2: %d-%d (%d regs)\n", BLOCK2_START, BLOCK2_START + BLOCK2_COUNT - 1, BLOCK2_COUNT);
}

// ==================== DÉCODAGE ====================

static void decode_block1(const uint8_t *rtu) {
  int offset = BLOCK1_START;
  daily_grid_buy = modbus_get_u16_be(rtu, REG_GRID_BUY_DAY - offset);
  daily_grid_buy_valid = true;
  daily_grid_sell = modbus_get_u16_be(rtu, REG_GRID_SELL_DAY - offset);
  daily_grid_sell_valid = true;
  daily_load = modbus_get_u16_be(rtu, REG_LOAD_DAY - offset);
  daily_load_valid = true;
  uint16_t dc_raw = modbus_get_u16_be(rtu, REG_DC_TEMP - offset);
  if (dc_raw != 0 && dc_raw != 1000) main_data.dc_temperature_raw = (int16_t)dc_raw;
  uint16_t ac_raw = modbus_get_u16_be(rtu, REG_AC_TEMP - offset);
  if (ac_raw != 0 && ac_raw != 1000) main_data.ac_temperature_raw = (int16_t)ac_raw;
  daily_solar = modbus_get_u16_be(rtu, REG_PV_DAILY - offset);
  daily_solar_valid = true;
  pv_daily_yield = daily_solar;
  pv_daily_yield_valid = true;
}

static void decode_block2(const uint8_t *rtu) {
  int offset = BLOCK2_START;
  main_data.gen_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  main_data.ups_load_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  main_data.grid_power = (int16_t)modbus_get_u16_be(rtu, REG_GRID_POWER - offset);
  main_data.ups_load_power = (int16_t)modbus_get_u16_be(rtu, REG_UPS_POWER - offset);
  main_data.load_power = (int16_t)modbus_get_u16_be(rtu, REG_LOAD_POWER - offset);
  main_data.bat_temperature_raw = (int16_t)modbus_get_u16_be(rtu, REG_BATTERY_TEMP - offset);
  main_data.bat_voltage_raw = modbus_get_u16_be(rtu, REG_BATTERY_VOLTAGE - offset);
  main_data.bat_soc = modbus_get_u16_be(rtu, REG_BATTERY_SOC - offset);
  main_data.pv1_power = modbus_get_u16_be(rtu, REG_PV1_POWER - offset);
  main_data.pv2_power = modbus_get_u16_be(rtu, REG_PV2_POWER - offset);
  main_data.pv3_power = modbus_get_u16_be(rtu, REG_PV3_POWER - offset);
  main_data.bat_power = (int16_t)modbus_get_u16_be(rtu, REG_BATTERY_POWER - offset);
  main_data.grid_status_raw = modbus_get_u16_be(rtu, REG_GRID_STATUS - offset);
  main_data.smartload_status_raw = modbus_get_u16_be(rtu, REG_SMARTLOAD - offset);
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

// ==================== TÂCHE DE LECTURE ====================

static void solarman_reader_task(void *pvParameters) {
  int current_block = 0;
  uint32_t last_read_time = 0;
  bool first_read_done = false;
  uint32_t startup_time = millis();

  randomSeed(esp_random());

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
      if (now - startup_time < 2000) {
        vTaskDelay(pdMS_TO_TICKS(50));
        continue;
      }
      first_read_done = true;
      DBG.println("=== PREMIERE LECTURE ===");
    } else {
      if (now - last_read_time < BLOCK_INTERVAL_MS) {
        vTaskDelay(pdMS_TO_TICKS(10));
        continue;
      }
    }
    int random_delay = random(0, 200);
    vTaskDelay(pdMS_TO_TICKS(random_delay));

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      bool ok = false;
      if (current_block == 0) {
        uint8_t b1_rtu[5 + BLOCK1_COUNT * 2];
        if (read_block(BLOCK1_START, BLOCK1_COUNT, b1_rtu)) {
          decode_block1(b1_rtu);
          DBG.println("BLOC1 OK");
          ok = true;
        } else {
          DBG.println("BLOC1 echec");
        }
      } else {
        uint8_t b2_rtu[5 + BLOCK2_COUNT * 2];
        if (read_block(BLOCK2_START, BLOCK2_COUNT, b2_rtu)) {
          decode_block2(b2_rtu);
          DBG.println("BLOC2 OK");
          ok = true;
        } else {
          DBG.println("BLOC2 echec");
        }
      }
      if (ok) {
        last_read_time = millis();
        update_dashboard_from_data();
      }
      current_block = (current_block + 1) % 2;
      xSemaphoreGive(data_mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ==================== FONCTIONS PUBLIQUES ====================

void deye_solarman_set_ui_active(bool active) {
  ui_active = active;
}

void deye_solarman_begin() {
  DBG.println("=== DEYE SOLARMAN V5 (LSW) / Modbus TCP (LSE) ===");
  load_custom_registers();

  bool use_lse = settings_get_deye_mode_lse();
  DBG.printf("🔍 Mode lu depuis settings : %s\n", use_lse ? "LSE" : "LSW");
  DBG.printf("🔍 Port configuré : %d\n", cfg_deye_port);

  DBG.printf("Mode de communication : %s\n", use_lse ? "LSE (Ethernet / Modbus TCP)" : "LSW (WiFi / Solarman V5)");

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

// ==================== FONCTIONS POUR L'INTERFACE ====================
bool deye_is_connected() { return main_data_valid; }
bool deye_is_on_grid() { return deye_on_grid_state; }
uint16_t deye_get_pv_daily() { return pv_daily_yield; }
uint16_t deye_get_daily_load() { return daily_load; }
uint16_t deye_get_daily_grid_buy() { return daily_grid_buy; }
uint16_t deye_get_daily_grid_sell() { return daily_grid_sell; }