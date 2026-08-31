#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "app_data.h"
#include "settings.h"

// ==================== CONSTANTES ====================
#define DEYE_PORT 8899
#define DEYE_MODBUS_SLAVE_ID 1

#define MODBUS_READ_MS 5000
#define TCP_CONNECT_TIMEOUT_MS 3000
#define RESPONSE_WINDOW_MS 1800
#define FRAME_TIMEOUT_MS 700

// ==================== PLAYES DE LECTURE ====================
#define MAIN_FIRST_REG 150
#define MAIN_REG_COUNT 46

// ==================== STRUCTURES ====================
struct MainData {
  uint16_t pv1_power;
  uint16_t pv2_power;
  uint16_t pv3_power;
  uint16_t bat_voltage_raw;
  uint16_t bat_soc;
  int16_t bat_power;
  int16_t bat_temperature_raw;
  int16_t grid_power;
  int16_t load_power;
  int16_t ups_load_power;
  uint16_t ups_load_voltage_raw;
  uint16_t gen_voltage_raw;
  uint16_t grid_status_raw;
  uint16_t smartload_status_raw;
  int16_t dc_temperature_raw;
  int16_t ac_temperature_raw;
};

// ==================== VARIABLES ====================
static MainData main_data;
static bool main_data_valid = false;
bool deye_on_grid_state = true;

static WiFiClient deye_client;
static uint32_t last_modbus_read = 0;
static uint8_t request_sequence = 0;

static uint16_t pv_daily_yield = 0;
static bool pv_daily_yield_valid = false;

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
  out[pos++] = request_sequence++;
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

// ==================== LECTURE BLOC ====================
static bool solarman_read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response) {
  uint8_t rtu_request[8];
  uint8_t v5_request[64];
  uint8_t frame[256];
  size_t frame_len = 0;

  build_modbus_rtu_read(first_reg, count, rtu_request);
  size_t request_len = build_solarman_v5_request(rtu_request, sizeof(rtu_request), v5_request);

  if (deye_client.connected()) {
    deye_client.stop();
  }

  if (!deye_client.connect(cfg_deye_host.c_str(), DEYE_PORT, TCP_CONNECT_TIMEOUT_MS)) {
    return false;
  }

  deye_client.write(v5_request, request_len);

  uint32_t deadline = millis() + RESPONSE_WINDOW_MS;
  bool found = false;

  while ((int32_t)(deadline - millis()) > 0) {
    if (!deye_client.available()) {
      delay(2);
      continue;
    }

    if (!receive_one_v5_frame(deye_client, frame, &frame_len)) {
      break;
    }

    if (get_rtu_from_v5_frame(frame, frame_len, count, rtu_response)) {
      found = true;
      break;
    }
  }

  deye_client.stop();
  return found;
}

// ==================== DÉCODAGE ====================
static bool decode_main_block(const uint8_t *rtu) {
  MainData next;
  
  next.gen_voltage_raw = modbus_get_u16_be(rtu, 0);
  next.ups_load_voltage_raw = modbus_get_u16_be(rtu, 0);
  next.grid_power = (int16_t)modbus_get_u16_be(rtu, 19);
  next.load_power = (int16_t)modbus_get_u16_be(rtu, 22);
  next.ups_load_power = (int16_t)modbus_get_u16_be(rtu, 25);
  next.bat_temperature_raw = (int16_t)modbus_get_u16_be(rtu, 32);
  next.bat_voltage_raw = modbus_get_u16_be(rtu, 33);
  next.bat_soc = modbus_get_u16_be(rtu, 34);
  next.pv1_power = modbus_get_u16_be(rtu, 36);
  next.pv2_power = modbus_get_u16_be(rtu, 37);
  next.pv3_power = modbus_get_u16_be(rtu, 38);
  next.bat_power = (int16_t)modbus_get_u16_be(rtu, 40);
  next.grid_status_raw = modbus_get_u16_be(rtu, 44);
  next.smartload_status_raw = modbus_get_u16_be(rtu, 45);
  
  main_data = next;
  main_data_valid = true;
  
  DBG.printf("PV1=%dW PV2=%dW SOC=%d%% Grid=%dW Load=%dW GridStatus=%d SmartLoad=%d\n",
             main_data.pv1_power, main_data.pv2_power,
             main_data.bat_soc, main_data.grid_power, main_data.load_power,
             main_data.grid_status_raw, main_data.smartload_status_raw);
  
  return true;
}

// ==================== LECTURE TEMPÉRATURES (INDIVIDUELLE) ====================
static void solarman_read_temperatures() {
    uint8_t temp_rtu[5 + 1 * 2];
    
    // 1. DC Temp (registre 90) - lecture individuelle
    if (solarman_read_block(90, 1, temp_rtu)) {
        main_data.dc_temperature_raw = (int16_t)modbus_get_u16_be(temp_rtu, 0);
        DBG.printf("DC Temp: %.1f°C\n", (main_data.dc_temperature_raw - 1000) * 0.1f);
    } else {
        DBG.println("DC Temp: echec");
    }
    
    delay(100);
    
    // 2. AC Temp (registre 91) - lecture individuelle
    if (solarman_read_block(91, 1, temp_rtu)) {
        main_data.ac_temperature_raw = (int16_t)modbus_get_u16_be(temp_rtu, 0);
        DBG.printf("AC Temp: %.1f°C\n", (main_data.ac_temperature_raw - 1000) * 0.1f);
    } else {
        DBG.println("AC Temp: echec");
    }
}

// ==================== MISE À JOUR DASHBOARD ====================
// ==================== MISE À JOUR DASHBOARD ====================
static void update_dashboard_from_data() {
  if (!main_data_valid) {
    dashboard_data.valid = false;
    return;
  }

  dashboard_data.valid = true;
  
  // PV
  dashboard_data.pv1_w = main_data.pv1_power;
  dashboard_data.pv2_w = main_data.pv2_power;
  dashboard_data.pv3_w = main_data.pv3_power;
  
  // Batterie
  dashboard_data.battery_soc = main_data.bat_soc;
  dashboard_data.battery_voltage = main_data.bat_voltage_raw * 0.01f;
  dashboard_data.battery_power = main_data.bat_power;
  dashboard_data.battery_temperature = (main_data.bat_temperature_raw - 1000) * 0.1f;
  
  // Réseau / Charge
  dashboard_data.grid_power = main_data.grid_power * 10;
  dashboard_data.load_power = main_data.load_power * 10;
  dashboard_data.ups_power = main_data.ups_load_power;
  dashboard_data.ups_voltage = main_data.ups_load_voltage_raw * 0.1f;
  
  // Températures
  dashboard_data.dc_temperature = (main_data.dc_temperature_raw - 1000) * 0.1f;
  dashboard_data.ac_temperature = (main_data.ac_temperature_raw - 1000) * 0.1f;
  
  /// =============================================
  // SMARTLOAD - Registre 195 (0x00C3)
  // =============================================
  // ON si le bit 0 (0x01) est activé -> relais actionné
  dashboard_data.smartload_on = (main_data.smartload_status_raw & 0x01) == 0x01;

  // Debug
  DBG.printf("SmartLoad: raw=%d (0x%02X) - Bit0=%d -> %s\n",
           main_data.smartload_status_raw,
           main_data.smartload_status_raw,
           main_data.smartload_status_raw & 0x01,
           dashboard_data.smartload_on ? "ON" : "OFF");
  
  // =============================================
  // ON GRID / OFF GRID - Registre 194
  // =============================================
  deye_on_grid_state = (main_data.grid_status_raw == 1);
  
  DBG.printf("Grid=%dW Load=%dW GridStatus=%d (%s) SmartLoad=%s (GEN=%d)\n",
             dashboard_data.grid_power,
             dashboard_data.load_power,
             main_data.grid_status_raw,
             deye_on_grid_state ? "ON GRID" : "OFF GRID",
             dashboard_data.smartload_on ? "ON" : "OFF",
             main_data.gen_voltage_raw);
}

// ==================== FONCTIONS PUBLIQUES ====================
static void deye_solarman_begin() {
  DBG.println("=== DEYE SOLARMAN V5 ===");
  DBG.print("Host: "); DBG.println(cfg_deye_host);
  DBG.print("Port: "); DBG.println(DEYE_PORT);
  DBG.print("Logger Serial: "); DBG.println(cfg_logger_serial);
  
  main_data_valid = false;
  dashboard_data.valid = false;
  pv_daily_yield_valid = false;
}

static void deye_solarman_process() {
  if (WiFi.status() != WL_CONNECTED) {
    dashboard_data.valid = false;
    return;
  }

  uint32_t now = millis();
  if (now - last_modbus_read < MODBUS_READ_MS) return;
  last_modbus_read = now;

  // =============================================
  // 1. LECTURE BLOC PRINCIPAL (150-196)
  // =============================================
  uint8_t main_rtu[5 + MAIN_REG_COUNT * 2];
  bool main_ok = solarman_read_block(MAIN_FIRST_REG, MAIN_REG_COUNT, main_rtu);

  if (main_ok) {
    decode_main_block(main_rtu);
    DBG.println("MAIN: OK");
  } else {
    dashboard_data.valid = false;
    DBG.println("MAIN: echec");
    return;
  }

  delay(100);

  // =============================================
  // 2. LECTURE TEMPÉRATURES (INDIVIDUELLE)
  // =============================================
  solarman_read_temperatures();

  update_dashboard_from_data();
}

// ==================== FONCTIONS POUR L'INTERFACE ====================
static bool deye_is_connected() {
  return main_data_valid;
}

static bool deye_is_on_grid() {
  return deye_on_grid_state;
}

static uint16_t deye_get_pv_daily() {
  return 0;
}
