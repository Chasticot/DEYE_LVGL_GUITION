// ui_settings.h - Version avec variable globale et logs pour déboguer

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "config.h"
#include "settings.h"
#include "ui_registres_perso.h"
#include "ui_v2_navigation.h"
#include "github_ota.h"

// ==================== DÉCLARATIONS EXTERNES ====================
extern void deye_solarman_set_ui_active(bool active);
extern bool deye_copy_diagnostics(DeyeDiagnostics *out);

// ==================== VARIABLES STATIQUES ====================
static lv_obj_t *screen_settings = nullptr;
static lv_obj_t *screen_wifi = nullptr;
static lv_obj_t *screen_ntp = nullptr;
static lv_obj_t *screen_manual_clock = nullptr;
static lv_obj_t *textarea_manual_date = nullptr, *textarea_manual_time = nullptr;
static lv_obj_t *keyboard_manual_clock = nullptr;
static lv_obj_t *screen_deye = nullptr;
static lv_obj_t *screen_tempo = nullptr;
static lv_obj_t *screen_theme = nullptr;

static lv_obj_t *dropdown_wifi = nullptr;
static lv_obj_t *textarea_wifi_password = nullptr;
static lv_obj_t *checkbox_web_auth_reset = nullptr;
static lv_obj_t *label_wifi_scan = nullptr;
static lv_obj_t *label_wifi_password_length = nullptr;
static lv_obj_t *label_wifi_ip = nullptr;

static lv_obj_t *dropdown_timezone = nullptr;
static lv_obj_t *textarea_ntp_primary = nullptr;
static lv_obj_t *textarea_ntp_secondary = nullptr;

static lv_obj_t *textarea_deye_host = nullptr;
static lv_obj_t *textarea_logger_serial = nullptr;
static lv_obj_t *dropdown_deye_transport = nullptr, *textarea_deye_port = nullptr, *textarea_deye_unit = nullptr;
static lv_obj_t *button_deye_results=nullptr, *screen_logger_candidates=nullptr, *dropdown_logger_candidates=nullptr,
  *label_logger_candidates=nullptr;
static lv_obj_t *button_deye_discovery = nullptr;
static lv_obj_t *switch_tempo = nullptr;
static lv_obj_t *switch_tempo_colorblind = nullptr;
static lv_obj_t *label_ev_backend_status = nullptr;
static lv_obj_t *theme_btnmatrix = nullptr;
static lv_obj_t *slider_day_brightness = nullptr;
static lv_obj_t *slider_night_brightness = nullptr;
static lv_obj_t *switch_night_mode = nullptr;
static lv_obj_t *switch_sunset_mode = nullptr;
static lv_obj_t *dropdown_night_start = nullptr;
static lv_obj_t *dropdown_night_end = nullptr;
static lv_obj_t *label_deye_blocks = nullptr;
static lv_obj_t *label_deye_discovery = nullptr;
static uint32_t ui_deye_discovery_applied_generation = 0;
static lv_timer_t *wifi_scan_timer = nullptr;
static lv_obj_t *keyboard_wifi = nullptr;
static lv_obj_t *keyboard_ntp = nullptr;
static lv_obj_t *keyboard_deye = nullptr;

static void ui_settings_create();
static void ui_settings_update_deye_status();

static UiThemeId selected_ui_theme = UI_THEME_DEFAULT;

static String wifi_ssid_list[32];
static int wifi_ssid_count = 0;

static const char *timezone_names =
  "Europe/Paris\n"
  "Europe/London\n"
  "Europe/Athens\n"
  "America/New_York\n"
  "America/Chicago\n"
  "America/Denver\n"
  "America/Los_Angeles\n"
  "UTC";

static const char *timezone_rules[] = {
  "CET-1CEST,M3.5.0,M10.5.0/3",
  "GMT0BST,M3.5.0/1,M10.5.0/2",
  "EET-2EEST,M3.5.0/3,M10.5.0/4",
  "EST5EDT,M3.2.0,M11.1.0",
  "CST6CDT,M3.2.0,M11.1.0",
  "MST7MDT,M3.2.0,M11.1.0",
  "PST8PDT,M3.2.0,M11.1.0",
  "UTC0"
};

// ==================== FONCTIONS UI ====================
static const UiThemePalette &ui_settings_theme() {
  return ui_theme_palette(cfg_ui_theme);
}

static lv_color_t ui_settings_color(uint32_t color) {
  return lv_color_hex(color);
}

static lv_obj_t *ui_settings_make_title(lv_obj_t *parent, const char *text) {
  lv_obj_t *title = lv_label_create(parent);
  lv_label_set_text(title, text);
  lv_obj_set_pos(title, 0, 12);
  lv_obj_set_width(title, LCD_W);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(title, ui_settings_color(ui_settings_theme().accent), LV_PART_MAIN);
  return title;
}

static lv_obj_t *ui_settings_make_button(
  lv_obj_t *parent,
  const char *text,
  lv_coord_t x,
  lv_coord_t y,
  lv_coord_t width,
  lv_coord_t height,
  lv_event_cb_t callback
) {
  lv_obj_t *button = lv_btn_create(parent);
  lv_obj_set_size(button, width, height);
  lv_obj_set_pos(button, x, y);
  lv_obj_set_style_bg_color(button, ui_settings_color(ui_settings_theme().accent), LV_PART_MAIN);
  lv_obj_set_style_radius(button, 8, LV_PART_MAIN);
  if (callback != nullptr) {
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
  } else {
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  }

  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().accent_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_center(label);
  return button;
}

static void ui_settings_restart_message() {
  lv_obj_t *msg = lv_msgbox_create(nullptr, "Sauvegarde", "Parametres enregistres. Redemarrage...", nullptr, false);
  lv_obj_center(msg);
  lv_timer_create([](lv_timer_t *timer) {
    lv_timer_del(timer);
    ESP.restart();
  }, 700, nullptr);
}

static void ui_settings_show_error(const char *message) {
  lv_obj_t *msg = lv_msgbox_create(nullptr, "Valeur invalide", message, nullptr, true);
  lv_obj_center(msg);
}

static bool ui_parse_u32(const char *text, uint32_t minimum, uint32_t maximum, uint32_t *out) {
  if (text == nullptr || *text == '\0' || out == nullptr) return false;
  char *end = nullptr;
  const unsigned long value = strtoul(text, &end, 10);
  if (end == text || *end != '\0' || value < minimum || value > maximum) return false;
  *out = (uint32_t)value;
  return true;
}

static void ui_textarea_event(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *textarea = lv_event_get_target(e);
  lv_obj_t *keyboard = (lv_obj_t *)lv_event_get_user_data(e);

  if (code == LV_EVENT_FOCUSED) {
    lv_keyboard_set_textarea(keyboard, textarea);
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(keyboard);
  }

  if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
    lv_keyboard_set_textarea(keyboard, nullptr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

static lv_obj_t *ui_settings_make_textarea(
  lv_obj_t *parent,
  const char *value,
  lv_coord_t x,
  lv_coord_t y,
  bool password,
  lv_obj_t **keyboard_out
) {
  if (*keyboard_out == nullptr) {
    *keyboard_out = lv_keyboard_create(parent);
    lv_obj_set_size(*keyboard_out, LCD_W, 190);
    lv_obj_align(*keyboard_out, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(*keyboard_out, ui_settings_color(ui_settings_theme().control_bg), LV_PART_MAIN);
    lv_obj_add_flag(*keyboard_out, LV_OBJ_FLAG_HIDDEN);
  }

  lv_obj_t *textarea = lv_textarea_create(parent);
  lv_textarea_set_one_line(textarea, true);
  lv_textarea_set_max_length(textarea, 63);
  lv_textarea_set_password_mode(textarea, password);
  lv_textarea_set_text(textarea, value);
  lv_obj_set_pos(textarea, x, y);
  lv_obj_set_size(textarea, 430, 42);
  lv_obj_set_style_bg_color(textarea, ui_settings_color(ui_settings_theme().control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(textarea, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_border_color(textarea, ui_settings_color(ui_settings_theme().control_border), LV_PART_MAIN);
  lv_obj_set_style_border_width(textarea, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(textarea, 6, LV_PART_MAIN);
  lv_obj_add_event_cb(textarea, ui_textarea_event, LV_EVENT_ALL, *keyboard_out);
  return textarea;
}

// ==================== FONCTIONS DE NAVIGATION ====================
void ui_show_dashboard(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(false);
  lv_scr_load(screen_main);
}

void ui_show_settings_screen(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  if (screen_settings == nullptr) ui_settings_create();
  if (screen_settings == nullptr) return;
  lv_scr_load(screen_settings);
}

#include "ui_github_ota.h"

static void ui_wifi_password_length_changed(lv_event_t *e) {
  if (label_wifi_password_length == nullptr) return;
  const char *password = lv_textarea_get_text(lv_event_get_target(e));
  const size_t length = password == nullptr ? 0 : strlen(password);
  char text[48];
  snprintf(text, sizeof(text), "Mot de passe saisi : %u caracteres", (unsigned)length);
  lv_label_set_text(label_wifi_password_length, text);
}

void ui_show_settings(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  if (screen_settings == nullptr) ui_settings_create();
  if (screen_settings == nullptr) return;
  lv_scr_load(screen_settings);
}

void ui_show_registers(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  if (screen_registers == nullptr) ui_registers_create();
  if (screen_registers != nullptr) {
    lv_scr_load(screen_registers);
  }
}

static void ui_wifi_update_ip() {
  if (label_wifi_ip == nullptr) return;
  const String address = WiFi.status() == WL_CONNECTED
    ? WiFi.localIP().toString() : String("Non connecte");
  const String text = String("IP Serveur Web/Ecran : ") + address;
  if (strcmp(lv_label_get_text(label_wifi_ip), text.c_str()) != 0) {
    lv_label_set_text(label_wifi_ip, text.c_str());
  }
}

static void ui_show_wifi_screen(lv_event_t *e) {
  (void)e;
  ui_wifi_update_ip();
  if (checkbox_web_auth_reset) lv_obj_clear_state(checkbox_web_auth_reset, LV_STATE_CHECKED);
  deye_solarman_set_ui_active(true);
  lv_scr_load(screen_wifi);
}

static void ui_show_ntp_screen(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  lv_scr_load(screen_ntp);
}

static void ui_show_deye_screen(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  ui_settings_update_deye_status();
  lv_scr_load(screen_deye);
}

static void ui_show_tempo_screen(lv_event_t *e) {
  (void)e;
  lv_label_set_text(label_ev_backend_status, ev_backend_name(cfg_ev_backend));
  deye_solarman_set_ui_active(true);
  lv_scr_load(screen_tempo);
}

static void ui_show_theme_screen(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  selected_ui_theme = cfg_ui_theme;
  for (uint8_t i = 0; i <= UI_THEME_LIGHT; ++i) {
    if (i == selected_ui_theme) lv_btnmatrix_set_btn_ctrl(theme_btnmatrix, i, LV_BTNMATRIX_CTRL_CHECKED);
    else lv_btnmatrix_clear_btn_ctrl(theme_btnmatrix, i, LV_BTNMATRIX_CTRL_CHECKED);
  }
  lv_slider_set_value(slider_day_brightness,
    cfg_display.day_brightness < DISPLAY_BRIGHTNESS_MIN ? DISPLAY_BRIGHTNESS_MIN : cfg_display.day_brightness, LV_ANIM_OFF);
  lv_slider_set_value(slider_night_brightness,
    cfg_display.night_brightness < DISPLAY_BRIGHTNESS_MIN ? DISPLAY_BRIGHTNESS_MIN : cfg_display.night_brightness, LV_ANIM_OFF);
  if (cfg_display.night_enabled) lv_obj_add_state(switch_night_mode, LV_STATE_CHECKED);
  else lv_obj_clear_state(switch_night_mode, LV_STATE_CHECKED);
  lv_dropdown_set_selected(dropdown_night_start, cfg_display.night_start_hour);
  lv_dropdown_set_selected(dropdown_night_end, cfg_display.night_end_hour);
  if (cfg_display.sunset_mode) lv_obj_add_state(switch_sunset_mode, LV_STATE_CHECKED);
  else lv_obj_clear_state(switch_sunset_mode, LV_STATE_CHECKED);
  lv_scr_load(screen_theme);
}

// ==================== FONCTIONS DE CONFIGURATION ====================

static void ui_finish_wifi_scan(int count) {
  wifi_manager_resume_after_scan();
  if (count <= 0) {
    lv_label_set_text(
      label_wifi_scan,
      count == WIFI_SCAN_FAILED ? "Echec du scan Wi-Fi" : "Aucun reseau trouve"
    );
    WiFi.scanDelete();
    return;
  }

  wifi_ssid_count = 0;
  String display_list = "";

  for (int i = 0; i < count && i < 32; i++) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) continue;

    int rssi = WiFi.RSSI(i);
    int quality;
    if (rssi <= -100) quality = 0;
    else if (rssi >= -50) quality = 100;
    else quality = 2 * (rssi + 100);

    if (wifi_ssid_count > 0) display_list += "\n";
    display_list += ssid + " " + String(quality) + "%";
    wifi_ssid_list[wifi_ssid_count] = ssid;
    wifi_ssid_count++;
  }

  if (wifi_ssid_count == 0) {
    lv_label_set_text(label_wifi_scan, "Aucun reseau visible trouve");
    WiFi.scanDelete();
    return;
  }

  lv_dropdown_set_options(dropdown_wifi, display_list.c_str());
  lv_dropdown_set_selected(dropdown_wifi, 0);
  lv_obj_invalidate(dropdown_wifi);
  lv_dropdown_open(dropdown_wifi);

  char msg[64];
  snprintf(msg, sizeof(msg), "%d reseaux trouves", wifi_ssid_count);
  lv_label_set_text(label_wifi_scan, msg);

  WiFi.scanDelete();
}

static void ui_wifi_scan_timer_cb(lv_timer_t *timer) {
  const int count = WiFi.scanComplete();
  if (count == WIFI_SCAN_RUNNING) return;
  lv_timer_pause(timer);
  ui_finish_wifi_scan(count);
}

static void ui_scan_wifi(lv_event_t *e) {
  (void)e;
  if (dropdown_wifi == nullptr || wifi_scan_timer == nullptr) {
    DBG.println("ERROR: WiFi scan UI not initialized");
    return;
  }

  lv_label_set_text(label_wifi_scan, "Scan en cours...");
  wifi_manager_set_scan_active(true);

  // ESP32 Arduino core 2.0.x returns WIFI_SCAN_FAILED (-2) when a scan is
  // requested while a previous Wi-Fi connection attempt is still active.
  // End that attempt first, scan, then restore the saved connection.
  WiFi.disconnect(false, false);
  WiFi.mode(WIFI_STA);
  WiFi.scanDelete();
  delay(150);
  lv_refr_now(nullptr);
  const int result = WiFi.scanNetworks(false, true);
  DBG.printf("Resultat Wi-Fi scan : %d\n", result);
  ui_finish_wifi_scan(result);
}

static void ui_save_wifi(lv_event_t *e) {
  (void)e;
  if (checkbox_web_auth_reset && lv_obj_has_state(checkbox_web_auth_reset, LV_STATE_CHECKED)) {
    if (!settings_reset_web_auth()) {
      ui_settings_show_error("Reset authentification Web echoue.");
      return;
    }
    ui_settings_restart_message();
    return;
  }
  uint16_t selected = lv_dropdown_get_selected(dropdown_wifi);
  if (selected >= wifi_ssid_count) {
    DBG.println("Erreur: selection invalide");
    return;
  }
  String ssid = wifi_ssid_list[selected];
  const char *entered_password = lv_textarea_get_text(textarea_wifi_password);
  // The password field starts blank so a new value can never be appended to
  // an invisible old value.  Leaving it blank preserves the stored password.
  const String password = (entered_password != nullptr && *entered_password != '\0')
    ? String(entered_password)
    : cfg_wifi_password;
  if (!settings_save_wifi(ssid, password)) {
    ui_settings_show_error("Sauvegarde Wi-Fi echouee.");
    return;
  }
  ui_settings_restart_message();
}

static void ui_save_ntp(lv_event_t *e) {
  (void)e;
  uint16_t selected = lv_dropdown_get_selected(dropdown_timezone);
  const size_t count = sizeof(timezone_rules) / sizeof(timezone_rules[0]);
  if (selected >= count) selected = 0;

  if (!settings_save_ntp(
    timezone_rules[selected],
    lv_textarea_get_text(textarea_ntp_primary),
    lv_textarea_get_text(textarea_ntp_secondary)
  )) {
    ui_settings_show_error("Sauvegarde NTP impossible.");
    return;
  }
  ntp_manager_begin();
  lv_obj_t *message = lv_msgbox_create(nullptr, "Heure / NTP", "Parametres appliques. Horloge conservee sans redemarrage.", nullptr, true);
  lv_obj_center(message);
}

// ==================== SAUVEGARDE DEYE / SOLARMAN V5 ====================
static void ui_show_manual_clock(lv_event_t *) {
  struct tm now = {};
  char date[16] = {}, clock[16] = {};
  if (clock_get_local_time(&now)) {
    strftime(date, sizeof(date), "%Y-%m-%d", &now);
    strftime(clock, sizeof(clock), "%H:%M", &now);
  }
  lv_textarea_set_text(textarea_manual_date, date);
  lv_textarea_set_text(textarea_manual_time, clock);
  lv_scr_load(screen_manual_clock);
}
static void ui_apply_manual_clock(lv_event_t *) {
  if (!ntp_manager_set_manual(lv_textarea_get_text(textarea_manual_date), lv_textarea_get_text(textarea_manual_time))) {
    ui_settings_show_error("Date ou heure invalide. Utiliser AAAA-MM-JJ et HH:MM."); return;
  }
  lv_obj_add_flag(keyboard_manual_clock, LV_OBJ_FLAG_HIDDEN);
  lv_scr_load(screen_ntp);
  lv_obj_t *message = lv_msgbox_create(nullptr, "Heure", "Horloge reglee. NTP prendra le relais au retour du reseau.", nullptr, true);
  lv_obj_center(message);
}
static void ui_deye_transport_changed(lv_event_t *) {
  const LoggerModel model=LoggerModel(lv_dropdown_get_selected(dropdown_deye_transport));
  lv_textarea_set_text(textarea_deye_port,String(logger_default_port(model)).c_str());
  lv_obj_clear_state(button_deye_discovery,LV_STATE_DISABLED);
}
static void ui_discover_deye_logger(lv_event_t *e) {
  (void)e;
  uint32_t port=0,unit=0,numeric=0;char identity[33];
  const LoggerModel model=LoggerModel(lv_dropdown_get_selected(dropdown_deye_transport));
  if (!logger_identity_parse(lv_textarea_get_text(textarea_logger_serial),model,identity,sizeof(identity),numeric) ||
      (model!=LoggerModel::Dl1000B && numeric==123456789UL) ||
      !ui_parse_u32(lv_textarea_get_text(textarea_deye_port),1,65535,&port) ||
      !ui_parse_u32(lv_textarea_get_text(textarea_deye_unit),1,247,&unit)) {
    ui_settings_show_error("Choisir le type et renseigner son vrai numero de serie, le port et l'unite.");return;
  }
  if (deye_logger_discovery_start(identity,model,uint16_t(port),uint8_t(unit))) {
    lv_label_set_text(label_deye_discovery, "Recherche du logger en attente...");
  } else {
    lv_label_set_text(label_deye_discovery, WiFi.status() == WL_CONNECTED
      ? "Recherche deja en cours ou lecteur indisponible."
      : "Connectez d'abord l'ecran au Wi-Fi.");
  }
}
static void ui_save_deye(lv_event_t *e) {
  (void)e;

  const char *host = lv_textarea_get_text(textarea_deye_host);
  uint32_t serial = 0, port = 0, unit = 0; char identity[33];
  const auto model = LoggerModel(lv_dropdown_get_selected(dropdown_deye_transport));
  if (host == nullptr || *host == '\0' || strlen(host) > 63 ||
      !ui_parse_u32(lv_textarea_get_text(textarea_deye_port), 1, 65535, &port) ||
      !ui_parse_u32(lv_textarea_get_text(textarea_deye_unit), 1, 247, &unit) ||
      !logger_identity_parse(lv_textarea_get_text(textarea_logger_serial),model,identity,sizeof(identity),serial)) {
    ui_settings_show_error("Verifier IP, port, unite et numero V5 si utilise.");
    return;
  }
  if (!settings_save_logger(host, identity, model, uint16_t(port), uint8_t(unit))) {
    ui_settings_show_error("Sauvegarde Deye impossible.");
    return;
  }
  ui_settings_restart_message();
}

static void ui_save_tempo(lv_event_t *e) {
  (void)e;
  if (!settings_save_tempo(
    lv_obj_has_state(switch_tempo, LV_STATE_CHECKED),
    lv_obj_has_state(switch_tempo_colorblind, LV_STATE_CHECKED),
    cfg_ev_charger_enabled
  )) {
    ui_settings_show_error("Echec de sauvegarde Tempo / VE. Veuillez reessayer.");
    return;
  }
  ui_settings_restart_message();
}

static void ui_save_theme(lv_event_t *e) {
  (void)e;
  DisplayConfig display = cfg_display;
  display.day_brightness = lv_slider_get_value(slider_day_brightness);
  display.night_brightness = lv_slider_get_value(slider_night_brightness);
  display.night_enabled = lv_obj_has_state(switch_night_mode, LV_STATE_CHECKED);
  display.sunset_mode = lv_obj_has_state(switch_sunset_mode, LV_STATE_CHECKED);
  display.night_start_hour = lv_dropdown_get_selected(dropdown_night_start);
  display.night_end_hour = lv_dropdown_get_selected(dropdown_night_end);
  if (!settings_save_display(display) || !settings_set_ui_theme(selected_ui_theme)) {
    ui_settings_show_error("Sauvegarde affichage impossible.");
    return;
  }
  ui_settings_restart_message();
}

static bool ui_logger_result_current(const DeyeLoggerDiscoveryStatus &status) {
  char identity[33];uint32_t numeric;
  return status.model==LoggerModel(lv_dropdown_get_selected(dropdown_deye_transport)) &&
    logger_identity_parse(lv_textarea_get_text(textarea_logger_serial),status.model,identity,sizeof(identity),numeric) &&
    !strcmp(identity,status.identity) &&
    !strcmp(lv_textarea_get_text(textarea_deye_port),String(status.port).c_str()) &&
    !strcmp(lv_textarea_get_text(textarea_deye_unit),String(status.unit).c_str());
}
static void ui_show_logger_candidates(lv_event_t *) {
  DeyeLoggerDiscoveryStatus status;deye_logger_discovery_copy_status(&status);
  if (!ui_logger_result_current(status) || status.state!=DEYE_LOGGER_DISCOVERY_CANDIDATES || !status.candidate_count) return;
  char options[160]={},description[128];
  for (uint8_t i=0;i<status.candidate_count;++i) {
    if (i) strcat(options,"\n");strcat(options,status.candidates[i]);
  }
  lv_dropdown_set_options(dropdown_logger_candidates,options);
  snprintf(description,sizeof(description),"Numero demande : %s\nPort %u ; unite %u",status.identity,status.port,status.unit);
  lv_label_set_text(label_logger_candidates,description);
  lv_scr_load(screen_logger_candidates);
}
static void ui_use_logger_candidate(lv_event_t *) {
  DeyeLoggerDiscoveryStatus status;deye_logger_discovery_copy_status(&status);
  const uint16_t index=lv_dropdown_get_selected(dropdown_logger_candidates);
  if (!ui_logger_result_current(status) || status.state!=DEYE_LOGGER_DISCOVERY_CANDIDATES || index>=status.candidate_count) return;
  lv_textarea_set_text(textarea_deye_host,status.candidates[index]);
  lv_scr_load(screen_deye);
}
static void ui_settings_update_deye_status() {
  if (label_deye_discovery) {
    DeyeLoggerDiscoveryStatus discovery;
    deye_logger_discovery_copy_status(&discovery);
    char message[180];
    switch (discovery.state) {
      case DEYE_LOGGER_DISCOVERY_QUEUED:
        snprintf(message, sizeof(message), "Recherche en attente de la lecture en cours..."); break;
      case DEYE_LOGGER_DISCOVERY_RUNNING:
        snprintf(message, sizeof(message), "Recherche %s%s : %u / %u. Patientez...",
          discovery.automatic ? "auto " : "", logger_discovery_phase_name(discovery.phase),
          discovery.scanned_hosts, discovery.total_hosts); break;
      case DEYE_LOGGER_DISCOVERY_FOUND:
        if (discovery.automatic) {
          snprintf(message, sizeof(message), discovery.restart_pending
            ? "Nouvelle IP : %s\nSauvegardee. Redemarrage..." : discovery.save_failed
            ? "IP trouvee : %s\nSauvegarde impossible. Pas de redemarrage."
            : "Logger retrouve : %s\nNumero de serie enregistre conserve.", discovery.host);
          break;
        }
        snprintf(message, sizeof(message), "Trouve : %s ; numero verifie.\nVerifier puis SAUVEGARDER.",discovery.host);
        if (discovery.generation != ui_deye_discovery_applied_generation && ui_logger_result_current(discovery)) {
          lv_textarea_set_text(textarea_deye_host, discovery.host);
          ui_deye_discovery_applied_generation = discovery.generation;
        }
        break;
      case DEYE_LOGGER_DISCOVERY_CANDIDATES:
        snprintf(message,sizeof(message),"%u serveur(s) Modbus ; numero non verifie.\nOuvrir RESULTATS pour choisir l'adresse IP.",discovery.candidate_count);break;
      case DEYE_LOGGER_DISCOVERY_NOT_FOUND:
        snprintf(message, sizeof(message), "Aucun logger trouve. Verifier Wi-Fi et numero de serie."); break;
      case DEYE_LOGGER_DISCOVERY_ERROR:
        snprintf(message, sizeof(message), "Recherche interrompue : verifier la connexion Wi-Fi."); break;
      default:
        snprintf(message, sizeof(message), "Renseigner le numero de serie avant de lancer\nla recherche de l'adresse IP du logger."); break;
    }
    if (!discovery.automatic && (discovery.state==DEYE_LOGGER_DISCOVERY_FOUND || discovery.state==DEYE_LOGGER_DISCOVERY_CANDIDATES) &&
        !ui_logger_result_current(discovery))
      snprintf(message,sizeof(message),"Resultat pour un autre type, numero ou port.\nRelancer la recherche avec ces parametres.");
    lv_label_set_text(label_deye_discovery, message);
    if (button_deye_results) {
      if (discovery.state==DEYE_LOGGER_DISCOVERY_CANDIDATES && ui_logger_result_current(discovery))
        lv_obj_clear_state(button_deye_results,LV_STATE_DISABLED);
      else lv_obj_add_state(button_deye_results,LV_STATE_DISABLED);
    }
  }
  if (label_deye_blocks == nullptr) return;
  DeyeDiagnostics diagnostic = {};
  if (!deye_copy_diagnostics(&diagnostic)) return;
  char text[180], age1_text[20], age2_text[20], age3_text[20];
  const uint32_t now = millis();
  const uint32_t age1 = diagnostic.block_last_success_ms[0] ? (now - diagnostic.block_last_success_ms[0]) / 1000 : UINT32_MAX;
  const uint32_t age2 = diagnostic.block_last_success_ms[1] ? (now - diagnostic.block_last_success_ms[1]) / 1000 : UINT32_MAX;
  const uint32_t age3 = diagnostic.block_last_success_ms[2] ? (now - diagnostic.block_last_success_ms[2]) / 1000 : UINT32_MAX;
  if (age1 == UINT32_MAX) strcpy(age1_text, "jamais"); else snprintf(age1_text, sizeof(age1_text), "%lu s", (unsigned long)age1);
  if (age2 == UINT32_MAX) strcpy(age2_text, "jamais"); else snprintf(age2_text, sizeof(age2_text), "%lu s", (unsigned long)age2);
  if (age3 == UINT32_MAX) strcpy(age3_text, "jamais"); else snprintf(age3_text, sizeof(age3_text), "%lu s", (unsigned long)age3);
  snprintf(text, sizeof(text), "Dernieres lectures : B1 %s | B2 %s | B3 %s\nOK : %lu / %lu / %lu   Echecs : %lu / %lu / %lu",
    age1_text, age2_text, age3_text,
    (unsigned long)diagnostic.block_success[0], (unsigned long)diagnostic.block_success[1], (unsigned long)diagnostic.block_success[2],
    (unsigned long)diagnostic.block_failure[0], (unsigned long)diagnostic.block_failure[1], (unsigned long)diagnostic.block_failure[2]);
  lv_label_set_text(label_deye_blocks, text);
}

// ==================== CRÉATION DES ÉCRANS ====================

static void ui_settings_create() {
  // ÉCRAN PRINCIPAL SETTINGS
  screen_settings = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_settings, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_settings, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_settings, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_settings, "CONFIGURATION");

  int btn_w = 280;
  int btn_h = 42;
  int spacing = 7;
  int col_x = (LCD_W - btn_w) / 2;
  int start_y = 55;
  int step_y = btn_h + spacing;

  ui_settings_make_button(screen_settings, "DEYE / SOLARMAN", col_x, start_y + 0 * step_y, btn_w, btn_h, ui_show_v2_deye);
  ui_settings_make_button(screen_settings, "TARIFS / HEURES CREUSES", col_x, start_y + 1 * step_y, btn_w, btn_h, ui_show_v2_tariffs);
  ui_settings_make_button(screen_settings, "VEHICULE ELECTRIQUE", col_x, start_y + 2 * step_y, btn_w, btn_h, ui_show_v2_vehicle);
  ui_settings_make_button(screen_settings, "RELAIS", col_x, start_y + 3 * step_y, btn_w, btn_h, ui_show_v2_relay);
  ui_settings_make_button(screen_settings, "RESEAU / HEURE", col_x, start_y + 4 * step_y, btn_w, btn_h, ui_show_v2_network);
  ui_settings_make_button(screen_settings, "AFFICHAGE", col_x, start_y + 5 * step_y, btn_w, btn_h, ui_show_v2_display);

  ui_settings_make_button(screen_settings, "MISE A JOUR", col_x, start_y + 6 * step_y, btn_w, btn_h, ui_show_ota);

  ui_settings_make_button(screen_settings, "RETOUR", col_x, 415, btn_w, 50, ui_show_dashboard);

  // ÉCRAN TEMPO / VE
  screen_tempo = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_tempo, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_tempo, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_tempo, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_tempo, "TEMPO / RECHARGE");

  lv_obj_t *tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Afficher la couleur et le tarif en cours");
  lv_obj_set_pos(tempo_label, 35, 70);
  lv_obj_set_width(tempo_label, 300);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  switch_tempo = lv_switch_create(screen_tempo);
  lv_obj_set_size(switch_tempo, 64, 34);
  lv_obj_set_pos(switch_tempo, 370, 66);
  lv_obj_set_style_bg_color(switch_tempo, ui_settings_color(ui_settings_theme().switch_off), LV_PART_MAIN);
  lv_obj_set_style_bg_color(switch_tempo, ui_settings_color(ui_settings_theme().switch_on), LV_PART_INDICATOR | LV_STATE_CHECKED);
  if (cfg_tempo_enabled) lv_obj_add_state(switch_tempo, LV_STATE_CHECKED);

  tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Desactive : aucune requete et aucune tuile Tempo.");
  lv_obj_set_pos(tempo_label, 35, 125);
  lv_obj_set_width(tempo_label, 410);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);

  tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Mode daltonien");
  lv_obj_set_pos(tempo_label, 35, 165);
  lv_obj_set_width(tempo_label, 300);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  switch_tempo_colorblind = lv_switch_create(screen_tempo);
  lv_obj_set_size(switch_tempo_colorblind, 64, 34);
  lv_obj_set_pos(switch_tempo_colorblind, 370, 161);
  lv_obj_set_style_bg_color(switch_tempo_colorblind, ui_settings_color(ui_settings_theme().switch_off), LV_PART_MAIN);
  lv_obj_set_style_bg_color(
    switch_tempo_colorblind, ui_settings_color(ui_settings_theme().switch_on), LV_PART_INDICATOR | LV_STATE_CHECKED
  );
  if (cfg_tempo_colorblind_mode) {
    lv_obj_add_state(switch_tempo_colorblind, LV_STATE_CHECKED);
  }

  tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Ajoute le nom de la couleur aux libelles du jour et de demain.");
  lv_obj_set_pos(tempo_label, 35, 220);
  lv_obj_set_width(tempo_label, 410);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);

  tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Borne de recharge");
  lv_obj_set_pos(tempo_label, 35, 270);
  lv_obj_set_width(tempo_label, 190);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  label_ev_backend_status = lv_label_create(screen_tempo);
  lv_label_set_text(label_ev_backend_status, ev_backend_name(cfg_ev_backend));
  lv_obj_set_pos(label_ev_backend_status, 235, 270);
  lv_obj_set_width(label_ev_backend_status, 210);
  lv_obj_set_style_text_color(label_ev_backend_status, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label_ev_backend_status, &lv_font_montserrat_16, LV_PART_MAIN);
  ui_settings_make_button(screen_tempo, "CHOISIR LA BORNE / TARIFS", 35, 310, 410, 44, ui_show_v2_ev_options);

  ui_settings_make_button(screen_tempo, "RETOUR", 20, 390, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_tempo, "SAUVEGARDER", 250, 390, 200, 55, ui_save_tempo);

  // ÉCRAN THÈME — la palette est appliquée après redémarrage pour éviter une
  // reconstruction coûteuse de tous les objets LVGL pendant l'utilisation.
  screen_theme = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_theme, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_theme, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_theme, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_theme, "THEME / ECRAN");

  lv_obj_t *theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Choisissez l'apparence de l'interface");
  lv_obj_set_pos(theme_label, 0, 48);
  lv_obj_set_width(theme_label, LCD_W);
  lv_obj_set_style_text_align(theme_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  // Deux choix larges sur une seule ligne : plus faciles a viser au doigt.
  static const char *theme_options[] = {"SOMBRE", "CLAIR", ""};
  theme_btnmatrix = lv_btnmatrix_create(screen_theme);
  lv_btnmatrix_set_map(theme_btnmatrix, theme_options);
  lv_obj_set_pos(theme_btnmatrix, 50, 76);
  lv_obj_set_size(theme_btnmatrix, 380, 70);
  lv_obj_set_style_bg_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_border_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().control_border), LV_PART_MAIN);
  lv_obj_set_style_border_width(theme_btnmatrix, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(theme_btnmatrix, 8, LV_PART_MAIN);
  lv_obj_set_style_bg_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().control_bg), LV_PART_ITEMS);
  lv_obj_set_style_bg_opa(theme_btnmatrix, LV_OPA_COVER, LV_PART_ITEMS);
  lv_obj_set_style_text_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().text), LV_PART_ITEMS);
  lv_obj_set_style_border_color(theme_btnmatrix, ui_settings_color(ui_settings_theme().control_border), LV_PART_ITEMS);
  lv_obj_set_style_border_width(theme_btnmatrix, 1, LV_PART_ITEMS);
  lv_obj_set_style_radius(theme_btnmatrix, 6, LV_PART_ITEMS);
  lv_obj_set_style_bg_color(
    theme_btnmatrix, ui_settings_color(ui_settings_theme().accent), LV_PART_ITEMS | LV_STATE_CHECKED
  );
  lv_obj_set_style_bg_opa(theme_btnmatrix, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_text_color(
    theme_btnmatrix, ui_settings_color(ui_settings_theme().accent_text), LV_PART_ITEMS | LV_STATE_CHECKED
  );
  lv_obj_set_style_border_color(
    theme_btnmatrix, ui_settings_color(ui_settings_theme().accent), LV_PART_ITEMS | LV_STATE_CHECKED
  );
  lv_obj_set_style_border_width(theme_btnmatrix, 2, LV_PART_ITEMS | LV_STATE_CHECKED);
  for (uint8_t i = 0; i <= UI_THEME_LIGHT; ++i) {
    lv_btnmatrix_set_btn_ctrl(theme_btnmatrix, i, LV_BTNMATRIX_CTRL_CHECKABLE);
  }
  // Le mode exclusif empêche LVGL de décocher le bouton au relâchement.
  // Sans lui, l'état visuel ne dure que pendant l'appui tactile.
  lv_btnmatrix_set_one_checked(theme_btnmatrix, true);
  selected_ui_theme = cfg_ui_theme;
  lv_btnmatrix_set_btn_ctrl(theme_btnmatrix, selected_ui_theme, LV_BTNMATRIX_CTRL_CHECKED);
  lv_obj_add_event_cb(theme_btnmatrix, [](lv_event_t *e) {
    lv_obj_t *buttons = lv_event_get_target(e);
    const uint32_t selected = lv_btnmatrix_get_selected_btn(buttons);
    if (selected > UI_THEME_LIGHT) return;
    selected_ui_theme = static_cast<UiThemeId>(selected);
    for (uint8_t i = 0; i <= UI_THEME_LIGHT; ++i) {
      if (i == selected) lv_btnmatrix_set_btn_ctrl(buttons, i, LV_BTNMATRIX_CTRL_CHECKED);
      else lv_btnmatrix_clear_btn_ctrl(buttons, i, LV_BTNMATRIX_CTRL_CHECKED);
    }
  }, LV_EVENT_VALUE_CHANGED, nullptr);

  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Luminosite jour");
  lv_obj_set_pos(theme_label, 25, 162);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  slider_day_brightness = lv_slider_create(screen_theme);
  lv_slider_set_range(slider_day_brightness, DISPLAY_BRIGHTNESS_MIN, 255);
  lv_slider_set_value(slider_day_brightness,
    cfg_display.day_brightness < DISPLAY_BRIGHTNESS_MIN ? DISPLAY_BRIGHTNESS_MIN : cfg_display.day_brightness, LV_ANIM_OFF);
  lv_obj_set_pos(slider_day_brightness, 190, 164);
  lv_obj_set_size(slider_day_brightness, 250, 18);
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Luminosite nuit");
  lv_obj_set_pos(theme_label, 25, 205);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  slider_night_brightness = lv_slider_create(screen_theme);
  lv_slider_set_range(slider_night_brightness, DISPLAY_BRIGHTNESS_MIN, 255);
  lv_slider_set_value(slider_night_brightness,
    cfg_display.night_brightness < DISPLAY_BRIGHTNESS_MIN ? DISPLAY_BRIGHTNESS_MIN : cfg_display.night_brightness, LV_ANIM_OFF);
  lv_obj_set_pos(slider_night_brightness, 190, 207);
  lv_obj_set_size(slider_night_brightness, 250, 18);
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Mode nuit programme");
  lv_obj_set_pos(theme_label, 25, 248);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  switch_night_mode = lv_switch_create(screen_theme);
  lv_obj_set_pos(switch_night_mode, 370, 240);
  lv_obj_set_size(switch_night_mode, 64, 34);
  if (cfg_display.night_enabled) lv_obj_add_state(switch_night_mode, LV_STATE_CHECKED);
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Suivre coucher / lever du soleil");
  lv_obj_set_pos(theme_label, 25, 285);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  switch_sunset_mode = lv_switch_create(screen_theme);
  lv_obj_set_pos(switch_sunset_mode, 370, 277);
  lv_obj_set_size(switch_sunset_mode, 64, 34);
  if (cfg_display.sunset_mode) lv_obj_add_state(switch_sunset_mode, LV_STATE_CHECKED);
  static const char *hours = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "De");
  lv_obj_set_pos(theme_label, 25, 338);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  dropdown_night_start = lv_dropdown_create(screen_theme);
  lv_dropdown_set_options(dropdown_night_start, hours);
  lv_dropdown_set_selected(dropdown_night_start, cfg_display.night_start_hour);
  lv_obj_set_pos(dropdown_night_start, 70, 325);
  lv_obj_set_size(dropdown_night_start, 100, 40);
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "a");
  lv_obj_set_pos(theme_label, 200, 338);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  dropdown_night_end = lv_dropdown_create(screen_theme);
  lv_dropdown_set_options(dropdown_night_end, hours);
  lv_dropdown_set_selected(dropdown_night_end, cfg_display.night_end_hour);
  lv_obj_set_pos(dropdown_night_end, 230, 325);
  lv_obj_set_size(dropdown_night_end, 100, 40);
  theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Coordonnees solaires reglables sur la page Web.");
  lv_obj_set_pos(theme_label, 0, 365);
  lv_obj_set_width(theme_label, LCD_W);
  lv_obj_set_style_text_align(theme_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_12, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);

  ui_settings_make_button(screen_theme, "RETOUR", 20, 390, 190, 55, ui_show_v2_display);
  ui_settings_make_button(screen_theme, "APPLIQUER", 250, 390, 200, 55, ui_save_theme);

  // ÉCRAN WIFI
  screen_wifi = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_wifi, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_wifi, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_wifi, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_wifi, "WIFI / RESEAU");

  lv_obj_t *label = lv_label_create(screen_wifi);
  lv_label_set_text(label, "Reseau Wi-Fi");
  lv_obj_set_pos(label, 20, 55);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  dropdown_wifi = lv_dropdown_create(screen_wifi);
  lv_dropdown_set_options(dropdown_wifi, cfg_wifi_ssid.c_str());
  lv_obj_set_pos(dropdown_wifi, 20, 82);
  lv_obj_set_size(dropdown_wifi, 300, 45);
  lv_obj_set_style_bg_color(dropdown_wifi, ui_settings_color(ui_settings_theme().control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(dropdown_wifi, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_border_color(dropdown_wifi, ui_settings_color(ui_settings_theme().control_border), LV_PART_MAIN);
  lv_obj_set_style_border_width(dropdown_wifi, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(dropdown_wifi, 6, LV_PART_MAIN);
  lv_obj_set_style_max_height(dropdown_wifi, 200, LV_PART_MAIN);

  ui_settings_make_button(screen_wifi, "SCAN", 335, 82, 115, 45, ui_scan_wifi);

  textarea_wifi_password = ui_settings_make_textarea(
    screen_wifi,
    "",
    20, 172,
    true,
    &keyboard_wifi
  );
  label_wifi_password_length = lv_label_create(screen_wifi);
  lv_label_set_text(label_wifi_password_length, "Mot de passe : vide (conserver l'actuel)");
  lv_obj_set_pos(label_wifi_password_length, 20, 220);
  lv_obj_set_style_text_color(label_wifi_password_length, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label_wifi_password_length, &lv_font_montserrat_12, LV_PART_MAIN);
  lv_obj_add_event_cb(textarea_wifi_password, ui_wifi_password_length_changed, LV_EVENT_VALUE_CHANGED, nullptr);

  label_wifi_scan = lv_label_create(screen_wifi);
  lv_label_set_text(label_wifi_scan, "");
  lv_obj_set_pos(label_wifi_scan, 20, 245);
  lv_obj_set_style_text_color(label_wifi_scan, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label_wifi_scan, &lv_font_montserrat_14, LV_PART_MAIN);
  wifi_scan_timer = lv_timer_create(ui_wifi_scan_timer_cb, 200, nullptr);
  lv_timer_pause(wifi_scan_timer);

  label_wifi_ip = lv_label_create(screen_wifi);
  lv_obj_set_pos(label_wifi_ip, 20, 270);
  lv_obj_set_width(label_wifi_ip, 430);
  lv_obj_set_style_text_color(label_wifi_ip, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label_wifi_ip, &lv_font_montserrat_14, LV_PART_MAIN);
  ui_wifi_update_ip();

  ui_settings_make_button(screen_wifi, "RETOUR", 20, 370, 190, 55, ui_show_v2_network);
  checkbox_web_auth_reset = lv_checkbox_create(screen_wifi);
  lv_checkbox_set_text(checkbox_web_auth_reset, "Reset authentification serveur Web");
  lv_obj_set_pos(checkbox_web_auth_reset, 20, 300);
  lv_obj_set_style_text_font(checkbox_web_auth_reset, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(checkbox_web_auth_reset, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  ui_settings_make_button(screen_wifi, "SAUVEGARDER", 250, 370, 200, 55, ui_save_wifi);

  // ÉCRAN NTP
  screen_ntp = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_ntp, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_ntp, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_ntp, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_ntp, "CONFIGURATION HEURE / NTP");

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Fuseau horaire avec ete/hiver");
  lv_obj_set_pos(label, 20, 52);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  dropdown_timezone = lv_dropdown_create(screen_ntp);
  lv_dropdown_set_options(dropdown_timezone, timezone_names);
  lv_obj_set_pos(dropdown_timezone, 20, 80);
  lv_obj_set_size(dropdown_timezone, 430, 45);
  lv_obj_set_style_bg_color(dropdown_timezone, ui_settings_color(ui_settings_theme().control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(dropdown_timezone, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);
  lv_obj_set_style_border_color(dropdown_timezone, ui_settings_color(ui_settings_theme().control_border), LV_PART_MAIN);
  lv_obj_set_style_border_width(dropdown_timezone, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(dropdown_timezone, 6, LV_PART_MAIN);

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Serveur NTP principal");
  lv_obj_set_pos(label, 20, 140);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_ntp_primary = ui_settings_make_textarea(
    screen_ntp,
    cfg_ntp_primary.c_str(),
    20, 167,
    false,
    &keyboard_ntp
  );

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Serveur NTP secondaire");
  lv_obj_set_pos(label, 20, 225);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_ntp_secondary = ui_settings_make_textarea(
    screen_ntp,
    cfg_ntp_secondary.c_str(),
    20, 252,
    false,
    &keyboard_ntp
  );

  ui_settings_make_button(screen_ntp, "RETOUR", 20, 370, 190, 55, ui_show_v2_network);
  ui_settings_make_button(screen_ntp, "SAUVEGARDER", 250, 370, 200, 55, ui_save_ntp);
  ui_settings_make_button(screen_ntp, "REGLER L'HEURE MANUELLEMENT", 20, 312, 430, 42, ui_show_manual_clock);

  screen_manual_clock = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_manual_clock, ui_settings_color(ui_settings_theme().screen_bg), 0);
  lv_obj_clear_flag(screen_manual_clock, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_manual_clock, "HEURE SANS INTERNET");
  label = lv_label_create(screen_manual_clock);
  lv_label_set_text(label, "Date locale : AAAA-MM-JJ"); lv_obj_set_pos(label, 20, 65);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().text), 0);
  textarea_manual_date = ui_settings_make_textarea(screen_manual_clock, "", 20, 92, false, &keyboard_manual_clock);
  label = lv_label_create(screen_manual_clock);
  lv_label_set_text(label, "Heure locale : HH:MM (24 h)"); lv_obj_set_pos(label, 20, 155);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().text), 0);
  textarea_manual_time = ui_settings_make_textarea(screen_manual_clock, "", 20, 182, false, &keyboard_manual_clock);
  label = lv_label_create(screen_manual_clock);
  lv_label_set_text(label, "Utilise le fuseau horaire enregistre.\nApplique sans redemarrer.\nApres coupure de courant, regler a nouveau\nsi NTP reste indisponible.");
  lv_obj_set_pos(label, 20, 245); lv_obj_set_width(label, 440);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), 0);
  ui_settings_make_button(screen_manual_clock, "RETOUR", 20, 370, 190, 55, ui_show_ntp_screen);
  ui_settings_make_button(screen_manual_clock, "APPLIQUER", 250, 370, 200, 55, ui_apply_manual_clock);

  // ============================================================
  // ÉCRAN DEYE / SOLARMAN
  // ============================================================
  screen_deye = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_deye, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_deye, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_deye, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_deye, "CONFIGURATION DEYE");

  // Adresse IP
  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Adresse IP du logger");
  lv_obj_set_pos(label, 20, 110);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_deye_host = ui_settings_make_textarea(
    screen_deye,
    cfg_deye_host.c_str(),
    20, 130,
    false,
    &keyboard_deye
  );

  // Numéro de série
  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Numero de serie du logger");
  lv_obj_set_pos(label, 20, 242);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  char serial[33];
  snprintf(serial, sizeof(serial), "%s", cfg_logger_identity.c_str());

  textarea_logger_serial = ui_settings_make_textarea(
    screen_deye,
    serial,
    20, 262,
    false,
    &keyboard_deye
  );

  label_deye_blocks = lv_label_create(screen_deye);
  lv_obj_set_pos(label_deye_blocks, 20, 308);
  lv_obj_set_width(label_deye_blocks, 440);
  lv_obj_set_style_text_font(label_deye_blocks, &lv_font_montserrat_12, LV_PART_MAIN);
  lv_obj_set_style_text_color(label_deye_blocks, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  label_deye_discovery = lv_label_create(screen_deye);
  lv_obj_set_pos(label_deye_discovery, 20, 342);
  lv_obj_set_width(label_deye_discovery, 440);
  lv_obj_set_style_text_font(label_deye_discovery, &lv_font_montserrat_12, LV_PART_MAIN);
  lv_obj_set_style_text_color(label_deye_discovery, ui_settings_color(ui_settings_theme().accent), LV_PART_MAIN);
  button_deye_discovery = ui_settings_make_button(screen_deye, "RECHERCHER", 20, 378, 205, 32, ui_discover_deye_logger);
  button_deye_results = ui_settings_make_button(screen_deye,"RESULTATS",245,378,205,32,ui_show_logger_candidates);
  lv_obj_set_style_bg_opa(button_deye_discovery, LV_OPA_40, LV_STATE_DISABLED);
  dropdown_deye_transport = lv_dropdown_create(screen_deye);
  lv_dropdown_set_options(dropdown_deye_transport, "LSW - Solarman V5\nLSE - Ethernet Modbus TCP\nDL1000B - Modbus TCP");
  lv_obj_set_pos(dropdown_deye_transport, 20, 60); lv_obj_set_size(dropdown_deye_transport, 430, 40);
  lv_dropdown_set_selected(dropdown_deye_transport, uint8_t(cfg_logger_model));
  lv_obj_add_event_cb(dropdown_deye_transport, ui_deye_transport_changed, LV_EVENT_VALUE_CHANGED, nullptr);
  label = lv_label_create(screen_deye); lv_label_set_text(label, "Port TCP"); lv_obj_set_pos(label, 20, 176);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().text), 0);
  label = lv_label_create(screen_deye); lv_label_set_text(label, "Unite Modbus (1-247)"); lv_obj_set_pos(label, 245, 176);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().text), 0);
  textarea_deye_port = ui_settings_make_textarea(screen_deye, String(cfg_deye_port).c_str(), 20, 196, false, &keyboard_deye);
  textarea_deye_unit = ui_settings_make_textarea(screen_deye, String(cfg_deye_unit).c_str(), 245, 196, false, &keyboard_deye);
  lv_obj_set_width(textarea_deye_port, 205); lv_obj_set_width(textarea_deye_unit, 205);
  ui_settings_update_deye_status();

  ui_settings_make_button(screen_deye, "RETOUR", 20, 415, 190, 45, ui_show_v2_deye);
  ui_settings_make_button(screen_deye, "SAUVEGARDER", 250, 415, 200, 45, ui_save_deye);

  screen_logger_candidates=lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_logger_candidates,ui_settings_color(ui_settings_theme().screen_bg),0);
  lv_obj_clear_flag(screen_logger_candidates,LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_logger_candidates,"RESULTATS RECHERCHE");
  label_logger_candidates=lv_label_create(screen_logger_candidates);
  lv_obj_set_pos(label_logger_candidates,20,85);lv_obj_set_width(label_logger_candidates,430);
  lv_obj_set_style_text_color(label_logger_candidates,ui_settings_color(ui_settings_theme().text),0);
  lv_label_set_text(label_logger_candidates,"");
  dropdown_logger_candidates=lv_dropdown_create(screen_logger_candidates);
  lv_obj_set_pos(dropdown_logger_candidates,20,150);lv_obj_set_size(dropdown_logger_candidates,430,48);
  lv_obj_set_style_max_height(dropdown_logger_candidates,160,0);
  label=lv_label_create(screen_logger_candidates);
  lv_label_set_text(label,"Ces adresses repondent en Modbus TCP.\nLe numero du logger n'est pas verifie.\nChoisir seulement l'appareil attendu,\npuis sauvegarder sur la page precedente.");
  lv_obj_set_pos(label,20,235);lv_obj_set_width(label,430);
  lv_obj_set_style_text_font(label,&lv_font_montserrat_16,0);
  lv_obj_set_style_text_color(label,ui_settings_color(ui_settings_theme().text),0);
  ui_settings_make_button(screen_logger_candidates,"RETOUR",20,370,190,55,ui_show_deye_screen);
  ui_settings_make_button(screen_logger_candidates,"CHOISIR CETTE IP",230,370,220,55,ui_use_logger_candidate);
}
