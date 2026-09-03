// ui_settings.h - Version avec variable globale et logs pour déboguer

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "config.h"
#include "settings.h"
#include "ui_registres_perso.h"

// ==================== DÉCLARATIONS EXTERNES ====================
extern void deye_solarman_set_ui_active(bool active);

// ==================== VARIABLES STATIQUES ====================
static lv_obj_t *screen_settings = nullptr;
static lv_obj_t *screen_wifi = nullptr;
static lv_obj_t *screen_ntp = nullptr;
static lv_obj_t *screen_deye = nullptr;

static lv_obj_t *dropdown_wifi = nullptr;
static lv_obj_t *textarea_wifi_password = nullptr;
static lv_obj_t *label_wifi_scan = nullptr;

static lv_obj_t *dropdown_timezone = nullptr;
static lv_obj_t *textarea_ntp_primary = nullptr;
static lv_obj_t *textarea_ntp_secondary = nullptr;

static lv_obj_t *textarea_deye_host = nullptr;
static lv_obj_t *textarea_logger_serial = nullptr;
static lv_obj_t *textarea_deye_port = nullptr;
static lv_obj_t *mode_btnmatrix = nullptr;
static lv_timer_t *wifi_scan_timer = nullptr;
static lv_obj_t *keyboard_wifi = nullptr;
static lv_obj_t *keyboard_ntp = nullptr;
static lv_obj_t *keyboard_deye = nullptr;

// Variable globale pour stocker l'état du mode (true = LSE, false = LSW)
// Elle est initialisée au démarrage et mise à jour à chaque clic
static bool current_deye_mode_lse = false;

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
static lv_obj_t *ui_settings_make_title(lv_obj_t *parent, const char *text) {
  lv_obj_t *title = lv_label_create(parent);
  lv_label_set_text(title, text);
  lv_obj_set_pos(title, 0, 12);
  lv_obj_set_width(title, LCD_W);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(title, lv_color_hex(0x55D6FF), LV_PART_MAIN);
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
  lv_obj_set_style_bg_color(button, lv_color_hex(0x1D4ED8), LV_PART_MAIN);
  lv_obj_set_style_radius(button, 8, LV_PART_MAIN);
  if (callback != nullptr) {
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
  } else {
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  }

  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
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
    lv_obj_set_style_bg_color(*keyboard_out, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
    lv_obj_add_flag(*keyboard_out, LV_OBJ_FLAG_HIDDEN);
  }

  lv_obj_t *textarea = lv_textarea_create(parent);
  lv_textarea_set_one_line(textarea, true);
  lv_textarea_set_max_length(textarea, 63);
  lv_textarea_set_password_mode(textarea, password);
  lv_textarea_set_text(textarea, value);
  lv_obj_set_pos(textarea, x, y);
  lv_obj_set_size(textarea, 430, 42);
  lv_obj_set_style_bg_color(textarea, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(textarea, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(textarea, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
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
  lv_scr_load(screen_settings);
}

void ui_show_settings(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  lv_scr_load(screen_settings);
}

void ui_show_registers(lv_event_t *e) {
  (void)e;
  deye_solarman_set_ui_active(true);
  if (screen_registers != nullptr) {
    lv_scr_load(screen_registers);
  }
}

static void ui_show_wifi_screen(lv_event_t *e) {
  (void)e;
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
  lv_scr_load(screen_deye);
}

// ==================== FONCTIONS DE CONFIGURATION ====================

static void ui_finish_wifi_scan(int count) {
  if (count <= 0) {
    lv_label_set_text(label_wifi_scan, "Aucun reseau trouve");
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
  WiFi.mode(WIFI_STA);
  WiFi.scanDelete();
  const int result = WiFi.scanNetworks(true, true);
  if (result == WIFI_SCAN_FAILED) {
    lv_label_set_text(label_wifi_scan, "Echec du scan Wi-Fi");
    return;
  }
  lv_timer_reset(wifi_scan_timer);
  lv_timer_resume(wifi_scan_timer);
}

static void ui_save_wifi(lv_event_t *e) {
  (void)e;
  uint16_t selected = lv_dropdown_get_selected(dropdown_wifi);
  if (selected >= wifi_ssid_count) {
    DBG.println("Erreur: selection invalide");
    return;
  }
  String ssid = wifi_ssid_list[selected];
  settings_save_wifi(ssid, lv_textarea_get_text(textarea_wifi_password));
  ui_settings_restart_message();
}

static void ui_save_ntp(lv_event_t *e) {
  (void)e;
  uint16_t selected = lv_dropdown_get_selected(dropdown_timezone);
  const size_t count = sizeof(timezone_rules) / sizeof(timezone_rules[0]);
  if (selected >= count) selected = 0;

  settings_save_ntp(
    timezone_rules[selected],
    lv_textarea_get_text(textarea_ntp_primary),
    lv_textarea_get_text(textarea_ntp_secondary)
  );
  ui_settings_restart_message();
}

// ==================== ui_save_deye avec logs ====================
static void ui_save_deye(lv_event_t *e) {
  (void)e;

  const char *host = lv_textarea_get_text(textarea_deye_host);
  uint32_t serial = 0;
  uint32_t port_value = 0;
  if (host == nullptr || *host == '\0' || strlen(host) > 63 ||
      !ui_parse_u32(lv_textarea_get_text(textarea_logger_serial), 1, UINT32_MAX, &serial) ||
      !ui_parse_u32(lv_textarea_get_text(textarea_deye_port), 1, 65535, &port_value)) {
    ui_settings_show_error("Verifier l'hote, le serial et le port (1-65535).");
    return;
  }
  const uint16_t port = (uint16_t)port_value;
  settings_save_deye(host, serial, port);

  // 2. Sauvegarder le mode depuis la variable globale
  settings_set_deye_mode_lse(current_deye_mode_lse);

  // 3. Redémarrer
  ui_settings_restart_message();
}

// ==================== CRÉATION DES ÉCRANS ====================

static void ui_settings_create() {
  // ÉCRAN PRINCIPAL SETTINGS
  screen_settings = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_settings, lv_color_hex(0x0a0a1a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_settings, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_settings, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_settings, "CONFIGURATION");

  int btn_w = 280;
  int btn_h = 55;
  int spacing = 15;
  int col_x = (LCD_W - btn_w) / 2;
  int start_y = 70;
  int step_y = btn_h + spacing;

  ui_settings_make_button(screen_settings, "WIFI", col_x, start_y + 0 * step_y, btn_w, btn_h, ui_show_wifi_screen);
  ui_settings_make_button(screen_settings, "HEURE / NTP", col_x, start_y + 1 * step_y, btn_w, btn_h, ui_show_ntp_screen);
  ui_settings_make_button(screen_settings, "DEYE / SOLARMAN", col_x, start_y + 2 * step_y, btn_w, btn_h, ui_show_deye_screen);
  ui_settings_make_button(screen_settings, "REGISTRES PERSO", col_x, start_y + 3 * step_y, btn_w, btn_h, ui_show_registers);

  ui_settings_make_button(screen_settings, "RETOUR", col_x, 415, btn_w, 50, ui_show_dashboard);

  // ÉCRAN WIFI
  screen_wifi = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_wifi, lv_color_hex(0x0a0a1a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_wifi, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_wifi, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_wifi, "CONFIGURATION WIFI");

  lv_obj_t *label = lv_label_create(screen_wifi);
  lv_label_set_text(label, "Reseau Wi-Fi");
  lv_obj_set_pos(label, 20, 55);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  dropdown_wifi = lv_dropdown_create(screen_wifi);
  lv_dropdown_set_options(dropdown_wifi, cfg_wifi_ssid.c_str());
  lv_obj_set_pos(dropdown_wifi, 20, 82);
  lv_obj_set_size(dropdown_wifi, 300, 45);
  lv_obj_set_style_bg_color(dropdown_wifi, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(dropdown_wifi, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(dropdown_wifi, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
  lv_obj_set_style_border_width(dropdown_wifi, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(dropdown_wifi, 6, LV_PART_MAIN);
  lv_obj_set_style_max_height(dropdown_wifi, 200, LV_PART_MAIN);

  ui_settings_make_button(screen_wifi, "SCAN", 335, 82, 115, 45, ui_scan_wifi);

  label = lv_label_create(screen_wifi);
  lv_label_set_text(label, "Mot de passe");
  lv_obj_set_pos(label, 20, 145);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_wifi_password = ui_settings_make_textarea(
    screen_wifi,
    cfg_wifi_password.c_str(),
    20, 172,
    true,
    &keyboard_wifi
  );

  label_wifi_scan = lv_label_create(screen_wifi);
  lv_label_set_text(label_wifi_scan, "");
  lv_obj_set_pos(label_wifi_scan, 20, 235);
  lv_obj_set_style_text_color(label_wifi_scan, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label_wifi_scan, &lv_font_montserrat_14, LV_PART_MAIN);
  wifi_scan_timer = lv_timer_create(ui_wifi_scan_timer_cb, 200, nullptr);
  lv_timer_pause(wifi_scan_timer);

  ui_settings_make_button(screen_wifi, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_wifi, "SAUVEGARDER", 250, 370, 200, 55, ui_save_wifi);

  // ÉCRAN NTP
  screen_ntp = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_ntp, lv_color_hex(0x0a0a1a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_ntp, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_ntp, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_ntp, "CONFIGURATION HEURE / NTP");

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Fuseau horaire avec ete/hiver");
  lv_obj_set_pos(label, 20, 52);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  dropdown_timezone = lv_dropdown_create(screen_ntp);
  lv_dropdown_set_options(dropdown_timezone, timezone_names);
  lv_obj_set_pos(dropdown_timezone, 20, 80);
  lv_obj_set_size(dropdown_timezone, 430, 45);
  lv_obj_set_style_bg_color(dropdown_timezone, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(dropdown_timezone, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(dropdown_timezone, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
  lv_obj_set_style_border_width(dropdown_timezone, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(dropdown_timezone, 6, LV_PART_MAIN);

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Serveur NTP principal");
  lv_obj_set_pos(label, 20, 140);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
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
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_ntp_secondary = ui_settings_make_textarea(
    screen_ntp,
    cfg_ntp_secondary.c_str(),
    20, 252,
    false,
    &keyboard_ntp
  );

  ui_settings_make_button(screen_ntp, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_ntp, "SAUVEGARDER", 250, 370, 200, 55, ui_save_ntp);

  // ============================================================
  // ÉCRAN DEYE / SOLARMAN
  // ============================================================
  screen_deye = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_deye, lv_color_hex(0x0a0a1a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_deye, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_deye, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_deye, "CONFIGURATION DEYE");

  // Adresse IP
  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Adresse IP du logger");
  lv_obj_set_pos(label, 20, 70);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  textarea_deye_host = ui_settings_make_textarea(
    screen_deye,
    cfg_deye_host.c_str(),
    20, 100,
    false,
    &keyboard_deye
  );

  // Numéro de série
  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Numero de serie du logger");
  lv_obj_set_pos(label, 20, 170);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  char serial[16];
  snprintf(serial, sizeof(serial), "%lu", (unsigned long)cfg_logger_serial);

  textarea_logger_serial = ui_settings_make_textarea(
    screen_deye,
    serial,
    20, 200,
    false,
    &keyboard_deye
  );

  // Port
  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Port (default 8899)");
  lv_obj_set_pos(label, 20, 270);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  char port_str[8];
  snprintf(port_str, sizeof(port_str), "%d", cfg_deye_port);
  textarea_deye_port = ui_settings_make_textarea(
    screen_deye,
    port_str,
    20, 300,
    false,
    &keyboard_deye
  );

  // Mode LSE/LSW
  lv_obj_t *mode_label = lv_label_create(screen_deye);
  lv_label_set_text(mode_label, "Mode de communication");
  lv_obj_set_pos(mode_label, 20, 352);
  lv_obj_set_style_text_color(mode_label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, LV_PART_MAIN);

  static const char *mode_opts[] = {"LSE", "LSW", ""};
  mode_btnmatrix = lv_btnmatrix_create(screen_deye);
  lv_btnmatrix_set_map(mode_btnmatrix, mode_opts);
  lv_btnmatrix_set_btn_ctrl(mode_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(mode_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_obj_set_pos(mode_btnmatrix, 160, 350);
  lv_obj_set_size(mode_btnmatrix, 180, 34);
  lv_obj_set_style_bg_color(mode_btnmatrix, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(mode_btnmatrix, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(mode_btnmatrix, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
  lv_obj_set_style_border_width(mode_btnmatrix, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(mode_btnmatrix, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_all(mode_btnmatrix, 2, LV_PART_MAIN);

  // Initialiser la variable globale avec la valeur sauvegardée
  current_deye_mode_lse = settings_get_deye_mode_lse();
  DBG.printf("🔍 ui_settings_create: mode lu = %s\n", current_deye_mode_lse ? "LSE" : "LSW");

  // Cocher le bon bouton
  if (current_deye_mode_lse) {
    lv_btnmatrix_set_btn_ctrl(mode_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_clear_btn_ctrl(mode_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKED);
  } else {
    lv_btnmatrix_set_btn_ctrl(mode_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_clear_btn_ctrl(mode_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
  }

  // Callback qui met à jour la variable globale ET l'affichage
  lv_obj_add_event_cb(mode_btnmatrix, [](lv_event_t *e) {
    lv_obj_t *btnm = lv_event_get_target(e);
    uint32_t idx = lv_btnmatrix_get_selected_btn(btnm);
    if (idx == 0) {
      current_deye_mode_lse = true;
      lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
      lv_btnmatrix_clear_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
      DBG.println("🔄 Mode LSE sélectionné (variable = true)");
    } else if (idx == 1) {
      current_deye_mode_lse = false;
      lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
      lv_btnmatrix_clear_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
      DBG.println("🔄 Mode LSW sélectionné (variable = false)");
    }
  }, LV_EVENT_VALUE_CHANGED, NULL);

  // Boutons RETOUR et SAUVEGARDER
  ui_settings_make_button(screen_deye, "RETOUR", 20, 415, 190, 50, ui_show_settings_screen);
  ui_settings_make_button(screen_deye, "SAUVEGARDER", 250, 415, 200, 50, ui_save_deye);
}
