// settings.h - Configuration Solarman V5 (LSW)

#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <atomic>
#include "config.h"
#include "ui_theme.h"
#include "network_config.h"
#include "inverter_profiles.h"
#include "v2_settings.h"

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
static String cfg_vetronic_host;
static uint32_t cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
static String cfg_ntp_primary;
static String cfg_ntp_secondary;
static String cfg_tz_rule;
// Les fonctions optionnelles restent inactives tant que l'utilisateur ne les
// active pas explicitement dans le menu. Cela evite les requetes Tempo et les
// lectures VE inutiles lors de la premiere mise en service.
static bool cfg_tempo_enabled = false;
static bool cfg_tempo_colorblind_mode = false;
static bool cfg_ev_charger_enabled = false;
static std::atomic<bool> cfg_gen_smartload{true};
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
static constexpr uint8_t DISPLAY_BRIGHTNESS_MIN = 220;

struct DisplayConfig {
  uint32_t version = 2;
  uint8_t day_brightness = 255;
  // Sur ce panneau, DISPLAY_BRIGHTNESS_MIN est le seuil visuel utile minimum.
  uint8_t night_brightness = 255;
  uint8_t night_start_hour = 22;
  uint8_t night_end_hour = 7;
  // Les deux modes nuit sont des choix explicites de l'utilisateur.
  bool night_enabled = false;
  bool sunset_mode = false;
  // Valeur de depart pour la France metropolitaine. A ajuster depuis le Web
  // pour que les horaires solaires correspondent exactement a l'installation.
  float latitude = 46.5f;
  float longitude = 2.5f;
};
static DisplayConfig cfg_display;

static bool settings_display_valid(const DisplayConfig &cfg) {
  return cfg.version == 2 && cfg.day_brightness >= DISPLAY_BRIGHTNESS_MIN &&
    cfg.night_brightness >= DISPLAY_BRIGHTNESS_MIN &&
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

// ==================== CONFIGURATION DES REGISTRES VE ====================
// Cette configuration est separee des registres de mesure : ses deux adresses
// peuvent etre la cible d'ecritures Modbus lorsque l'utilisateur les debloque
// explicitement depuis la page Web.
struct EvRegisterConfig {
  uint32_t version = 1;
  uint16_t mode_register = 489;
  uint16_t max_power_register = 490;
  bool write_enabled = false;
};
static EvRegisterConfig cfg_ev_registers;

static bool settings_ev_registers_valid(const EvRegisterConfig &cfg) {
  // Les deux adresses peuvent etre separees, mais doivent rester lisibles
  // dans un meme bloc Modbus FC03 (maximum 125 registres).
  const uint16_t first = cfg.mode_register < cfg.max_power_register ? cfg.mode_register : cfg.max_power_register;
  const uint16_t last = cfg.mode_register < cfg.max_power_register ? cfg.max_power_register : cfg.mode_register;
  return cfg.version == 1 && cfg.mode_register != cfg.max_power_register &&
    uint32_t(last) - first + 1 <= 125;
}

static bool settings_save_ev_registers(const EvRegisterConfig &cfg) {
  if (!settings_ev_registers_valid(cfg) || cfg.write_enabled) return false;
  Preferences storage;
  if (!storage.begin("deye-ev", false)) return false;
  EvRegisterConfig verify;
  bool ok = storage.putBytes("config", &cfg, sizeof(cfg)) == sizeof(cfg);
  ok = ok && storage.getBytes("config", &verify, sizeof(verify)) == sizeof(verify) &&
    memcmp(&cfg, &verify, sizeof(cfg)) == 0;
  storage.end();
  if (ok) cfg_ev_registers = cfg;
  return ok;
}

static void settings_load_ev_registers() {
  cfg_ev_registers = EvRegisterConfig{};
  if (!inverter_profile().ev_supported) return;
  Preferences storage;
  if (!storage.begin("deye-ev", true)) return;
  EvRegisterConfig saved;
  if (storage.getBytesLength("config") == sizeof(saved) &&
      storage.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) &&
      settings_ev_registers_valid(saved)) cfg_ev_registers = saved;
  cfg_ev_registers.write_enabled = false; // All charging commands go to the WB01 gateway.
  storage.end();
}

// ==================== STRUCTURE POUR LES REGISTRES PERSONNALISÉS ====================
#include "inverter_settings.h"

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

static bool settings_vetronic_host_valid(const String &host) {
  uint32_t address = 0;
  return host.length() >= 7 && host.length() <= 15 &&
    network_parse_ipv4(host.c_str(), address) && network_unicast(address);
}

static bool settings_save_vetronic_host(const String &host) {
  if (!settings_vetronic_host_valid(host) || !preferences.begin("deye-ui", false)) return false;
  const bool ok = preferences.putString("vetronic_host", host) == host.length() &&
    preferences.getString("vetronic_host", "") == host;
  preferences.end();
  if (ok) cfg_vetronic_host = host;
  return ok;
}

static bool settings_save_tempo(bool enabled, bool colorblind, bool ev_charger) {
  if (ev_charger && !inverter_profile().ev_supported) return false;
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
  cfg_inverter_model = 0; // Dedicated SG02LP1 edition; do not change the shared model preference.
  settings_load_network();
  settings_load_display();
  settings_load_ev_registers();
  v2_load();
  settings_load_web_auth();
  preferences.begin("deye-ui", true);

  cfg_wifi_ssid = preferences.getString("wifi_ssid", DEFAULT_WIFI_SSID);
  cfg_wifi_password = preferences.getString("wifi_pwd", DEFAULT_WIFI_PASSWORD);
  cfg_deye_host = preferences.getString("deye_host", DEFAULT_DEYE_HOST);
  cfg_vetronic_host = preferences.getString("vetronic_host", VETRONIC_HOST);
  if (!settings_vetronic_host_valid(cfg_vetronic_host)) cfg_vetronic_host = VETRONIC_HOST;
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
  cfg_tempo_enabled = preferences.getBool(SETTINGS_KEY_TEMPO, false);
  cfg_tempo_colorblind_mode = preferences.getBool(SETTINGS_KEY_TEMPO_COLORBLIND, false);
  cfg_ev_charger_enabled = inverter_profile().ev_supported && preferences.getBool(SETTINGS_KEY_EV_CHARGER, false);
  cfg_gen_smartload = cfg_inverter_model == 0 ? preferences.getBool("gen_smartload", true) : true;
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
  Preferences model;
  if (model.begin(inverter_profile().storage, true)) {
    cfg_gen_smartload = model.getBool("gen_mode", cfg_gen_smartload.load());
    model.end();
  }
}

// ==================== ACCÈS AUX REGISTRES ====================

static CustomRegisters get_custom_registers() {
  return settings_load_registers();
}

// ==================== MODE GEN (SmartLoad / GEN MO) ====================

static bool settings_get_gen_mode() {
  return cfg_gen_smartload;
}

static bool settings_set_gen_mode(bool smartload) {
  if (!preferences.begin(inverter_profile().storage, false)) return false;
  const bool ok = preferences.putBool("gen_mode", smartload) == 1 &&
    preferences.getBool("gen_mode", !smartload) == smartload;
  preferences.end();
  if (ok) cfg_gen_smartload = smartload;
  return ok;
}
