#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "app_data.h"
#include "settings.h"

// ==================== CONSTANTES (comme Jeedom) ====================
#define DEYE_PORT 8899
#define DEYE_MODBUS_SLAVE_ID 1

#define MODBUS_READ_MS 15000
#define TCP_CONNECT_TIMEOUT_MS 15000
#define RESPONSE_WINDOW_MS 15000
#define FRAME_TIMEOUT_MS 5000

// ==================== PLAYES DE LECTURE (comme Jeedom) ====================
#define BLOCK1_START 3
#define BLOCK1_COUNT 110   // 3 à 112

#define BLOCK2_START 150
#define BLOCK2_COUNT 100   // 150 à 249

#define BLOCK3_START 250
#define BLOCK3_COUNT 30    // 250 à 279

// ==================== STRUCTURES ====================
struct MainData {
  // PV (registres 186, 187, 188)
  uint16_t pv1_power;
  uint16_t pv2_power;
  uint16_t pv3_power;
  
  // Batterie (registres 182, 183, 184, 190)
  int16_t bat_temperature_raw;
  uint16_t bat_voltage_raw;
  uint16_t bat_soc;
  int16_t bat_power;
  
  // Réseau (registres 169, 172, 175, 150)
  int16_t grid_power;
  int16_t load_power;
  int16_t ups_load_power;
  uint16_t ups_load_voltage_raw;
  uint16_t gen_voltage_raw;
  
  // Statuts (registres 194, 195)
  uint16_t grid_status_raw;
  uint16_t smartload_status_raw;
  
  // Températures (registres 90, 91)
  int16_t dc_temperature_raw;
  int16_t ac_temperature_raw;
  
  // Données journalières (registres 76, 77, 84, 108)
  uint16_t daily_grid_buy;
  uint16_t daily_grid_sell;
  uint16_t daily_load;
  uint16_t daily_solar;
};

// ==================== VARIABLES ====================
static MainData main_data;
static bool main_data_valid = false;
bool deye_on_grid_state = true;

static WiFiClient deye_client;
static uint32_t last_modbus_read = 0;
static uint8_t request_sequence = 0;

// Données journalières pour l'interface
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
    delay(100);
  }

  if (!deye_client.connect(cfg_deye_host.c_str(), DEYE_PORT, TCP_CONNECT_TIMEOUT_MS)) {
    return false;
  }

  deye_client.write(v5_request, request_len);

  uint32_t deadline = millis() + RESPONSE_WINDOW_MS;
  bool found = false;

  while ((int32_t)(deadline - millis()) > 0) {
    if (!deye_client.available()) {
      delay(5);
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

// ==================== DÉCODAGE BLOC 1 (3-112) ====================
static bool decode_block1(const uint8_t *rtu) {
  int offset = BLOCK1_START; // 3
  
  // daily_grid_buy (registre 76)
  daily_grid_buy = modbus_get_u16_be(rtu, 76 - offset);
  daily_grid_buy_valid = true;
  DBG.printf("Grid Buy: %.1f kWh\n", daily_grid_buy * 0.1f);
  
  // daily_grid_sell (registre 77)
  daily_grid_sell = modbus_get_u16_be(rtu, 77 - offset);
  daily_grid_sell_valid = true;
  DBG.printf("Grid Sell: %.1f kWh\n", daily_grid_sell * 0.1f);
  
  // daily_load (registre 84)
  daily_load = modbus_get_u16_be(rtu, 84 - offset);
  daily_load_valid = true;
  DBG.printf("Daily Load: %.1f kWh\n", daily_load * 0.1f);
  
  // dc_temp (registre 90)
  uint16_t dc_raw = modbus_get_u16_be(rtu, 90 - offset);
  if (dc_raw != 0 && dc_raw != 1000) {
    main_data.dc_temperature_raw = (int16_t)dc_raw;
    DBG.printf("DC Temp: %.1f°C\n", (main_data.dc_temperature_raw - 1000) * 0.1f);
  }
  
  // ac_temp (registre 91)
  uint16_t ac_raw = modbus_get_u16_be(rtu, 91 - offset);
  if (ac_raw != 0 && ac_raw != 1000) {
    main_data.ac_temperature_raw = (int16_t)ac_raw;
    DBG.printf("AC Temp: %.1f°C\n", (main_data.ac_temperature_raw - 1000) * 0.1f);
  }
  
  // daily_solar (registre 108)
  daily_solar = modbus_get_u16_be(rtu, 108 - offset);
  daily_solar_valid = true;
  pv_daily_yield = daily_solar;
  pv_daily_yield_valid = true;
  DBG.printf("Daily Solar: %.1f kWh\n", daily_solar * 0.1f);
  
  return true;
}

// ==================== DÉCODAGE BLOC 2 (150-249) ====================
static bool decode_block2(const uint8_t *rtu) {
  int offset = BLOCK2_START; // 150
  
  // Gen Voltage / UPS Voltage (registre 150)
  main_data.gen_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  main_data.ups_load_voltage_raw = modbus_get_u16_be(rtu, 150 - offset);
  
  // Grid Power (registre 169)
  main_data.grid_power = (int16_t)modbus_get_u16_be(rtu, 169 - offset);
  
  // Load Power (registre 172)
  main_data.load_power = (int16_t)modbus_get_u16_be(rtu, 172 - offset);
  
  // UPS Load Power (registre 175)
  main_data.ups_load_power = (int16_t)modbus_get_u16_be(rtu, 175 - offset);
  
  // Battery Temperature (registre 182)
  main_data.bat_temperature_raw = (int16_t)modbus_get_u16_be(rtu, 182 - offset);
  
  // Battery Voltage (registre 183)
  main_data.bat_voltage_raw = modbus_get_u16_be(rtu, 183 - offset);
  
  // Battery SOC (registre 184)
  main_data.bat_soc = modbus_get_u16_be(rtu, 184 - offset);
  
  // PV1 (registre 186)
  main_data.pv1_power = modbus_get_u16_be(rtu, 186 - offset);
  
  // PV2 (registre 187)
  main_data.pv2_power = modbus_get_u16_be(rtu, 187 - offset);
  
  // PV3 (registre 188)
  main_data.pv3_power = modbus_get_u16_be(rtu, 188 - offset);
  
  // Battery Power (registre 190)
  main_data.bat_power = (int16_t)modbus_get_u16_be(rtu, 190 - offset);
  
  // Grid Status (registre 194)
  main_data.grid_status_raw = modbus_get_u16_be(rtu, 194 - offset);
  
  // SmartLoad (registre 195)
  main_data.smartload_status_raw = modbus_get_u16_be(rtu, 195 - offset);
  
  main_data_valid = true;
  
  DBG.printf("BLOC2: PV1=%dW PV2=%dW PV3=%dW SOC=%d%% Grid=%dW Load=%dW GridStatus=%d SmartLoad=%d\n",
             main_data.pv1_power, main_data.pv2_power, main_data.pv3_power,
             main_data.bat_soc, main_data.grid_power, main_data.load_power,
             main_data.grid_status_raw, main_data.smartload_status_raw);
  
  return true;
}

// ==================== DÉCODAGE BLOC 3 (250-279) ====================
static bool decode_block3(const uint8_t *rtu) {
  // Plages horaires - pour l'instant on ne fait rien
  DBG.println("BLOC3 (250-279): OK");
  return true;
}

// ==================== MISE À JOUR DASHBOARD ====================
static void update_dashboard_from_data() {
  if (!main_data_valid) {
    // Garder les dernières valeurs
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
  
  // SmartLoad - valeur brute (comme Jeedom)
  // 0 = OFF, 16 ou 17 = ON selon le bit 0
  dashboard_data.smartload_on = (main_data.smartload_status_raw & 0x01) == 0x01;
  
  // ON GRID / OFF GRID
  deye_on_grid_state = (main_data.grid_status_raw == 1);
  
  // PV Daily
  if (daily_solar_valid) {
    pv_daily_yield = daily_solar;
    pv_daily_yield_valid = true;
  }
  
  DBG.printf("Grid=%dW Load=%dW GridStatus=%d (%s) SmartLoad=%s DailySolar=%.1fkWh\n",
             dashboard_data.grid_power,
             dashboard_data.load_power,
             main_data.grid_status_raw,
             deye_on_grid_state ? "ON GRID" : "OFF GRID",
             dashboard_data.smartload_on ? "ON" : "OFF",
             daily_solar * 0.1f);
}

// ==================== FONCTIONS PUBLIQUES ====================
static void deye_solarman_begin() {
  DBG.println("=== DEYE SOLARMAN V5 (3 blocs comme Jeedom) ===");
  DBG.printf("Bloc1: %d-%d (%d regs)\n", BLOCK1_START, BLOCK1_START + BLOCK1_COUNT - 1, BLOCK1_COUNT);
  DBG.printf("Bloc2: %d-%d (%d regs)\n", BLOCK2_START, BLOCK2_START + BLOCK2_COUNT - 1, BLOCK2_COUNT);
  DBG.printf("Bloc3: %d-%d (%d regs)\n", BLOCK3_START, BLOCK3_START + BLOCK3_COUNT - 1, BLOCK3_COUNT);
  
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

  DBG.println("=== CYCLE DE LECTURE (3 blocs) ===");

  // =============================================
  // 1. BLOC 1 (3-112)
  // =============================================
  uint8_t b1_rtu[5 + BLOCK1_COUNT * 2];
  if (solarman_read_block(BLOCK1_START, BLOCK1_COUNT, b1_rtu)) {
    decode_block1(b1_rtu);
    DBG.println("BLOC1 (3-112): OK");
  } else {
    DBG.println("BLOC1 (3-112): echec");
  }

  delay(300);

  // =============================================
  // 2. BLOC 2 (150-249)
  // =============================================
  uint8_t b2_rtu[5 + BLOCK2_COUNT * 2];
  if (solarman_read_block(BLOCK2_START, BLOCK2_COUNT, b2_rtu)) {
    decode_block2(b2_rtu);
    DBG.println("BLOC2 (150-249): OK");
  } else {
    DBG.println("BLOC2 (150-249): echec");
  }

  delay(300);

  // =============================================
  // 3. BLOC 3 (250-279) - Optionnel
  // =============================================
  uint8_t b3_rtu[5 + BLOCK3_COUNT * 2];
  if (solarman_read_block(BLOCK3_START, BLOCK3_COUNT, b3_rtu)) {
    decode_block3(b3_rtu);
    DBG.println("BLOC3 (250-279): OK");
  } else {
    DBG.println("BLOC3 (250-279): echec");
  }

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
  return pv_daily_yield;
}

static uint16_t deye_get_daily_load() {
  return daily_load;
}

static uint16_t deye_get_daily_grid_buy() {
  return daily_grid_buy;
}

static uint16_t deye_get_daily_grid_sell() {
  return daily_grid_sell;
}
