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
static lv_obj_t *screen_tempo = nullptr;
static lv_obj_t *screen_theme = nullptr;

static lv_obj_t *dropdown_wifi = nullptr;
static lv_obj_t *textarea_wifi_password = nullptr;
static lv_obj_t *label_wifi_scan = nullptr;
static lv_obj_t *label_wifi_password_length = nullptr;

static lv_obj_t *dropdown_timezone = nullptr;
static lv_obj_t *textarea_ntp_primary = nullptr;
static lv_obj_t *textarea_ntp_secondary = nullptr;

static lv_obj_t *textarea_deye_host = nullptr;
static lv_obj_t *textarea_logger_serial = nullptr;
static lv_obj_t *switch_tempo = nullptr;
static lv_obj_t *switch_tempo_colorblind = nullptr;
static lv_obj_t *switch_ev_charger = nullptr;
static lv_obj_t *theme_btnmatrix = nullptr;
static lv_timer_t *wifi_scan_timer = nullptr;
static lv_obj_t *keyboard_wifi = nullptr;
static lv_obj_t *keyboard_ntp = nullptr;
static lv_obj_t *keyboard_deye = nullptr;

static void ui_settings_create();

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

static void ui_show_tempo_screen(lv_event_t *e) {
  (void)e;
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

  settings_save_ntp(
    timezone_rules[selected],
    lv_textarea_get_text(textarea_ntp_primary),
    lv_textarea_get_text(textarea_ntp_secondary)
  );
  ui_settings_restart_message();
}

// ==================== SAUVEGARDE DEYE / SOLARMAN V5 ====================
static void ui_save_deye(lv_event_t *e) {
  (void)e;

  const char *host = lv_textarea_get_text(textarea_deye_host);
  uint32_t serial = 0;
  if (host == nullptr || *host == '\0' || strlen(host) > 63 ||
      !ui_parse_u32(lv_textarea_get_text(textarea_logger_serial), 1, UINT32_MAX, &serial)) {
    ui_settings_show_error("Verifier l'hote et le serial du logger.");
    return;
  }
  settings_save_deye(host, serial);
  ui_settings_restart_message();
}

static void ui_save_tempo(lv_event_t *e) {
  (void)e;
  if (!settings_save_tempo(
    lv_obj_has_state(switch_tempo, LV_STATE_CHECKED),
    lv_obj_has_state(switch_tempo_colorblind, LV_STATE_CHECKED),
    lv_obj_has_state(switch_ev_charger, LV_STATE_CHECKED)
  )) {
    ui_settings_show_error("Echec de sauvegarde Tempo / VE. Veuillez reessayer.");
    return;
  }
  ui_show_dashboard(nullptr);
}

static void ui_save_theme(lv_event_t *e) {
  (void)e;
  settings_set_ui_theme(selected_ui_theme);
  ui_settings_restart_message();
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
  int btn_h = 48;
  int spacing = 10;
  int col_x = (LCD_W - btn_w) / 2;
  int start_y = 55;
  int step_y = btn_h + spacing;

  ui_settings_make_button(screen_settings, "WIFI", col_x, start_y + 0 * step_y, btn_w, btn_h, ui_show_wifi_screen);
  ui_settings_make_button(screen_settings, "HEURE / NTP", col_x, start_y + 1 * step_y, btn_w, btn_h, ui_show_ntp_screen);
  ui_settings_make_button(screen_settings, "DEYE / SOLARMAN", col_x, start_y + 2 * step_y, btn_w, btn_h, ui_show_deye_screen);
  ui_settings_make_button(screen_settings, "REGISTRES PERSO", col_x, start_y + 3 * step_y, btn_w, btn_h, ui_show_registers);
  ui_settings_make_button(screen_settings, "TEMPO / VE", col_x, start_y + 4 * step_y, btn_w, btn_h, ui_show_tempo_screen);
  ui_settings_make_button(screen_settings, "THEME", col_x, start_y + 5 * step_y, btn_w, btn_h, ui_show_theme_screen);

  ui_settings_make_button(screen_settings, "RETOUR", col_x, 415, btn_w, 50, ui_show_dashboard);

  // ÉCRAN TEMPO / VE
  screen_tempo = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_tempo, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_tempo, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_tempo, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_tempo, "TEMPO / VE");

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
  lv_label_set_text(tempo_label, "Activer la page VE");
  lv_obj_set_pos(tempo_label, 35, 270);
  lv_obj_set_width(tempo_label, 300);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  switch_ev_charger = lv_switch_create(screen_tempo);
  lv_obj_set_size(switch_ev_charger, 64, 34);
  lv_obj_set_pos(switch_ev_charger, 370, 266);
  lv_obj_set_style_bg_color(switch_ev_charger, ui_settings_color(ui_settings_theme().switch_off), LV_PART_MAIN);
  lv_obj_set_style_bg_color(
    switch_ev_charger, ui_settings_color(ui_settings_theme().switch_on), LV_PART_INDICATOR | LV_STATE_CHECKED
  );
  if (cfg_ev_charger_enabled) lv_obj_add_state(switch_ev_charger, LV_STATE_CHECKED);

  tempo_label = lv_label_create(screen_tempo);
  lv_label_set_text(tempo_label, "Lit les reglages VE de l'onduleur, meme sans borne.");
  lv_obj_set_pos(tempo_label, 35, 325);
  lv_obj_set_width(tempo_label, 410);
  lv_obj_set_style_text_font(tempo_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(tempo_label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);

  ui_settings_make_button(screen_tempo, "RETOUR", 20, 390, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_tempo, "SAUVEGARDER", 250, 390, 200, 55, ui_save_tempo);

  // ÉCRAN THÈME — la palette est appliquée après redémarrage pour éviter une
  // reconstruction coûteuse de tous les objets LVGL pendant l'utilisation.
  screen_theme = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_theme, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_theme, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_theme, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_theme, "THEME");

  lv_obj_t *theme_label = lv_label_create(screen_theme);
  lv_label_set_text(theme_label, "Choisissez l'apparence de l'interface");
  lv_obj_set_pos(theme_label, 0, 62);
  lv_obj_set_width(theme_label, LCD_W);
  lv_obj_set_style_text_align(theme_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().text), LV_PART_MAIN);

  static const char *theme_options[] = {"SOMBRE", "\n", "CLAIR", ""};
  theme_btnmatrix = lv_btnmatrix_create(screen_theme);
  lv_btnmatrix_set_map(theme_btnmatrix, theme_options);
  lv_obj_set_pos(theme_btnmatrix, 90, 100);
  lv_obj_set_size(theme_btnmatrix, 300, 130);
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
  lv_label_set_text(theme_label, "Le changement est applique au prochain redemarrage.");
  lv_obj_set_pos(theme_label, 0, 315);
  lv_obj_set_width(theme_label, LCD_W);
  lv_obj_set_style_text_align(theme_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(theme_label, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(theme_label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);

  ui_settings_make_button(screen_theme, "RETOUR", 20, 390, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_theme, "APPLIQUER", 250, 390, 200, 55, ui_save_theme);

  // ÉCRAN WIFI
  screen_wifi = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_wifi, ui_settings_color(ui_settings_theme().screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_wifi, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_wifi, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_wifi, "CONFIGURATION WIFI");

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

  ui_settings_make_button(screen_wifi, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
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

  ui_settings_make_button(screen_ntp, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_ntp, "SAUVEGARDER", 250, 370, 200, 55, ui_save_ntp);

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
  lv_obj_set_pos(label, 20, 70);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
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
  lv_obj_set_pos(label, 20, 180);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().muted_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  char serial[16];
  snprintf(serial, sizeof(serial), "%lu", (unsigned long)cfg_logger_serial);

  textarea_logger_serial = ui_settings_make_textarea(
    screen_deye,
    serial,
    20, 210,
    false,
    &keyboard_deye
  );

  ui_settings_make_button(screen_deye, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_deye, "SAUVEGARDER", 250, 370, 200, 55, ui_save_deye);
}
