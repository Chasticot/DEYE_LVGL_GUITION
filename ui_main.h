#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "app_data.h"
#include "wifi_manager.h"
#include "ntp_manager.h"

extern bool deye_on_grid_state;

extern void ui_show_settings(lv_event_t *e);

extern uint16_t grid_voltage_l1;
extern uint16_t grid_voltage_l2;
extern uint16_t grid_voltage_l3;
extern bool grid_voltage_valid;

extern bool deye_on_grid_state;

extern uint16_t deye_get_pv_daily();
extern uint16_t deye_get_daily_grid_buy();
extern uint16_t deye_get_daily_grid_sell();
extern uint16_t deye_get_daily_load();
extern bool deye_is_on_grid();

static lv_obj_t *screen_main = nullptr;

static lv_obj_t *label_time = nullptr;
static lv_obj_t *label_date = nullptr;
static lv_obj_t *label_day = nullptr;
static lv_obj_t *label_wifi = nullptr;

static lv_obj_t *label_pv_total = nullptr;
static lv_obj_t *label_pv_detail = nullptr;

static lv_obj_t *arc_soc = nullptr;
static lv_obj_t *label_soc = nullptr;
static lv_obj_t *label_battery_voltage = nullptr;
static lv_obj_t *label_battery_power = nullptr;
static lv_obj_t *label_battery_state = nullptr;

static lv_obj_t *label_load = nullptr;
static lv_obj_t *label_ups = nullptr;

static lv_obj_t *label_grid_title = nullptr;
static lv_obj_t *label_grid_power = nullptr;
static lv_obj_t *label_grid_direction = nullptr;

static lv_obj_t *label_smartload = nullptr;
static lv_obj_t *label_temp = nullptr;

static lv_obj_t *led_wifi = nullptr;
static lv_obj_t *led_deye = nullptr;

static lv_obj_t *label_pv_daily = nullptr;


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
  // Créer un conteneur pour le titre en haut du bloc
  lv_obj_t *title_container = lv_obj_create(parent);
  lv_obj_set_size(title_container, lv_obj_get_width(parent) - 16, 28);
  lv_obj_align(title_container, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_opa(title_container, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(title_container, 0, LV_PART_MAIN);
  lv_obj_clear_flag(title_container, LV_OBJ_FLAG_SCROLLABLE);
  
  // Créer le label du titre
  lv_obj_t *label = lv_label_create(title_container);
  lv_label_set_text(label, text);
  lv_obj_set_width(label, lv_obj_get_width(title_container));
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_center(label);
}

static void ui_main_create() {
  screen_main = lv_obj_create(nullptr);

  lv_obj_set_style_bg_color(
    screen_main,
    lv_color_hex(0x050A12),
    LV_PART_MAIN
  );

  lv_obj_set_style_bg_opa(
    screen_main,
    LV_OPA_COVER,
    LV_PART_MAIN
  );

  lv_obj_clear_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);

  // ======================================================
  // BANDEAU SUPÉRIEUR (Header)
  // ======================================================
  lv_obj_t *header = ui_main_make_card(
    screen_main,
    10, 8,
    460, 92
  );

  label_day = ui_main_label(
    header,
    "DEYE MONITOR",
    150,
    &lv_font_montserrat_16,
    lv_color_hex(0x55D6FF),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_day, LV_ALIGN_TOP_LEFT, 4, 3);

  label_date = ui_main_label(
    header,
    "--/--/----",
    150,
    &lv_font_montserrat_16,
    lv_color_white(),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_date, LV_ALIGN_BOTTOM_LEFT, 4, -2);

  label_time = ui_main_label(
    header,
    "--:--",
    180,
    &lv_font_montserrat_38,
    lv_color_hex(0x00E5FF),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_time, LV_ALIGN_CENTER, 15, 0);

  // LEDs et WiFi
  led_wifi = lv_led_create(header);
  lv_obj_set_size(led_wifi, 14, 14);
  lv_obj_align(led_wifi, LV_ALIGN_RIGHT_MID, -92, -18);
  lv_led_set_color(led_wifi, lv_palette_main(LV_PALETTE_RED));
  lv_led_on(led_wifi);

  led_deye = lv_led_create(header);
  lv_obj_set_size(led_deye, 14, 14);
  lv_obj_align(led_deye, LV_ALIGN_RIGHT_MID, -66, -18);
  lv_led_set_color(led_deye, lv_palette_main(LV_PALETTE_RED));
  lv_led_on(led_deye);

  label_wifi = ui_main_label(
    header,
    "WIFI : --%",
    150,
    &lv_font_montserrat_14,
    lv_color_white(),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_wifi, LV_ALIGN_RIGHT_MID, -4, 20);

  // Bouton Settings
  lv_obj_t *settings_btn = lv_btn_create(header);
  lv_obj_set_size(settings_btn, 50, 50);
  lv_obj_align(settings_btn, LV_ALIGN_RIGHT_MID, -4, -12);
  lv_obj_set_style_bg_color(
    settings_btn,
    lv_color_hex(0x1D4ED8),
    LV_PART_MAIN
  );
  lv_obj_set_style_radius(settings_btn, 10, LV_PART_MAIN);
  lv_obj_add_event_cb(
    settings_btn,
    ui_show_settings,
    LV_EVENT_CLICKED,
    nullptr
  );

  lv_obj_t *settings_icon = ui_main_label(
    settings_btn,
    "CFG",
    46,
    &lv_font_montserrat_14,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_center(settings_icon);

  // ======================================================
  // BLOC PRODUCTION PV
  // ======================================================
lv_obj_t *pv_card = ui_main_make_card(
  screen_main,
  10, 110,
  225, 150
);

// TITRE PV
lv_obj_t *pv_title = lv_label_create(pv_card);
lv_label_set_text(pv_title, "PRODUCTION PV");
lv_obj_set_width(pv_title, 205);
lv_obj_set_style_text_font(pv_title, &lv_font_montserrat_16, LV_PART_MAIN);
lv_obj_set_style_text_color(pv_title, lv_color_hex(0x00FF88), LV_PART_MAIN);
lv_obj_set_style_text_align(pv_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
lv_obj_align(pv_title, LV_ALIGN_TOP_MID, 0, 4);

// Label pour la production instantanée (police réduite)
label_pv_total = ui_main_label(
  pv_card,
  "0 W",
  205,
  &lv_font_montserrat_32,  // Réduit de 38 à 32
  lv_color_white(),
  LV_TEXT_ALIGN_CENTER
);
lv_obj_align(label_pv_total, LV_ALIGN_CENTER, 0, -10);

// Label pour la production journalière (nouveau)
label_pv_daily = lv_label_create(pv_card);
lv_label_set_text(label_pv_daily, "JOUR: 0.0 kWh");
lv_obj_set_width(label_pv_daily, 205);
lv_obj_set_style_text_font(label_pv_daily, &lv_font_montserrat_14, LV_PART_MAIN);
lv_obj_set_style_text_color(label_pv_daily, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
lv_obj_set_style_text_align(label_pv_daily, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
lv_obj_align(label_pv_daily, LV_ALIGN_CENTER, 0, 22);

// Détails PV1 PV2 PV3
label_pv_detail = ui_main_label(
  pv_card,
  "PV1 0W  PV2 0W  PV3 0W",
  205,
  &lv_font_montserrat_12,
  lv_color_hex(0xD1D5DB),
  LV_TEXT_ALIGN_CENTER
);
lv_obj_align(label_pv_detail, LV_ALIGN_BOTTOM_MID, 0, -4);

// Stocker le label de production journalière pour mise à jour
static lv_obj_t *label_pv_daily_global = nullptr;
label_pv_daily_global = label_pv_daily;

  // ======================================================
  // BLOC BATTERIE
  // ======================================================
  lv_obj_t *bat_card = ui_main_make_card(
    screen_main,
    245, 110,
    225, 150
  );

  // TITRE BATTERIE
  lv_obj_t *bat_title = lv_label_create(bat_card);
  lv_label_set_text(bat_title, "BATTERIE");
  lv_obj_set_width(bat_title, 205);
  lv_obj_set_style_text_font(bat_title, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(bat_title, lv_color_hex(0x00E5FF), LV_PART_MAIN);
  lv_obj_set_style_text_align(bat_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(bat_title, LV_ALIGN_TOP_MID, 0, 4);

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
  lv_obj_set_style_arc_color(
    arc_soc,
    lv_color_hex(0x263548),
    LV_PART_MAIN
  );

  lv_obj_set_style_arc_color(
    arc_soc,
    lv_color_hex(0x22C55E),
    LV_PART_INDICATOR
  );

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
  lv_obj_t *load_card = ui_main_make_card(
    screen_main,
    10, 270,
    225, 150
  );

  // TITRE CONSOMMATION
  lv_obj_t *load_title = lv_label_create(load_card);
  lv_label_set_text(load_title, "CONSOMMATION");
  lv_obj_set_width(load_title, 205);
  lv_obj_set_style_text_font(load_title, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(load_title, lv_color_hex(0xFACC15), LV_PART_MAIN);
  lv_obj_set_style_text_align(load_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(load_title, LV_ALIGN_TOP_MID, 0, 4);

  label_load = ui_main_label(
    load_card,
    "0 W",
    205,
    &lv_font_montserrat_38,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -5);

  label_ups = ui_main_label(
    load_card,
    "LOAD : 0 W   UPS : 0 W",
    205,
    &lv_font_montserrat_12,
    lv_color_hex(0xD1D5DB),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_ups, LV_ALIGN_BOTTOM_MID, 0, -5);

  // ======================================================
  // BLOC RESEAU
  // ======================================================
  lv_obj_t *grid_card = ui_main_make_card(
    screen_main,
    245, 270,
    225, 150
  );

  // TITRE RESEAU (celui-ci fonctionne déjà)
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
    &lv_font_montserrat_38,
    lv_color_white(),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_power, LV_ALIGN_CENTER, 0, -5);

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
  // BANDEAU INFÉRIEUR (SmartLoad + Températures)
  // ======================================================
  lv_obj_t *bottom_card = ui_main_make_card(
    screen_main,
    10, 430,
    460, 42
  );

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
    &lv_font_montserrat_14,
    lv_color_white(),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_temp, LV_ALIGN_RIGHT_MID, -3, 0);
}

static void ui_main_update() {
  if (screen_main == nullptr) return;

  char text[128];
  struct tm timeinfo;

  // Heure et date.
  if (ntp_manager_get_local_time(&timeinfo)) {
    char time_text[16];
    char date_text[16];
    char day_text[20];

    const char *days[] = {
      "DIMANCHE",
      "LUNDI",
      "MARDI",
      "MERCREDI",
      "JEUDI",
      "VENDREDI",
      "SAMEDI"
    };

    strftime(time_text, sizeof(time_text), "%H:%M", &timeinfo);
    strftime(date_text, sizeof(date_text), "%d/%m/%Y", &timeinfo);

    snprintf(
      day_text,
      sizeof(day_text),
      "%s",
      days[timeinfo.tm_wday]
    );

    lv_label_set_text(label_time, time_text);
    lv_label_set_text(label_date, date_text);
    lv_label_set_text(label_day, day_text);
  }

  // Wi-Fi.
  if (WiFi.status() == WL_CONNECTED) {
    snprintf(
      text,
      sizeof(text),
      "WIFI %d%%",
      wifi_quality_percent()
    );

    lv_led_set_color(
      led_wifi,
      lv_palette_main(LV_PALETTE_GREEN)
    );
  } else {
    snprintf(text, sizeof(text), "WIFI OFF");

    lv_led_set_color(
      led_wifi,
      lv_palette_main(LV_PALETTE_RED)
    );
  }

  lv_label_set_text(label_wifi, text);

  // Données simulées maintenant, Solarman plus tard.
  lv_led_set_color(
    led_deye,
    dashboard_data.valid
      ? lv_palette_main(LV_PALETTE_GREEN)
      : lv_palette_main(LV_PALETTE_RED)
  );

  uint32_t pv_total =
    dashboard_data.pv1_w +
    dashboard_data.pv2_w +
    dashboard_data.pv3_w;

snprintf(text, sizeof(text), "%lu W", (unsigned long)pv_total);  // Ajout de " W"
lv_label_set_text(label_pv_total, text);

  // Production journalière (à ajouter)
  // Récupérer la valeur depuis deye_solarman.h
  extern uint16_t pv_daily_yield;
  extern bool pv_daily_yield_valid;
  
  if (pv_daily_yield_valid) {
    float daily_kwh = pv_daily_yield * 0.1f;
    snprintf(text, sizeof(text), "JOUR: %.1f kWh", daily_kwh);
  } else {
    snprintf(text, sizeof(text), "JOUR: --.- kWh");
  }
  lv_label_set_text(label_pv_daily, text);

  // Détails PV

snprintf(
  text,
  sizeof(text),
  "PV1 %uW  PV2 %uW  PV3 %uW",
  dashboard_data.pv1_w,
  dashboard_data.pv2_w,
  dashboard_data.pv3_w
);
lv_label_set_text(label_pv_detail, text);

  // Batterie.
  uint16_t soc = constrain(dashboard_data.battery_soc, 0, 100);
  lv_arc_set_value(arc_soc, soc);

  if (soc < 20) {
    lv_obj_set_style_arc_color(
      arc_soc,
      lv_color_hex(0xEF4444),
      LV_PART_INDICATOR
    );
  } else if (soc < 50) {
    lv_obj_set_style_arc_color(
      arc_soc,
      lv_color_hex(0xFACC15),
      LV_PART_INDICATOR
    );
  } else {
    lv_obj_set_style_arc_color(
      arc_soc,
      lv_color_hex(0x22C55E),
      LV_PART_INDICATOR
    );
  }

  snprintf(text, sizeof(text), "%u%%", soc);
  lv_label_set_text(label_soc, text);

  snprintf(
    text,
    sizeof(text),
    "%.2f V",
    dashboard_data.battery_voltage
  );
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
  lv_obj_set_style_text_color(
    label_battery_state,
    battery_color,
    LV_PART_MAIN
  );

  snprintf(
    text,
    sizeof(text),
    "%d W",
    dashboard_data.battery_power
  );
  lv_label_set_text(label_battery_power, text);

  // Charge - Afficher la somme LOAD + UPS en gros
uint32_t total_consumption = dashboard_data.load_power + dashboard_data.ups_power;
snprintf(text, sizeof(text), "%lu W", (unsigned long)total_consumption);
lv_label_set_text(label_load, text);

// Détails LOAD et UPS en dessous
snprintf(
  text,
  sizeof(text),
  "LOAD : %d W   UPS : %d W",
  dashboard_data.load_power,
  dashboard_data.ups_power
);
lv_label_set_text(label_ups, text);

  //réseau
  
    int16_t grid_power = dashboard_data.grid_power;
  int32_t grid_abs = abs(grid_power);

  // Utiliser l'état détecté par deye_solarman.h
  bool on_grid = deye_on_grid_state;

  // Affichage de la puissance
  snprintf(text, sizeof(text), "%ld W", (long)grid_abs);
  lv_label_set_text(label_grid_power, text);

  // Affichage ON GRID / OFF GRID
  const char *grid_status = on_grid ? "ON GRID" : "OFF GRID";
  lv_label_set_text(label_grid_direction, grid_status);

  // Couleur : vert pour ON GRID, rouge pour OFF GRID
  lv_color_t grid_color = on_grid 
    ? lv_color_hex(0x22C55E)   // Vert
    : lv_color_hex(0xEF4444);  // Rouge

  lv_obj_set_style_text_color(
    label_grid_direction,
    grid_color,
    LV_PART_MAIN
  );

  lv_obj_set_style_text_color(
    label_grid_title,
    grid_color,
    LV_PART_MAIN
  );

  // SmartLoad et températures.
  lv_label_set_text(
    label_smartload,
    dashboard_data.smartload_on
      ? "SMARTLOAD : ON"
      : "SMARTLOAD : OFF"
  );

  lv_obj_set_style_text_color(
    label_smartload,
    dashboard_data.smartload_on
      ? lv_color_hex(0x22C55E)
      : lv_color_hex(0xFB7185),
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

void ui_show_settings(lv_event_t *e);