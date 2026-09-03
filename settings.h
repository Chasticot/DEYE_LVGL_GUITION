// settings.h - Version simplifiée pour le mode LSE/LSW

#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

static Preferences preferences;

static String cfg_wifi_ssid;
static String cfg_wifi_password;
static String cfg_deye_host;
static uint16_t cfg_deye_port = 8899;
static uint32_t cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
static String cfg_ntp_primary;
static String cfg_ntp_secondary;
static String cfg_tz_rule;

// ==================== STRUCTURE POUR LES REGISTRES PERSONNALISÉS ====================
struct CustomRegisters {
  uint16_t pv1_power;
  uint16_t pv2_power;
  uint16_t pv3_power;
  uint16_t pv_daily;
  uint16_t battery_soc;
  uint16_t battery_voltage;
  uint16_t battery_power;
  uint16_t battery_temp;
  uint16_t grid_power;
  uint16_t grid_status;
  uint16_t grid_buy_daily;
  uint16_t grid_sell_daily;
  uint16_t load_power;
  uint16_t ups_power;
  uint16_t load_daily;
  uint16_t dc_temp;
  uint16_t ac_temp;
  uint16_t smartload;
  uint32_t connect_timeout;
  uint32_t response_window;
  uint32_t frame_timeout;
  uint32_t block_interval;
  // Coefficients
  float coeff_grid_power;
  float coeff_load_power;
  float coeff_ups_power;
  float coeff_smartload;
};

// ==================== FONCTIONS DE SAUVEGARDE DES REGISTRES ====================

static void settings_save_registers(const CustomRegisters &regs) {
  preferences.begin("deye-ui", false);
  
  preferences.putUShort("reg_pv1", regs.pv1_power);
  preferences.putUShort("reg_pv2", regs.pv2_power);
  preferences.putUShort("reg_pv3", regs.pv3_power);
  preferences.putUShort("reg_pvd", regs.pv_daily);
  preferences.putUShort("reg_bat_soc", regs.battery_soc);
  preferences.putUShort("reg_bat_v", regs.battery_voltage);
  preferences.putUShort("reg_bat_p", regs.battery_power);
  preferences.putUShort("reg_bat_t", regs.battery_temp);
  preferences.putUShort("reg_grid_p", regs.grid_power);
  preferences.putUShort("reg_grid_s", regs.grid_status);
  preferences.putUShort("reg_grid_buy", regs.grid_buy_daily);
  preferences.putUShort("reg_grid_sell", regs.grid_sell_daily);
  preferences.putUShort("reg_load_p", regs.load_power);
  preferences.putUShort("reg_ups_p", regs.ups_power);
  preferences.putUShort("reg_load_d", regs.load_daily);
  preferences.putUShort("reg_dc_t", regs.dc_temp);
  preferences.putUShort("reg_ac_t", regs.ac_temp);
  preferences.putUShort("reg_smart", regs.smartload);
  preferences.putUInt("reg_conn_t", regs.connect_timeout);
  preferences.putUInt("reg_resp_t", regs.response_window);
  preferences.putUInt("reg_frame_t", regs.frame_timeout);
  preferences.putUInt("reg_block_i", regs.block_interval);
  // Coefficients
  preferences.putFloat("coeff_grid", regs.coeff_grid_power);
  preferences.putFloat("coeff_load", regs.coeff_load_power);
  preferences.putFloat("coeff_ups", regs.coeff_ups_power);
  preferences.putFloat("coeff_smart", regs.coeff_smartload);
  
  preferences.end();
}

static CustomRegisters settings_load_registers() {
  CustomRegisters regs;
  preferences.begin("deye-ui", true);
  
  regs.pv1_power = preferences.getUShort("reg_pv1", 186);
  regs.pv2_power = preferences.getUShort("reg_pv2", 187);
  regs.pv3_power = preferences.getUShort("reg_pv3", 188);
  regs.pv_daily = preferences.getUShort("reg_pvd", 108);
  regs.battery_soc = preferences.getUShort("reg_bat_soc", 184);
  regs.battery_voltage = preferences.getUShort("reg_bat_v", 183);
  regs.battery_power = preferences.getUShort("reg_bat_p", 190);
  regs.battery_temp = preferences.getUShort("reg_bat_t", 182);
  regs.grid_power = preferences.getUShort("reg_grid_p", 169);
  regs.grid_status = preferences.getUShort("reg_grid_s", 194);
  regs.grid_buy_daily = preferences.getUShort("reg_grid_buy", 76);
  regs.grid_sell_daily = preferences.getUShort("reg_grid_sell", 77);
  regs.load_power = preferences.getUShort("reg_load_p", 178);
  regs.ups_power = preferences.getUShort("reg_ups_p", 172);
  regs.load_daily = preferences.getUShort("reg_load_d", 84);
  regs.dc_temp = preferences.getUShort("reg_dc_t", 90);
  regs.ac_temp = preferences.getUShort("reg_ac_t", 91);
  regs.smartload = preferences.getUShort("reg_smart", 195);
  regs.connect_timeout = preferences.getUInt("reg_conn_t", 10000);
  regs.response_window = preferences.getUInt("reg_resp_t", 10000);
  regs.frame_timeout = preferences.getUInt("reg_frame_t", 7000);
  regs.block_interval = preferences.getUInt("reg_block_i", 100);
  // Coefficients
  regs.coeff_grid_power = preferences.getFloat("coeff_grid", 1.0f);
  regs.coeff_load_power = preferences.getFloat("coeff_load", 1.0f);
  regs.coeff_ups_power = preferences.getFloat("coeff_ups", 1.0f);
  regs.coeff_smartload = preferences.getFloat("coeff_smart", 1.0f);
  
  preferences.end();
  return regs;
}

// ==================== FONCTIONS DE SAUVEGARDE DES PARAMÈTRES GÉNÉRAUX ====================

static void settings_save_wifi(const String &ssid, const String &password) {
  preferences.begin("deye-ui", false);
  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_pwd", password);
  preferences.end();
}

static void settings_save_ntp(
  const String &tz_rule,
  const String &server_primary,
  const String &server_secondary
) {
  preferences.begin("deye-ui", false);
  preferences.putString("tz_rule", tz_rule);
  preferences.putString("ntp_1", server_primary);
  preferences.putString("ntp_2", server_secondary);
  preferences.end();
}

static void settings_save_deye(const String &host, uint32_t logger_serial, uint16_t port) {
  preferences.begin("deye-ui", false);
  preferences.putString("deye_host", host);
  preferences.putUInt("logger", logger_serial);
  preferences.putUShort("deye_port", port);
  preferences.end();
}

// ==================== CHARGEMENT GLOBAL ====================

static void settings_load() {
  preferences.begin("deye-ui", true);

  cfg_wifi_ssid = preferences.getString("wifi_ssid", DEFAULT_WIFI_SSID);
  cfg_wifi_password = preferences.getString("wifi_pwd", DEFAULT_WIFI_PASSWORD);
  cfg_deye_host = preferences.getString("deye_host", DEFAULT_DEYE_HOST);
  cfg_deye_port = preferences.getUShort("deye_port", 8899);
  if (cfg_deye_port == 0) cfg_deye_port = 8899;
  cfg_logger_serial = preferences.getUInt("logger", DEFAULT_LOGGER_SERIAL);
  cfg_ntp_primary = preferences.getString("ntp_1", DEFAULT_NTP_PRIMARY);
  cfg_ntp_secondary = preferences.getString("ntp_2", DEFAULT_NTP_SECONDARY);
  cfg_tz_rule = preferences.getString("tz_rule", DEFAULT_TZ_RULE);

  preferences.end();
}

// ==================== ACCÈS AUX REGISTRES ====================

static CustomRegisters get_custom_registers() {
  return settings_load_registers();
}

// ==================== MODE LSE / LSW (simplifié) ====================

static bool settings_get_deye_mode_lse() {
  preferences.begin("deye-ui", true);
  bool mode = preferences.getBool("deye_mode_lse", false);
  preferences.end();
  return mode;
}

static void settings_set_deye_mode_lse(bool lse) {
  preferences.begin("deye-ui", false);
  preferences.putBool("deye_mode_lse", lse);
  preferences.end();
  DBG.printf("💾 settings_set_deye_mode_lse(%s) - sauvegardé\n", lse ? "LSE" : "LSW");
}

// ==================== MODE GEN (SmartLoad / GEN MO) ====================

static bool settings_get_gen_mode() {
  preferences.begin("deye-ui", true);
  bool mode = preferences.getBool("gen_smartload", true);
  preferences.end();
  return mode;
}

static void settings_set_gen_mode(bool smartload) {
  preferences.begin("deye-ui", false);
  preferences.putBool("gen_smartload", smartload);
  preferences.end();
}
