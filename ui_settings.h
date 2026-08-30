#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "config.h"
#include "settings.h"


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

extern void ui_show_dashboard(lv_event_t *e);

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
  lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, text);
  lv_obj_center(label);
  return button;
}

static void ui_settings_restart_message() {
  lv_obj_t *msg = lv_msgbox_create(nullptr, "Sauvegarde", "Parametres enregistres. Redemarrage...", nullptr, false);
  lv_obj_center(msg);
  lv_timer_handler();
  delay(700);
  ESP.restart();
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
  lv_obj_t *keyboard,
  const char *value,
  lv_coord_t x,
  lv_coord_t y,
  bool password
) {
  lv_obj_t *textarea = lv_textarea_create(parent);
  lv_textarea_set_one_line(textarea, true);
  lv_textarea_set_password_mode(textarea, password);
  lv_textarea_set_text(textarea, value);
  lv_obj_set_pos(textarea, x, y);
  lv_obj_set_size(textarea, 430, 42);
  lv_obj_add_event_cb(textarea, ui_textarea_event, LV_EVENT_ALL, keyboard);
  return textarea;
}

static void ui_show_settings_screen(lv_event_t *e) {
  (void)e;
  lv_scr_load_anim(screen_settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

static void ui_show_wifi_screen(lv_event_t *e) {
  (void)e;
  lv_scr_load_anim(screen_wifi, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

static void ui_show_ntp_screen(lv_event_t *e) {
  (void)e;
  lv_scr_load_anim(screen_ntp, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

static void ui_show_deye_screen(lv_event_t *e) {
  (void)e;
  lv_scr_load_anim(screen_deye, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

static void ui_scan_wifi(lv_event_t *e) {
  (void)e;
  lv_label_set_text(label_wifi_scan, "Scan en cours...");
  lv_timer_handler();

  // Désactiver le WiFi pour éviter les interférences
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  int count = WiFi.scanNetworks();
  
  if (count <= 0) {
    lv_label_set_text(label_wifi_scan, "Aucun reseau trouve");
    WiFi.scanDelete();
    return;
  }

  // Construire la liste des SSID
  String names = "";
  for (int i = 0; i < count; i++) {
    if (i > 0) names += "\n";
    names += WiFi.SSID(i);
  }

  // Mettre à jour le dropdown
  lv_dropdown_set_options(dropdown_wifi, names.c_str());
  
  // Sélectionner le réseau actuel si présent
  String current_ssid = cfg_wifi_ssid;
  for (int i = 0; i < count; i++) {
    if (WiFi.SSID(i) == current_ssid) {
      lv_dropdown_set_selected(dropdown_wifi, i);
      break;
    }
  }

  // Ouvrir le dropdown pour montrer les résultats
  lv_dropdown_open(dropdown_wifi);
  
  char msg[64];
  snprintf(msg, sizeof(msg), "%d reseaux trouves", count);
  lv_label_set_text(label_wifi_scan, msg);
  
  // Libérer la mémoire du scan
  WiFi.scanDelete();
}

static void ui_save_wifi(lv_event_t *e) {
  (void)e;
  char ssid[64];
  lv_dropdown_get_selected_str(dropdown_wifi, ssid, sizeof(ssid));
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

static void ui_save_deye(lv_event_t *e) {
  (void)e;
  uint32_t serial = strtoul(lv_textarea_get_text(textarea_logger_serial), nullptr, 10);
  settings_save_deye(lv_textarea_get_text(textarea_deye_host), serial);
  ui_settings_restart_message();
}

static void ui_settings_create() {
  screen_settings = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_settings, lv_color_black(), LV_PART_MAIN);
  lv_obj_clear_flag(screen_settings, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_settings, "CONFIGURATION");

  ui_settings_make_button(screen_settings, "WIFI", 80, 90, 320, 60, ui_show_wifi_screen);
  ui_settings_make_button(screen_settings, "HEURE / NTP", 80, 180, 320, 60, ui_show_ntp_screen);
  ui_settings_make_button(screen_settings, "DEYE / SOLARMAN", 80, 270, 320, 60, ui_show_deye_screen);
  ui_settings_make_button(screen_settings, "RETOUR", 140, 385, 200, 50, ui_show_dashboard);

  screen_wifi = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_wifi, lv_color_black(), LV_PART_MAIN);
  lv_obj_clear_flag(screen_wifi, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_wifi, "CONFIGURATION WIFI");

  lv_obj_t *label = lv_label_create(screen_wifi);
  lv_label_set_text(label, "Reseau Wi-Fi");
  lv_obj_set_pos(label, 20, 55);

  dropdown_wifi = lv_dropdown_create(screen_wifi);
  lv_dropdown_set_options(dropdown_wifi, cfg_wifi_ssid.c_str());
  lv_obj_set_pos(dropdown_wifi, 20, 82);
  lv_obj_set_size(dropdown_wifi, 300, 45);
  ui_settings_make_button(screen_wifi, "SCAN", 335, 82, 115, 45, ui_scan_wifi);

  label = lv_label_create(screen_wifi);
  lv_label_set_text(label, "Mot de passe");
  lv_obj_set_pos(label, 20, 145);

  lv_obj_t *kb_wifi = lv_keyboard_create(screen_wifi);
  lv_obj_set_size(kb_wifi, LCD_W, 190);
  lv_obj_align(kb_wifi, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(kb_wifi, LV_OBJ_FLAG_HIDDEN);

  textarea_wifi_password = ui_settings_make_textarea(
    screen_wifi,
    kb_wifi,
    cfg_wifi_password.c_str(),
    20, 172,
    true
  );

  label_wifi_scan = lv_label_create(screen_wifi);
  lv_label_set_text(label_wifi_scan, "");
  lv_obj_set_pos(label_wifi_scan, 20, 235);

  ui_settings_make_button(screen_wifi, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_wifi, "SAUVEGARDER", 250, 370, 200, 55, ui_save_wifi);

  screen_ntp = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_ntp, lv_color_black(), LV_PART_MAIN);
  lv_obj_clear_flag(screen_ntp, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_ntp, "CONFIGURATION HEURE / NTP");

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Fuseau horaire avec ete/hiver");
  lv_obj_set_pos(label, 20, 52);

  dropdown_timezone = lv_dropdown_create(screen_ntp);
  lv_dropdown_set_options(dropdown_timezone, timezone_names);
  lv_obj_set_pos(dropdown_timezone, 20, 80);
  lv_obj_set_size(dropdown_timezone, 430, 45);

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Serveur NTP principal");
  lv_obj_set_pos(label, 20, 140);

  lv_obj_t *kb_ntp = lv_keyboard_create(screen_ntp);
  lv_obj_set_size(kb_ntp, LCD_W, 190);
  lv_obj_align(kb_ntp, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(kb_ntp, LV_OBJ_FLAG_HIDDEN);

  textarea_ntp_primary = ui_settings_make_textarea(
    screen_ntp,
    kb_ntp,
    cfg_ntp_primary.c_str(),
    20, 167,
    false
  );

  label = lv_label_create(screen_ntp);
  lv_label_set_text(label, "Serveur NTP secondaire");
  lv_obj_set_pos(label, 20, 225);

  textarea_ntp_secondary = ui_settings_make_textarea(
    screen_ntp,
    kb_ntp,
    cfg_ntp_secondary.c_str(),
    20, 252,
    false
  );

  ui_settings_make_button(screen_ntp, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_ntp, "SAUVEGARDER", 250, 370, 200, 55, ui_save_ntp);

  screen_deye = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_deye, lv_color_black(), LV_PART_MAIN);
  lv_obj_clear_flag(screen_deye, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(screen_deye, "CONFIGURATION DEYE");

  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Adresse IP du logger");
  lv_obj_set_pos(label, 20, 70);

  lv_obj_t *kb_deye_text = lv_keyboard_create(screen_deye);
  lv_obj_set_size(kb_deye_text, LCD_W, 190);
  lv_obj_align(kb_deye_text, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(kb_deye_text, LV_OBJ_FLAG_HIDDEN);

  textarea_deye_host = ui_settings_make_textarea(
    screen_deye,
    kb_deye_text,
    cfg_deye_host.c_str(),
    20, 100,
    false
  );

  label = lv_label_create(screen_deye);
  lv_label_set_text(label, "Numero de serie du logger");
  lv_obj_set_pos(label, 20, 180);

  lv_obj_t *kb_deye_number = lv_keyboard_create(screen_deye);
  lv_keyboard_set_mode(kb_deye_number, LV_KEYBOARD_MODE_NUMBER);
  lv_obj_set_size(kb_deye_number, LCD_W, 190);
  lv_obj_align(kb_deye_number, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(kb_deye_number, LV_OBJ_FLAG_HIDDEN);

  char serial[16];
  snprintf(serial, sizeof(serial), "%lu", (unsigned long)cfg_logger_serial);

  textarea_logger_serial = ui_settings_make_textarea(
    screen_deye,
    kb_deye_number,
    serial,
    20, 210,
    false
  );

  ui_settings_make_button(screen_deye, "RETOUR", 20, 370, 190, 55, ui_show_settings_screen);
  ui_settings_make_button(screen_deye, "SAUVEGARDER", 250, 370, 200, 55, ui_save_deye);
}
