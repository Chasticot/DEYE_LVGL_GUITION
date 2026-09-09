// settings.h - Configuration Solarman V5 (LSW)

#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"
#include "ui_theme.h"
#include "network_config.h"

static NetworkConfig cfg_network;

static bool settings_save_network(const NetworkConfig &cfg) {
  if (!network_config_valid(cfg)) return false;
  Preferences storage;
  if (!storage.begin("deye-network", false)) return false;
  NetworkConfig check;
  bool ok = storage.putBytes("config", &cfg, sizeof(cfg)) == sizeof(cfg);
  ok = ok && storage.getBytes("config", &check, sizeof(check)) == sizeof(check) &&
    memcmp(&cfg, &check, sizeof(cfg)) == 0;
  storage.end();
  if (ok) cfg_network = cfg;
  return ok;
}

static void settings_load_network() {
  cfg_network = NetworkConfig{};
  Preferences storage;
  if (!storage.begin("deye-network", true)) return;
  NetworkConfig saved;
  if (storage.getBytesLength("config") == sizeof(saved) &&
      storage.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) &&
      network_config_valid(saved)) cfg_network = saved;
  storage.end();
}

static Preferences preferences;

static bool cfg_web_auth = false;
static String cfg_web_user;
static String cfg_web_password;

// One NVS record keeps enabled state and credentials together.
static bool settings_save_web_auth(bool enabled, const String &user, const String &password) {
  if (enabled && (user.isEmpty() || password.isEmpty())) return false;
  Preferences web;
  if (!web.begin("deye-web", false)) return false;
  String record = String(enabled ? "1" : "0") + "\n" + user + "\n" + password;
  bool ok = web.putString("auth", record) == record.length();
  ok = ok && web.getString("auth", "") == record;
  web.end();
  if (ok) { cfg_web_auth = enabled; cfg_web_user = user; cfg_web_password = password; }
  return ok;
}

static bool settings_reset_web_auth() {
  Preferences web;
  if (!web.begin("deye-web", false)) return false;
  bool ok = !web.isKey("auth") || web.remove("auth");
  ok = ok && !web.isKey("auth");
  web.end();
  if (ok) { cfg_web_auth = false; cfg_web_user = ""; cfg_web_password = ""; }
  return ok;
}

static void settings_load_web_auth() {
  Preferences web;
  if (!web.begin("deye-web", true)) return;
  String record = web.getString("auth", "");
  web.end();
  int separator = record.indexOf('\n', 2);
  if (separator < 0) return;
  cfg_web_user = record.substring(2, separator);
  cfg_web_password = record.substring(separator + 1);
  cfg_web_auth = record.startsWith("1\n");
}

static String cfg_wifi_ssid;
static String cfg_wifi_password;
static String cfg_deye_host;
static uint32_t cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
static String cfg_ntp_primary;
static String cfg_ntp_secondary;
static String cfg_tz_rule;
static bool cfg_tempo_enabled = true;
static bool cfg_tempo_colorblind_mode = false;
static bool cfg_ev_charger_enabled = false;
// NVS limite les cles a 15 caracteres. Les anciennes cles
// "tempo_colorblind" et "ev_charger_enabled" ne pouvaient pas etre ecrites.
static constexpr char SETTINGS_KEY_TEMPO[] = "tempo_enabled";
static constexpr char SETTINGS_KEY_TEMPO_COLORBLIND[] = "tempo_cb";
static constexpr char SETTINGS_KEY_EV_CHARGER[] = "ev_charger";
static_assert(sizeof(SETTINGS_KEY_TEMPO) <= 16, "Cle NVS trop longue");
static_assert(sizeof(SETTINGS_KEY_TEMPO_COLORBLIND) <= 16, "Cle NVS trop longue");
static_assert(sizeof(SETTINGS_KEY_EV_CHARGER) <= 16, "Cle NVS trop longue");
// Le thème historique est explicitement le thème par défaut.
static UiThemeId cfg_ui_theme = UI_THEME_DEFAULT;

struct DisplayConfig {
  uint32_t version = 2;
  uint8_t day_brightness = 220;
  uint8_t night_brightness = 35;
  uint8_t night_start_hour = 22;
  uint8_t night_end_hour = 7;
  bool night_enabled = true;
  bool sunset_mode = false;
  // Valeur de depart pour la France metropolitaine. A ajuster depuis le Web
  // pour que les horaires solaires correspondent exactement a l'installation.
  float latitude = 46.5f;
  float longitude = 2.5f;
};
static DisplayConfig cfg_display;

static bool settings_display_valid(const DisplayConfig &cfg) {
  return cfg.version == 2 && cfg.day_brightness >= 10 && cfg.night_brightness >= 1 &&
    cfg.night_start_hour < 24 && cfg.night_end_hour < 24 && isfinite(cfg.latitude) && isfinite(cfg.longitude) &&
    cfg.latitude >= -89.0f && cfg.latitude <= 89.0f && cfg.longitude >= -180.0f && cfg.longitude <= 180.0f;
}

static bool settings_save_display(const DisplayConfig &cfg) {
  if (!settings_display_valid(cfg)) return false;
  Preferences storage;
  if (!storage.begin("deye-display", false)) return false;
  DisplayConfig verify;
  bool ok = storage.putBytes("config", &cfg, sizeof(cfg)) == sizeof(cfg);
  ok = ok && storage.getBytes("config", &verify, sizeof(verify)) == sizeof(verify) &&
    memcmp(&cfg, &verify, sizeof(cfg)) == 0;
  storage.end();
  if (ok) cfg_display = cfg;
  return ok;
}

static void settings_load_display() {
  cfg_display = DisplayConfig{};
  Preferences storage;
  if (!storage.begin("deye-display", true)) return;
  DisplayConfig saved;
  if (storage.getBytesLength("config") == sizeof(saved) &&
      storage.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) && settings_display_valid(saved)) {
    cfg_display = saved;
  }
  storage.end();
}

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

static bool settings_registers_valid(const CustomRegisters &regs) {
  const uint16_t block1[] = { regs.grid_buy_daily, regs.grid_sell_daily, regs.load_daily,
    regs.dc_temp, regs.ac_temp, regs.pv_daily };
  const uint16_t block2[] = { regs.grid_power, regs.ups_power, regs.load_power, regs.battery_temp,
    regs.battery_voltage, regs.battery_soc, regs.pv1_power, regs.pv2_power, regs.pv3_power,
    regs.battery_power, regs.grid_status, regs.smartload };
  uint16_t min1 = block1[0], max1 = block1[0], min2 = block2[0], max2 = block2[0];
  for (uint8_t i = 1; i < sizeof(block1) / sizeof(block1[0]); ++i) { min1 = min(min1, block1[i]); max1 = max(max1, block1[i]); }
  for (uint8_t i = 1; i < sizeof(block2) / sizeof(block2[0]); ++i) { min2 = min(min2, block2[i]); max2 = max(max2, block2[i]); }
  const bool timeouts = regs.connect_timeout >= 50 && regs.connect_timeout <= 60000 &&
    regs.response_window >= 50 && regs.response_window <= 60000 && regs.frame_timeout >= 50 &&
    regs.frame_timeout <= 60000 && regs.block_interval >= 50 && regs.block_interval <= 60000;
  const bool coefficients = isfinite(regs.coeff_grid_power) && isfinite(regs.coeff_load_power) &&
    isfinite(regs.coeff_ups_power) && isfinite(regs.coeff_smartload) &&
    regs.coeff_grid_power >= -100.0f && regs.coeff_grid_power <= 100.0f &&
    regs.coeff_load_power >= -100.0f && regs.coeff_load_power <= 100.0f &&
    regs.coeff_ups_power >= -100.0f && regs.coeff_ups_power <= 100.0f &&
    regs.coeff_smartload >= -100.0f && regs.coeff_smartload <= 100.0f;
  return timeouts && coefficients && uint32_t(max1) - min1 + 1 <= 125 && uint32_t(max2) - min2 + 1 <= 125;
}

// ==================== FONCTIONS DE SAUVEGARDE DES REGISTRES ====================

static bool settings_save_registers(const CustomRegisters &regs) {
  if (!settings_registers_valid(regs) || !preferences.begin("deye-ui", false)) return false;
  CustomRegisters verify;
  
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
  const bool ok = preferences.putBytes("regs_v2", &regs, sizeof(regs)) == sizeof(regs) &&
    preferences.getBytes("regs_v2", &verify, sizeof(verify)) == sizeof(verify) &&
    memcmp(&regs, &verify, sizeof(regs)) == 0;
  preferences.end();
  return ok;
}

static CustomRegisters settings_load_registers() {
  CustomRegisters regs;
  preferences.begin("deye-ui", true);
  if (preferences.getBytesLength("regs_v2") == sizeof(regs) &&
      preferences.getBytes("regs_v2", &regs, sizeof(regs)) == sizeof(regs) && settings_registers_valid(regs)) {
    preferences.end();
    return regs;
  }
  
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
  regs.block_interval = preferences.getUInt("reg_block_i", 3000);
  // Coefficients
  regs.coeff_grid_power = preferences.getFloat("coeff_grid", 1.0f);
  regs.coeff_load_power = preferences.getFloat("coeff_load", 1.0f);
  regs.coeff_ups_power = preferences.getFloat("coeff_ups", 1.0f);
  regs.coeff_smartload = preferences.getFloat("coeff_smart", 1.0f);
  
  preferences.end();
  if (!settings_registers_valid(regs)) {
    return CustomRegisters{186,187,188,108,184,183,190,182,169,194,76,77,178,172,84,90,91,195,
      10000,10000,7000,3000,1.0f,1.0f,1.0f,1.0f};
  }
  return regs;
}

// ==================== FONCTIONS DE SAUVEGARDE DES PARAMÈTRES GÉNÉRAUX ====================

static bool settings_save_wifi(const String &ssid, const String &password) {
  if (ssid.length() == 0 || !preferences.begin("deye-ui", false)) {
    DBG.println("ERREUR: sauvegarde Wi-Fi impossible.");
    return false;
  }
  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_pwd", password);
  const String saved_ssid = preferences.getString("wifi_ssid", "");
  const String saved_password = preferences.getString("wifi_pwd", "");
  preferences.end();
  const bool saved = saved_ssid == ssid && saved_password == password;
  if (!saved) {
    DBG.println("ERREUR: verification de sauvegarde Wi-Fi echouee.");
    return false;
  }
  cfg_wifi_ssid = ssid;
  cfg_wifi_password = password;
  DBG.printf("Wi-Fi sauvegarde : SSID=%s, mot de passe=%u caracteres\n", ssid.c_str(), password.length());
  return true;
}

static bool settings_save_ntp(
  const String &tz_rule,
  const String &server_primary,
  const String &server_secondary
) {
  if (!preferences.begin("deye-ui", false)) return false;
  const bool ok = preferences.putString("tz_rule", tz_rule) == tz_rule.length() &&
    preferences.putString("ntp_1", server_primary) == server_primary.length() &&
    preferences.putString("ntp_2", server_secondary) == server_secondary.length() &&
    preferences.getString("tz_rule", "") == tz_rule && preferences.getString("ntp_1", "") == server_primary &&
    preferences.getString("ntp_2", "") == server_secondary;
  preferences.end();
  if (ok) { cfg_tz_rule = tz_rule; cfg_ntp_primary = server_primary; cfg_ntp_secondary = server_secondary; }
  return ok;
}

static bool settings_save_deye(const String &host, uint32_t logger_serial) {
  if (host.isEmpty() || !logger_serial || !preferences.begin("deye-ui", false)) return false;
  const bool ok = preferences.putString("deye_host", host) == host.length() &&
    preferences.putUInt("logger", logger_serial) == sizeof(logger_serial) &&
    preferences.getString("deye_host", "") == host && preferences.getUInt("logger", 0) == logger_serial;
  preferences.end();
  if (ok) { cfg_deye_host = host; cfg_logger_serial = logger_serial; }
  return ok;
}

static bool settings_save_tempo(bool enabled, bool colorblind, bool ev_charger) {
  if (!preferences.begin("deye-ui", false)) {
    DBG.println("ERREUR: ouverture sauvegarde Tempo / VE impossible.");
    return false;
  }
  const bool tempo_written = preferences.putBool(SETTINGS_KEY_TEMPO, enabled) == 1;
  const bool colorblind_written = preferences.putBool(SETTINGS_KEY_TEMPO_COLORBLIND, colorblind) == 1;
  const bool ev_written = preferences.putBool(SETTINGS_KEY_EV_CHARGER, ev_charger) == 1;
  const bool saved = tempo_written && colorblind_written && ev_written &&
    preferences.getBool(SETTINGS_KEY_TEMPO, !enabled) == enabled &&
    preferences.getBool(SETTINGS_KEY_TEMPO_COLORBLIND, !colorblind) == colorblind &&
    preferences.getBool(SETTINGS_KEY_EV_CHARGER, !ev_charger) == ev_charger;
  preferences.end();
  if (!saved) {
    DBG.println("ERREUR: verification de sauvegarde Tempo / VE echouee.");
    return false;
  }
  cfg_tempo_enabled = enabled;
  cfg_tempo_colorblind_mode = colorblind;
  cfg_ev_charger_enabled = ev_charger;
  return true;
}

static bool settings_set_ui_theme(UiThemeId theme) {
  theme = ui_theme_from_value(static_cast<uint8_t>(theme));
  if (!preferences.begin("deye-ui", false)) return false;
  const bool ok = preferences.putUChar("ui_theme", static_cast<uint8_t>(theme)) == 1 &&
    preferences.putBool("ui_theme_v2", true) == 1 &&
    preferences.getUChar("ui_theme", 255) == static_cast<uint8_t>(theme) && preferences.getBool("ui_theme_v2", false);
  preferences.end();
  if (ok) cfg_ui_theme = theme;
  return ok;
}

// ==================== CHARGEMENT GLOBAL ====================

static void settings_load() {
  settings_load_network();
  settings_load_display();
  settings_load_web_auth();
  preferences.begin("deye-ui", true);

  cfg_wifi_ssid = preferences.getString("wifi_ssid", DEFAULT_WIFI_SSID);
  cfg_wifi_password = preferences.getString("wifi_pwd", DEFAULT_WIFI_PASSWORD);
  cfg_deye_host = preferences.getString("deye_host", DEFAULT_DEYE_HOST);
  cfg_logger_serial = preferences.getUInt("logger", DEFAULT_LOGGER_SERIAL);
  // Migration des builds qui avaient sauvegardé le numéro factice utilisé
  // pendant les essais. Ce numéro est placé dans l'en-tête Solarman V5 ; avec
  // lui, le logger ignore les requêtes même si le réseau est correct.
  if (cfg_logger_serial == 123456789UL) {
    cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
    DBG.println("Serial logger factice remplace par le serial configure par defaut.");
  }
  cfg_ntp_primary = preferences.getString("ntp_1", DEFAULT_NTP_PRIMARY);
  cfg_ntp_secondary = preferences.getString("ntp_2", DEFAULT_NTP_SECONDARY);
  cfg_tz_rule = preferences.getString("tz_rule", DEFAULT_TZ_RULE);
  cfg_tempo_enabled = preferences.getBool(SETTINGS_KEY_TEMPO, true);
  cfg_tempo_colorblind_mode = preferences.getBool(SETTINGS_KEY_TEMPO_COLORBLIND, false);
  cfg_ev_charger_enabled = preferences.getBool(SETTINGS_KEY_EV_CHARGER, false);
  const uint8_t stored_theme = preferences.getUChar(
    "ui_theme", static_cast<uint8_t>(UI_THEME_DEFAULT)
  );
  // Migration : les anciennes versions avaient Actuel=0, Sombre=1 et Clair=2.
  // Toute préférence ancienne sauf Clair devient donc le nouveau Sombre.
  if (preferences.getBool("ui_theme_v2", false)) {
    cfg_ui_theme = ui_theme_from_value(stored_theme);
  } else {
    cfg_ui_theme = stored_theme == 2 ? UI_THEME_LIGHT : UI_THEME_DARK;
  }

  preferences.end();
}

// ==================== ACCÈS AUX REGISTRES ====================

static CustomRegisters get_custom_registers() {
  return settings_load_registers();
}

// ==================== MODE GEN (SmartLoad / GEN MO) ====================

static bool settings_get_gen_mode() {
  preferences.begin("deye-ui", true);
  bool mode = preferences.getBool("gen_smartload", true);
  preferences.end();
  return mode;
}

static bool settings_set_gen_mode(bool smartload) {
  if (!preferences.begin("deye-ui", false)) return false;
  const bool ok = preferences.putBool("gen_smartload", smartload) == 1 &&
    preferences.getBool("gen_smartload", !smartload) == smartload;
  preferences.end();
  return ok;
}
