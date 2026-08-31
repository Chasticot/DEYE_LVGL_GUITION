#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "app_data.h"
#include "wifi_manager.h"
#include "ntp_manager.h"

// ==================== VARIABLES STATIQUES ====================
static lv_obj_t *screen_main = nullptr;

static lv_obj_t *label_date = nullptr;      // Date en haut à gauche
static lv_obj_t *label_time = nullptr;      // Heure centrée
static lv_obj_t *label_wifi = nullptr;

static lv_obj_t *label_pv_total = nullptr;
static lv_obj_t *label_pv_detail = nullptr;
static lv_obj_t *label_pv_daily = nullptr;

static lv_obj_t *arc_soc = nullptr;
static lv_obj_t *label_soc = nullptr;
static lv_obj_t *label_battery_voltage = nullptr;
static lv_obj_t *label_battery_power = nullptr;
static lv_obj_t *label_battery_state = nullptr;

static lv_obj_t *label_load = nullptr;
static lv_obj_t *label_load_daily = nullptr;

static lv_obj_t *label_grid_title = nullptr;
static lv_obj_t *label_grid_power = nullptr;
static lv_obj_t *label_grid_daily = nullptr;
static lv_obj_t *label_grid_direction = nullptr;

static lv_obj_t *label_smartload = nullptr;
static lv_obj_t *label_temp = nullptr;

static lv_obj_t *led_wifi = nullptr;
static lv_obj_t *led_deye = nullptr;

// ==================== FONCTIONS EXTERNES ====================
extern void ui_show_settings(lv_event_t *e);

// ==================== FONCTIONS UI ====================
static lv_color_t ui_card_color() {
  return lv_color_hex(0x111827);
}

static lv_color_t ui_border_color() {
  return lv_color_hex(0x273449);
}

static lv_obj_t *ui_main_label(
  lv_obj_t *parent,
  const char *text,
  lv_coord_t width,
  const lv_font_t *font,
  lv_color_t color,
  lv_text_align_t align
) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_width(label, width);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, align, LV_PART_MAIN);
  return label;
}

static void ui_main_style_card(
  lv_obj_t *card,
  lv_coord_t x,
  lv_coord_t y,
  lv_coord_t width,
  lv_coord_t height
) {
  lv_obj_set_pos(card, x, y);
  lv_obj_set_size(card, width, height);
  lv_obj_set_style_bg_color(card, ui_card_color(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_color(card, ui_border_color(), LV_PART_MAIN);
  lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(card, 12, LV_PART_MAIN);
  lv_obj_set_style_pad_all(card, 8, LV_PART_MAIN);
  lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *ui_main_make_card(
  lv_obj_t *parent,
  lv_coord_t x,
  lv_coord_t y,
  lv_coord_t width,
  lv_coord_t height
) {
  lv_obj_t *card = lv_obj_create(parent);
  ui_main_style_card(card, x, y, width, height);
  return card;
}

static void ui_main_create_title(
  lv_obj_t *parent,
  const char *text,
  lv_color_t color
) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_width(label, lv_obj_get_width(parent) - 16);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 4);
}

// ==================== CRÉATION DE L'INTERFACE ====================
static void ui_main_create() {
  screen_main = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_main, lv_color_hex(0x050A12), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_main, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);

  // ======================================================
  // BANDEAU SUPÉRIEUR
  // ======================================================
  lv_obj_t *header = ui_main_make_card(screen_main, 10, 8, 460, 92);

  // LIGNE 1 : DATE (JJ/MM/AAAA) | DEYE MONITOR | CFG
  label_date = ui_main_label(
    header,
    "--/--/----",
    120,
    &lv_font_montserrat_16,
    lv_color_hex(0x55D6FF),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_date, LV_ALIGN_TOP_LEFT, 4, 3);

  lv_obj_t *label_deye_monitor = ui_main_label(
    header,
    "DEYE MONITOR",
    150,
    &lv_font_montserrat_16,
    lv_color_hex(0x55D6FF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_deye_monitor, LV_ALIGN_TOP_MID, 0, 3);

  // BOUTON CFG
  lv_obj_t *settings_btn = lv_btn_create(header);
  lv_obj_set_size(settings_btn, 44, 30);
  lv_obj_align(settings_btn, LV_ALIGN_TOP_RIGHT, -4, 0);
  lv_obj_set_style_bg_color(settings_btn, lv_color_hex(0x1D4ED8), LV_PART_MAIN);
  lv_obj_set_style_radius(settings_btn, 6, LV_PART_MAIN);
  lv_obj_add_event_cb(settings_btn, ui_show_settings, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *settings_icon = ui_main_label(
    settings_btn,
    "CFG",
    40,
    &lv_font_montserrat_12,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_center(settings_icon);

  // LIGNE 2 : HEURE (centrée)
  label_time = ui_main_label(
    header,
    "--:--",
    160,
    &lv_font_montserrat_38,
    lv_color_hex(0x00E5FF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 4);

  // LIGNE 3 : ● SSID WIFI 85% | ● DEYE
  led_wifi = lv_led_create(header);
  lv_obj_set_size(led_wifi, 12, 12);
  lv_obj_align(led_wifi, LV_ALIGN_BOTTOM_LEFT, 4, -10);
  lv_led_set_color(led_wifi, lv_palette_main(LV_PALETTE_RED));
  lv_led_on(led_wifi);

  label_wifi = ui_main_label(
    header,
    "SSID 0%",
    150,
    &lv_font_montserrat_14,
    lv_color_white(),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_wifi, LV_ALIGN_BOTTOM_LEFT, 20, -10);

  led_deye = lv_led_create(header);
  lv_obj_set_size(led_deye, 12, 12);
  lv_obj_align(led_deye, LV_ALIGN_BOTTOM_RIGHT, -4, -10);
  lv_led_set_color(led_deye, lv_palette_main(LV_PALETTE_RED));
  lv_led_on(led_deye);

  lv_obj_t *label_deye_text = ui_main_label(
    header,
    "DEYE",
    60,
    &lv_font_montserrat_14,
    lv_color_white(),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_deye_text, LV_ALIGN_BOTTOM_RIGHT, -20, -10);

  // ======================================================
  // BLOC PRODUCTION PV
  // ======================================================
  lv_obj_t *pv_card = ui_main_make_card(screen_main, 10, 110, 225, 150);

  ui_main_create_title(pv_card, "PRODUCTION PV", lv_color_hex(0x00FF88));

  label_pv_total = ui_main_label(
    pv_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_total, LV_ALIGN_CENTER, 0, -12);

  label_pv_daily = ui_main_label(
    pv_card,
    "JOUR: 0.0 kWh",
    205,
    &lv_font_montserrat_14,
    lv_color_hex(0x9CA3AF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_daily, LV_ALIGN_CENTER, 0, 20);

  label_pv_detail = ui_main_label(
    pv_card,
    "PV1 0W  PV2 0W  PV3 0W",
    205,
    &lv_font_montserrat_12,
    lv_color_hex(0xD1D5DB),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_detail, LV_ALIGN_BOTTOM_MID, 0, -4);

  // ======================================================
  // BLOC BATTERIE
  // ======================================================
  lv_obj_t *bat_card = ui_main_make_card(screen_main, 245, 110, 225, 150);

  ui_main_create_title(bat_card, "BATTERIE", lv_color_hex(0x00E5FF));

  arc_soc = lv_arc_create(bat_card);
  lv_obj_remove_style(arc_soc, nullptr, LV_PART_KNOB);
  lv_obj_clear_flag(arc_soc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(arc_soc, 94, 94);
  lv_obj_align(arc_soc, LV_ALIGN_LEFT_MID, 8, 12);

  lv_arc_set_rotation(arc_soc, 135);
  lv_arc_set_bg_angles(arc_soc, 0, 270);
  lv_arc_set_range(arc_soc, 0, 100);
  lv_arc_set_value(arc_soc, 0);

  lv_obj_set_style_arc_width(arc_soc, 10, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc_soc, 10, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0x263548), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0x22C55E), LV_PART_INDICATOR);

  label_soc = ui_main_label(
    bat_card,
    "0%",
    90,
    &lv_font_montserrat_26,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_soc, LV_ALIGN_LEFT_MID, 10, 12);

  label_battery_voltage = ui_main_label(
    bat_card,
    "--.-- V",
    100,
    &lv_font_montserrat_20,
    lv_color_white(),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_battery_voltage, LV_ALIGN_TOP_RIGHT, -5, 42);

  label_battery_state = ui_main_label(
    bat_card,
    "REPOS",
    100,
    &lv_font_montserrat_16,
    lv_color_hex(0xFACC15),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_battery_state, LV_ALIGN_TOP_RIGHT, -5, 73);

  label_battery_power = ui_main_label(
    bat_card,
    "0 W",
    100,
    &lv_font_montserrat_20,
    lv_color_white(),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_battery_power, LV_ALIGN_TOP_RIGHT, -5, 101);

  // ======================================================
  // BLOC CONSOMMATION
  // ======================================================
  lv_obj_t *load_card = ui_main_make_card(screen_main, 10, 270, 225, 150);

  ui_main_create_title(load_card, "CONSOMMATION", lv_color_hex(0xFACC15));

  label_load = ui_main_label(
    load_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -12);

  label_load_daily = ui_main_label(
    load_card,
    "Jour: 0.0 kWh",
    205,
    &lv_font_montserrat_14,
    lv_color_hex(0x9CA3AF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_load_daily, LV_ALIGN_CENTER, 0, 20);

  // ======================================================
  // BLOC RESEAU
  // ======================================================
  lv_obj_t *grid_card = ui_main_make_card(screen_main, 245, 270, 225, 150);

  label_grid_title = ui_main_label(
    grid_card,
    "RESEAU",
    205,
    &lv_font_montserrat_16,
    lv_color_hex(0xFB7185),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_title, LV_ALIGN_TOP_MID, 0, 4);

  label_grid_power = ui_main_label(
    grid_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_power, LV_ALIGN_CENTER, 0, -12);

  label_grid_daily = ui_main_label(
    grid_card,
    "D.Sell: 0.0kWh  D.Buy: 0.0kWh",
    205,
    &lv_font_montserrat_12,
    lv_color_hex(0x9CA3AF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_daily, LV_ALIGN_CENTER, 0, 20);

  label_grid_direction = ui_main_label(
    grid_card,
    "IMPORT",
    205,
    &lv_font_montserrat_16,
    lv_color_hex(0xFB7185),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_direction, LV_ALIGN_BOTTOM_MID, 0, -5);

  // ======================================================
  // BANDEAU INFÉRIEUR
  // ======================================================
  lv_obj_t *bottom_card = ui_main_make_card(screen_main, 10, 430, 460, 42);

  label_smartload = ui_main_label(
    bottom_card,
    "SMARTLOAD : --",
    175,
    &lv_font_montserrat_16,
    lv_color_hex(0xFACC15),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_smartload, LV_ALIGN_LEFT_MID, 3, 0);

  label_temp = ui_main_label(
    bottom_card,
    "DC --.-C  AC --.-C  BAT --.-C",
    270,
    &lv_font_montserrat_12,
    lv_color_white(),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_temp, LV_ALIGN_RIGHT_MID, -3, 0);
}

// ==================== MISE À JOUR DE L'INTERFACE ====================
static void ui_main_update() {
  if (screen_main == nullptr) return;

  char text[128];
  struct tm timeinfo;

  // HEURE ET DATE
  if (ntp_manager_get_local_time(&timeinfo)) {
    char time_text[16];
    char date_text[16];

    strftime(time_text, sizeof(time_text), "%H:%M", &timeinfo);
    strftime(date_text, sizeof(date_text), "%d/%m/%Y", &timeinfo);

    lv_label_set_text(label_time, time_text);
    lv_label_set_text(label_date, date_text);
  }

  // WIFI
  if (WiFi.status() == WL_CONNECTED) {
    snprintf(text, sizeof(text), "%s %d%%", WiFi.SSID().c_str(), wifi_quality_percent());
    lv_led_set_color(led_wifi, lv_palette_main(LV_PALETTE_GREEN));
  } else {
    snprintf(text, sizeof(text), "WIFI OFF");
    lv_led_set_color(led_wifi, lv_palette_main(LV_PALETTE_RED));
  }
  lv_label_set_text(label_wifi, text);

  // DEYE
  lv_led_set_color(
    led_deye,
    dashboard_data.valid
      ? lv_palette_main(LV_PALETTE_GREEN)
      : lv_palette_main(LV_PALETTE_RED)
  );

  // PRODUCTION PV
  uint32_t pv_total = dashboard_data.pv1_w + dashboard_data.pv2_w + dashboard_data.pv3_w;

  snprintf(text, sizeof(text), "%lu W", (unsigned long)pv_total);
  lv_label_set_text(label_pv_total, text);

  snprintf(
    text,
    sizeof(text),
    "PV1 %uW  PV2 %uW  PV3 %uW",
    dashboard_data.pv1_w,
    dashboard_data.pv2_w,
    dashboard_data.pv3_w
  );
  lv_label_set_text(label_pv_detail, text);

  if (pv_daily_yield_valid) {
    snprintf(text, sizeof(text), "JOUR: %.1f kWh", pv_daily_yield * 0.1f);
  } else {
    snprintf(text, sizeof(text), "JOUR: --.- kWh");
  }
  lv_label_set_text(label_pv_daily, text);

  // BATTERIE
  uint16_t soc = constrain(dashboard_data.battery_soc, 0, 100);
  lv_arc_set_value(arc_soc, soc);

  if (soc < 20) {
    lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0xEF4444), LV_PART_INDICATOR);
  } else if (soc < 50) {
    lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0xFACC15), LV_PART_INDICATOR);
  } else {
    lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0x22C55E), LV_PART_INDICATOR);
  }

  snprintf(text, sizeof(text), "%u%%", soc);
  lv_label_set_text(label_soc, text);

  snprintf(text, sizeof(text), "%.2f V", dashboard_data.battery_voltage);
  lv_label_set_text(label_battery_voltage, text);

  const char *battery_state = "REPOS";
  lv_color_t battery_color = lv_color_hex(0xFACC15);

  if (dashboard_data.battery_power < -5) {
    battery_state = "CHARGE";
    battery_color = lv_color_hex(0x22C55E);
  } else if (dashboard_data.battery_power > 5) {
    battery_state = "DECHARGE";
    battery_color = lv_color_hex(0xFB7185);
  }

  lv_label_set_text(label_battery_state, battery_state);
  lv_obj_set_style_text_color(label_battery_state, battery_color, LV_PART_MAIN);

  snprintf(text, sizeof(text), "%d W", dashboard_data.battery_power);
  lv_label_set_text(label_battery_power, text);

  // CONSOMMATION
  snprintf(text, sizeof(text), "%d W", dashboard_data.load_power);
  lv_label_set_text(label_load, text);

  float daily_load = deye_get_daily_load() * 0.1f;
  snprintf(text, sizeof(text), "Jour: %.1f kWh", daily_load);
  lv_label_set_text(label_load_daily, text);

  // RESEAU
  int16_t grid_power = dashboard_data.grid_power;
  bool on_grid = deye_on_grid_state;
  
  char sign = (grid_power < 0) ? '-' : ' ';
  int32_t grid_abs = abs(grid_power);
  
  snprintf(text, sizeof(text), "%c%ld W", sign, (long)grid_abs);
  lv_label_set_text(label_grid_power, text);

  float daily_sell = deye_get_daily_grid_sell() * 0.1f;
  float daily_buy = deye_get_daily_grid_buy() * 0.1f;

  snprintf(text, sizeof(text), "D.Sell: %.1fkWh  D.Buy: %.1fkWh", daily_sell, daily_buy);
  lv_label_set_text(label_grid_daily, text);

  const char *grid_status = on_grid ? "ON GRID" : "OFF GRID";
  lv_label_set_text(label_grid_direction, grid_status);

  lv_color_t grid_color;
  if (!on_grid) {
    grid_color = lv_color_hex(0xEF4444);
  } else if (grid_power < 0) {
    grid_color = lv_color_hex(0x22C55E);
  } else {
    grid_color = lv_color_hex(0xFB7185);
  }

  lv_obj_set_style_text_color(label_grid_direction, grid_color, LV_PART_MAIN);
  lv_obj_set_style_text_color(label_grid_title, grid_color, LV_PART_MAIN);
  
  if (grid_power < 0) {
    lv_obj_set_style_text_color(label_grid_power, lv_color_hex(0x22C55E), LV_PART_MAIN);
  } else if (grid_power > 0) {
    lv_obj_set_style_text_color(label_grid_power, lv_color_hex(0xFB7185), LV_PART_MAIN);
  } else {
    lv_obj_set_style_text_color(label_grid_power, lv_color_white(), LV_PART_MAIN);
  }

  // SMARTLOAD ET TEMPÉRATURES
  lv_label_set_text(
    label_smartload,
    dashboard_data.smartload_on ? "SMARTLOAD : ON" : "SMARTLOAD : OFF"
  );

  lv_obj_set_style_text_color(
    label_smartload,
    dashboard_data.smartload_on ? lv_color_hex(0x22C55E) : lv_color_hex(0xFB7185),
    LV_PART_MAIN
  );

  snprintf(
    text,
    sizeof(text),
    "DC %.1fC  AC %.1fC  BAT %.1fC",
    dashboard_data.dc_temperature,
    dashboard_data.ac_temperature,
    dashboard_data.battery_temperature
  );
  lv_label_set_text(label_temp, text);
}
