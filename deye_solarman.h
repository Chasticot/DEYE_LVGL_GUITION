// deye_solarman.h - Version corrigée et unifiée

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <math.h>
#include "config.h"
#include "app_data.h"
#include "settings.h"
#include "ve_deye.h"

// ==================== CONSTANTES ====================
#define DEYE_PORT 8899
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
uint16_t BLOCK3_START = DEYE_EV_BLOCK3_START;
uint16_t BLOCK3_COUNT = DEYE_EV_BLOCK3_COUNT;
static uint16_t block2_base_start = 169;
static uint16_t block2_base_count = 27;
static bool block2_has_ev = false;

uint32_t TCP_CONNECT_TIMEOUT_MS = 10000;
uint32_t RESPONSE_WINDOW_MS = 10000;
uint32_t FRAME_TIMEOUT_MS = 7000;
uint32_t BLOCK_INTERVAL_MS = 3000;
float COEFF_GRID_POWER = 1.0f;
float COEFF_LOAD_POWER = 1.0f;
float COEFF_UPS_POWER = 1.0f;
float COEFF_SMARTLOAD = 1.0f;

#define MODBUS_MAX_READ_REGISTERS 125
#define DATA_STALE_AFTER_MS 120000UL

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
static EvDeyeData ev_deye_data = {};
static QueueHandle_t ev_command_queue = nullptr;

// ==================== SYNCHRONISATION ====================
static SemaphoreHandle_t data_mutex = nullptr;
static volatile bool ui_active = false;
static volatile bool reader_running = false;
static uint32_t last_data_success_ms = 0;

// ==================== DÉCLARATIONS ====================
static void solarman_reader_task(void *pvParameters);

// ==================== FONCTIONS PROTOCOLE (LSW - Solarman V5) ====================

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
  const uint8_t checksum = solarman_checksum(out, pos);
  out[pos++] = checksum;
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
  if (total > 320 || payload_len == 0) return false;
  memcpy(frame, header, 11);
  if (!read_exact(client, frame + 11, payload_len + 2, FRAME_TIMEOUT_MS)) return false;
  *frame_len = total;
  return true;
}

static uint16_t modbus_get_u16_be(const uint8_t *data, uint16_t index) {
  size_t p = 3 + (size_t)index * 2;
  return ((uint16_t)data[p] << 8) | data[p + 1];
}

// Toutes les transactions sont executees par la meme tache, jamais dans LVGL.
static bool solarman_exchange(const uint8_t *request, size_t request_len,
  uint16_t reg, uint16_t count, uint8_t *response, size_t capacity, uint8_t *exception = nullptr) {
  if (exception) *exception = 0;
  if (!deye_modbus_range_valid(reg, count)) return false;
  WiFiClient client;
  uint8_t v5_request[64];
  uint8_t frame[320]; // 125 registres + RTU + enveloppe V5.
  const size_t v5_len = build_solarman_v5_request(request, request_len, v5_request);
  delay(50);
  if (!client.connect(cfg_deye_host.c_str(), DEYE_PORT, TCP_CONNECT_TIMEOUT_MS)) return false;
  if (client.write(v5_request, v5_len) != v5_len) { client.stop(); return false; }
  const uint32_t deadline = millis() + RESPONSE_WINDOW_MS;
  bool found = false;
  while ((int32_t)(deadline - millis()) > 0) {
    if (!client.available()) { delay(2); continue; }
    size_t frame_len = 0;
    if (!receive_one_v5_frame(client, frame, &frame_len)) break;
    if (!deye_v5_reply_matches(frame, frame_len, v5_request)) continue;
    const DeyeRtuResult result = deye_modbus_response(frame + 11, frame_len - 13,
      DEYE_MODBUS_SLAVE_ID, request[1], reg, count, response, capacity, exception);
    if (result == DEYE_RTU_EXCEPTION) {
      DBG.printf("Modbus exception FC%02X R%u : %u\n", request[1], reg, exception ? *exception : 0);
      break;
    }
    if (result == DEYE_RTU_OK) { found = true; break; }
  }
  client.stop();
  return found;
}

static bool solarman_read_block(uint16_t first_reg, uint16_t count, uint8_t *rtu_response,
  uint8_t *exception = nullptr) {
  if (!deye_modbus_range_valid(first_reg, count)) return false;
  uint8_t request[8];
  build_modbus_rtu_read(first_reg, count, request);
  return solarman_exchange(request, sizeof(request), first_reg, count, rtu_response, 5 + size_t(count)*2, exception);
}

static bool solarman_write_register(uint16_t reg, uint16_t value, uint8_t *exception) {
  // Liste fermee : aucun acces en ecriture aux autres reglages de l'onduleur.
  if (reg != DEYE_REG_EV_CHARGE_MODE && reg != DEYE_REG_EV_MAX_CHARGE_POWER) return false;
  uint8_t request[11], response[8];
  deye_modbus_write_one(DEYE_MODBUS_SLAVE_ID, reg, value, request);
  return solarman_exchange(request, sizeof(request), reg, 1, response, sizeof(response), exception);
}

// Base historique conservee hors VE ; extension jusqu'a R260 quand VE est actif.
static void deye_configure_block2(bool enabled) {
  BLOCK2_START = block2_base_start;
  BLOCK2_COUNT = block2_base_count;
  block2_has_ev = false;
  if (!enabled || !deye_modbus_range_valid(BLOCK2_START, BLOCK2_COUNT)) return;
  const uint16_t first = min(BLOCK2_START, DEYE_REG_EV_CHARGE_MODE);
  const uint32_t last = max(uint32_t(BLOCK2_START) + BLOCK2_COUNT - 1,
    uint32_t(DEYE_REG_EV_MAX_CHARGE_POWER));
  const uint32_t count = last - first + 1;
  if (count > MODBUS_MAX_READ_REGISTERS) {
    DBG.println("VE : bloc2 etendu depasse 125 registres, base seule conservee.");
    return;
  }
  BLOCK2_START = first;
  BLOCK2_COUNT = uint16_t(count);
  block2_has_ev = true;
  DBG.printf("VE bloc2 R%u-R%u (%u), bloc3 R%u (%u)\n", BLOCK2_START,
    BLOCK2_START + BLOCK2_COUNT - 1, BLOCK2_COUNT, BLOCK3_START, BLOCK3_COUNT);
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
  COEFF_GRID_POWER = regs.coeff_grid_power;
  COEFF_LOAD_POWER = regs.coeff_load_power;
  COEFF_UPS_POWER = regs.coeff_ups_power;
  COEFF_SMARTLOAD = regs.coeff_smartload;

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
  for (uint8_t i = 0; i < 12; i++) {
    if (regs2[i] < min_reg2) min_reg2 = regs2[i];
    if (regs2[i] > max_reg2) max_reg2 = regs2[i];
  }
  BLOCK2_START = min_reg2;
  BLOCK2_COUNT = max_reg2 - min_reg2 + 1;
  block2_base_start = BLOCK2_START;
  block2_base_count = BLOCK2_COUNT;
  DBG.println("=== REGISTRES PERSONNALISES CHARGES ===");
  DBG.printf("PV1=%d PV2=%d PV3=%d\n", REG_PV1_POWER, REG_PV2_POWER, REG_PV3_POWER);
  DBG.printf("Bloc1: %d-%d (%d regs)\n", BLOCK1_START, BLOCK1_START + BLOCK1_COUNT - 1, BLOCK1_COUNT);
  DBG.printf("Bloc2: %d-%d (%d regs)\n", BLOCK2_START, BLOCK2_START + BLOCK2_COUNT - 1, BLOCK2_COUNT);
  DBG.printf("Timeouts: C=%lu R=%lu F=%lu I=%lu\n",
             (unsigned long)TCP_CONNECT_TIMEOUT_MS,
             (unsigned long)RESPONSE_WINDOW_MS,
             (unsigned long)FRAME_TIMEOUT_MS,
             (unsigned long)BLOCK_INTERVAL_MS);
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
  if (cfg_ev_charger_enabled && block2_has_ev) {
    ev_deye_data.mode_raw = modbus_get_u16_be(rtu, DEYE_REG_EV_CHARGE_MODE - offset);
    ev_deye_data.max_charge_power_raw = modbus_get_u16_be(rtu, DEYE_REG_EV_MAX_CHARGE_POWER - offset);
    ev_deye_data.valid = true;
    ev_deye_data.settings_updated_ms = millis();
  }
  main_data_valid = true;
}

static void decode_block3(const uint8_t *rtu) {
  ev_deye_data.requested_power_w = modbus_get_u16_be(rtu, DEYE_REG_EV_REQUESTED_POWER - BLOCK3_START);
  ev_deye_data.requested_power_valid = true;
  ev_deye_data.requested_updated_ms = millis();
}

static int16_t scaled_power(int16_t raw, float coefficient, float scale) {
  const float value = raw * coefficient * scale;
  if (value >= INT16_MAX) return INT16_MAX;
  if (value <= INT16_MIN) return INT16_MIN;
  return (int16_t)lroundf(value);
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
  dashboard_data.grid_power = scaled_power(main_data.grid_power, COEFF_GRID_POWER, 10.0f);
  dashboard_data.load_power = scaled_power(main_data.load_power, COEFF_LOAD_POWER, 10.0f);
  dashboard_data.ups_power = scaled_power(main_data.ups_load_power, COEFF_UPS_POWER, 1.0f);
  dashboard_data.dc_temperature = (main_data.dc_temperature_raw - 1000) * 0.1f;
  dashboard_data.ac_temperature = (main_data.ac_temperature_raw - 1000) * 0.1f;
  dashboard_data.smartload_on = ((main_data.smartload_status_raw & 0x01) * COEFF_SMARTLOAD) >= 0.5f;
  deye_on_grid_state = (main_data.grid_status_raw == 1);
  if (daily_solar_valid) {
    pv_daily_yield = daily_solar;
    pv_daily_yield_valid = true;
  }
}

static bool deye_copy_snapshot(
  DashboardData *out,
  uint16_t *pv_daily,
  bool *pv_daily_valid_out,
  uint16_t *daily_load_out,
  uint16_t *daily_buy_out,
  uint16_t *daily_sell_out,
  bool *on_grid_out
) {
  if (out == nullptr || pv_daily == nullptr || pv_daily_valid_out == nullptr ||
      daily_load_out == nullptr || daily_buy_out == nullptr || daily_sell_out == nullptr ||
      on_grid_out == nullptr || data_mutex == nullptr ||
      xSemaphoreTake(data_mutex, pdMS_TO_TICKS(5)) != pdTRUE) {
    return false;
  }

  *out = dashboard_data;
  *pv_daily = pv_daily_yield;
  *pv_daily_valid_out = pv_daily_yield_valid;
  *daily_load_out = daily_load;
  *daily_buy_out = daily_grid_buy;
  *daily_sell_out = daily_grid_sell;
  *on_grid_out = deye_on_grid_state;
  const bool fresh = main_data_valid && (uint32_t)(millis() - last_data_success_ms) <= DATA_STALE_AFTER_MS;
  out->valid = out->valid && fresh;
  xSemaphoreGive(data_mutex);
  return true;
}

// Copie atomique des donnees VE. Lorsque le chargeur est desactive dans les
// reglages, cette fonction reste volontairement inutilisable.
bool deye_copy_ev_snapshot(EvDeyeData *out) {
  if (out == nullptr || !cfg_ev_charger_enabled || data_mutex == nullptr ||
      xSemaphoreTake(data_mutex, pdMS_TO_TICKS(5)) != pdTRUE) {
    return false;
  }

  *out = ev_deye_data;
  out->valid = out->valid && (uint32_t)(millis() - out->settings_updated_ms) <= DEYE_EV_FRESH_MS;
  out->requested_power_valid = out->requested_power_valid &&
    (uint32_t)(millis() - out->requested_updated_ms) <= DEYE_EV_FRESH_MS;
  xSemaphoreGive(data_mutex);
  return true;
}

// ==================== COMMANDES VE ====================
static void deye_ev_command_result(EvDeyeCommandState state, const char *message, uint8_t exception = 0) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);
  ev_deye_data.command_state = state;
  ev_deye_data.modbus_exception = exception;
  snprintf(ev_deye_data.command_message, sizeof(ev_deye_data.command_message), "%s", message);
  xSemaphoreGive(data_mutex);
  DBG.printf("VE commande : %s (exception %u)\n", message, exception);
}

bool deye_submit_ev_command(EvDeyeCommand command) {
  if (!cfg_ev_charger_enabled || ev_command_queue == nullptr || WiFi.status() != WL_CONNECTED ||
      (!command.set_power && !command.set_mode) ||
      (command.set_mode && command.mode != 1 && command.mode != 2) ||
      (command.set_power && (deye_ev_max_power_w(command.power_raw) < 1400 ||
       deye_ev_max_power_w(command.power_raw) > DEYE_EV_INSTALLATION_MAX_POWER_W))) return false;
  if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(5)) != pdTRUE) return false;
  const bool ready = ev_deye_data.valid &&
    uint32_t(millis() - ev_deye_data.settings_updated_ms) <= DEYE_EV_FRESH_MS &&
    !deye_ev_command_busy(ev_deye_data.command_state);
  command.queued_ms = millis();
  const bool queued = ready && xQueueSend(ev_command_queue, &command, 0) == pdTRUE;
  if (queued) {
    ev_deye_data.command_state = EV_COMMAND_QUEUED;
    ev_deye_data.modbus_exception = 0;
    snprintf(ev_deye_data.command_message, sizeof(ev_deye_data.command_message), "Commande en attente...");
  }
  xSemaphoreGive(data_mutex);
  return queued;
}

// Relecture du bloc2 entier : meme taille que la supervision, y compris apres ecriture.
static bool deye_ev_refresh_settings(uint16_t *mode, uint16_t *power, uint8_t *exception) {
  if (!block2_has_ev) return false;
  uint8_t rtu[255];
  if (!solarman_read_block(BLOCK2_START, BLOCK2_COUNT, rtu, exception)) {
    xSemaphoreTake(data_mutex, portMAX_DELAY);
    ev_deye_data.valid = false;
    xSemaphoreGive(data_mutex);
    return false;
  }
  *mode = modbus_get_u16_be(rtu, DEYE_REG_EV_CHARGE_MODE - BLOCK2_START);
  *power = modbus_get_u16_be(rtu, DEYE_REG_EV_MAX_CHARGE_POWER - BLOCK2_START);
  xSemaphoreTake(data_mutex, portMAX_DELAY);
  decode_block2(rtu);
  last_data_success_ms = millis();
  update_dashboard_from_data();
  xSemaphoreGive(data_mutex);
  return true;
}

static bool deye_process_ev_command() {
  EvDeyeCommand command = {};
  if (!ev_command_queue || xQueueReceive(ev_command_queue, &command, 0) != pdTRUE) return false;
  if (!cfg_ev_charger_enabled || !block2_has_ev || WiFi.status() != WL_CONNECTED ||
      uint32_t(millis() - command.queued_ms) > DEYE_EV_COMMAND_MAX_AGE_MS) {
    deye_ev_command_result(EV_COMMAND_CANCELLED, "Commande annulee : liaison ou delai.");
    return true;
  }
  deye_ev_command_result(EV_COMMAND_RUNNING, "Ecriture et verification...");
  uint16_t mode = 0, power = 0;
  uint8_t exception = 0;
  if (!deye_ev_refresh_settings(&mode, &power, &exception)) {
    deye_ev_command_result(EV_COMMAND_FAILED, "Lecture prealable impossible. Aucune ecriture.", exception);
    return true;
  }
  bool power_confirmed = false;
  if (command.set_power) {
    if (power != command.power_raw) {
      const bool ack = solarman_write_register(DEYE_REG_EV_MAX_CHARGE_POWER, command.power_raw, &exception);
      const uint8_t write_exception = exception;
      const bool read_back = deye_ev_refresh_settings(&mode, &power, &exception);
      // Un ACK perdu n'autorise jamais une repetition automatique de l'ecriture.
      if (write_exception || !read_back || power != command.power_raw) {
        deye_ev_command_result(EV_COMMAND_FAILED, "Puissance non confirmee. Verifier les valeurs lues.",
          write_exception ? write_exception : exception);
        return true;
      }
      if (!ack) DBG.println("VE : ACK perdu, puissance confirmee par relecture.");
    }
    power_confirmed = true;
  }
  if (command.set_mode) {
    // Lecture recente pour conserver port, SOC et tous les bits non documentes.
    if (!deye_ev_refresh_settings(&mode, &power, &exception)) {
      deye_ev_command_result(power_confirmed ? EV_COMMAND_PARTIAL : EV_COMMAND_FAILED,
        "Mode non applique : lecture impossible.", exception);
      return true;
    }
    const uint16_t new_mode = deye_ev_replace_mode(mode, command.mode);
    if (new_mode != mode) {
      solarman_write_register(DEYE_REG_EV_CHARGE_MODE, new_mode, &exception);
      const uint8_t write_exception = exception;
      const bool read_back = deye_ev_refresh_settings(&mode, &power, &exception);
      if (write_exception || !read_back || (mode & 3) != command.mode) {
        deye_ev_command_result(power_confirmed ? EV_COMMAND_PARTIAL : EV_COMMAND_FAILED,
          "Mode non confirme. Verifier les valeurs lues.", write_exception ? write_exception : exception);
        return true;
      }
    }
  }
  if (command.set_power && power != command.power_raw) {
    deye_ev_command_result(EV_COMMAND_PARTIAL, "Mode applique, mais puissance modifiee par l'onduleur.");
    return true;
  }
  deye_ev_command_result(EV_COMMAND_CONFIRMED, "Reglages confirmes par l'onduleur.");
  return true;
}

// ==================== TACHE DE LECTURE ====================
static void solarman_reader_task(void *pvParameters) {
  (void)pvParameters;
  uint8_t current_block = 0;
  uint32_t last_read_time = 0;
  const uint32_t startup_time = millis();
  bool first_read_done = false;
  bool previous_ev_enabled = !cfg_ev_charger_enabled;
  randomSeed(analogRead(0) + millis());
  while (true) {
    if (previous_ev_enabled != cfg_ev_charger_enabled) {
      previous_ev_enabled = cfg_ev_charger_enabled;
      deye_configure_block2(previous_ev_enabled);
      current_block = 0;
      xSemaphoreTake(data_mutex, portMAX_DELAY);
      ev_deye_data.valid = false;
      ev_deye_data.requested_power_valid = false;
      xSemaphoreGive(data_mutex);
    }
    // Une commande en attente expire meme hors ligne ou dans les reglages.
    if (deye_process_ev_command()) { last_read_time = millis(); continue; }
    if (ui_active || WiFi.status() != WL_CONNECTED) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }
    const uint32_t now = millis();
    if ((!first_read_done && uint32_t(now - startup_time) < 2000) ||
        (first_read_done && uint32_t(now - last_read_time) < BLOCK_INTERVAL_MS)) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    first_read_done = true;
    vTaskDelay(pdMS_TO_TICKS(random(0, 200)));
    uint8_t rtu[255];
    uint8_t exception = 0;
    const uint16_t start = current_block == 0 ? BLOCK1_START : current_block == 1 ? BLOCK2_START : BLOCK3_START;
    const uint16_t count = current_block == 0 ? BLOCK1_COUNT : current_block == 1 ? BLOCK2_COUNT : BLOCK3_COUNT;
    // Aucun mutex de donnees n'est garde durant les delais TCP/Modbus.
    const bool ok = solarman_read_block(start, count, rtu, &exception);
    last_read_time = millis(); // Temporisation aussi apres un echec.
    xSemaphoreTake(data_mutex, portMAX_DELAY);
    if (ok) {
      if (current_block == 0) decode_block1(rtu);
      else if (current_block == 1) { decode_block2(rtu); last_data_success_ms = last_read_time; }
      else decode_block3(rtu);
      update_dashboard_from_data();
    } else {
      if (current_block == 1) ev_deye_data.valid = false;
      if (current_block == 2) ev_deye_data.requested_power_valid = false;
    }
    if (ok && current_block == 1 && block2_has_ev) {
      DBG.printf("VE R259=0x%04X mode=%s R260=%u (%lu W)\n", ev_deye_data.mode_raw,
        deye_ev_mode_name(ev_deye_data.mode_raw), ev_deye_data.max_charge_power_raw,
        (unsigned long)deye_ev_max_power_w(ev_deye_data.max_charge_power_raw));
    }
    if (ok && current_block == 2) DBG.printf("VE R709 consigne=%u W\n", ev_deye_data.requested_power_w);
    xSemaphoreGive(data_mutex);
    DBG.printf("BLOC%u R%u+%u %s (exception %u)\n", current_block + 1, start, count, ok ? "OK" : "echec", exception);
    current_block = (current_block + 1) % (cfg_ev_charger_enabled ? 3 : 2);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ==================== FONCTIONS PUBLIQUES ====================

void deye_solarman_set_ui_active(bool active) {
  ui_active = active;
}

void deye_solarman_set_touch_active(bool active) {
  // Compatibilite avec le pilote tactile actuel : le lecteur historique LSW
  // ne suspendait pas ses lectures sur cet indicateur distinct.
  (void)active;
}

void deye_solarman_begin() {
  DBG.println("=== DEYE SOLARMAN V5 ===");
  load_custom_registers();

  main_data_valid = false;
  dashboard_data.valid = false;
  pv_daily_yield_valid = false;
  ev_deye_data = {};
  last_data_success_ms = 0;

  data_mutex = xSemaphoreCreateMutex();
  if (data_mutex == NULL) {
    DBG.println("ERREUR: creation mutex");
    return;
  }
  ev_command_queue = xQueueCreate(1, sizeof(EvDeyeCommand));
  if (ev_command_queue == nullptr) DBG.println("VE : file commandes indisponible (lecture seule).");

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
