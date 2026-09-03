#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "app_data.h"
#include "wifi_manager.h"
#include "ntp_manager.h"
#include "tempo_api.h"
#include "ve_deye.h"

// ==================== VARIABLES STATIQUES ====================
static lv_obj_t *screen_main = nullptr;

static lv_obj_t *label_weekday = nullptr;
static lv_obj_t *label_weekday_outline[8] = {nullptr};
static lv_obj_t *label_date = nullptr;
static lv_obj_t *label_tomorrow = nullptr;
static lv_obj_t *label_tomorrow_outline[8] = {nullptr};
static lv_obj_t *label_time = nullptr;
static lv_obj_t *label_wifi = nullptr;
static lv_obj_t *label_monitoring_unavailable = nullptr;

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
static lv_obj_t *ev_charge_group = nullptr;
static lv_obj_t *car_icon = nullptr;
static lv_obj_t *label_ev_charge_power = nullptr;
static lv_obj_t *tempo_card = nullptr;
static lv_obj_t *label_tempo = nullptr;

static lv_obj_t *label_grid_title = nullptr;
static lv_obj_t *label_grid_power = nullptr;
static lv_obj_t *label_grid_daily = nullptr;
static lv_obj_t *label_grid_direction = nullptr;

static lv_obj_t *label_smartload = nullptr;
static lv_obj_t *label_temp = nullptr;

static lv_obj_t *wifi_arc_outer = nullptr;
static lv_obj_t *wifi_arc_middle = nullptr;
static lv_obj_t *wifi_arc_inner = nullptr;
static lv_obj_t *wifi_arc_bottom = nullptr;
static lv_obj_t *inverter_icon = nullptr;
static lv_obj_t *solar_sun = nullptr;
static lv_obj_t *solar_panel_cells[12] = {nullptr};
static lv_obj_t *solar_rays[7] = {nullptr};

// ==================== FONCTIONS EXTERNES ====================
extern void ui_show_settings(lv_event_t *e);
extern void ui_show_ve_deye(lv_event_t *e);
extern bool deye_copy_ev_snapshot(EvDeyeData *out);

// ==================== FONCTIONS UI ====================
static lv_color_t ui_card_color() {
  return lv_color_hex(ui_theme_palette(cfg_ui_theme).card_bg);
}

static lv_color_t ui_border_color() {
  return lv_color_hex(ui_theme_palette(cfg_ui_theme).border);
}

static lv_color_t ui_main_theme_color(uint32_t color) {
  return lv_color_hex(color);
}

// Couleurs Tempo partagées par les libellés et la tuile de statut.
static lv_color_t ui_tempo_blue_color() { return lv_color_hex(0x3B82F6); }
static lv_color_t ui_tempo_white_color() { return lv_color_white(); }
static lv_color_t ui_tempo_red_color() { return lv_color_hex(0xEF4444); }

static const char *ui_tempo_color_name(uint8_t color_code) {
  if (color_code == 1) return "BLEU";
  if (color_code == 2) return "BLANC";
  if (color_code == 3) return "ROUGE";
  return "--";
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

static lv_obj_t *ui_main_make_icon_container(lv_obj_t *parent, lv_coord_t width, lv_coord_t height) {
  lv_obj_t *container = lv_obj_create(parent);
  lv_obj_set_size(container, width, height);
  lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(container, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(container, 0, LV_PART_MAIN);
  lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
  return container;
}

static lv_obj_t *ui_main_make_wifi_arc(lv_obj_t *parent, lv_coord_t size, lv_coord_t inset) {
  lv_obj_t *arc = lv_arc_create(parent);
  lv_obj_set_size(arc, size, size);
  lv_obj_set_pos(arc, inset, inset);
  lv_arc_set_bg_angles(arc, 225, 315);
  lv_arc_set_value(arc, 100);
  lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
  lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, 1, LV_PART_INDICATOR);
  lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);
  lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  return arc;
}

static void ui_main_set_wifi_icon_color(lv_color_t color) {
  lv_obj_set_style_arc_color(wifi_arc_outer, color, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(wifi_arc_middle, color, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(wifi_arc_inner, color, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(wifi_arc_bottom, color, LV_PART_INDICATOR);
}

static void ui_main_set_wifi_icon_opa(lv_opa_t opa) {
  lv_obj_set_style_arc_opa(wifi_arc_outer, opa, LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(wifi_arc_middle, opa, LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(wifi_arc_inner, opa, LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(wifi_arc_bottom, opa, LV_PART_INDICATOR);
}

static void ui_main_set_inverter_icon_color(lv_color_t color) {
  lv_obj_set_style_bg_color(solar_sun, color, LV_PART_MAIN);
  for (uint8_t i = 0; i < 12; ++i) {
    lv_obj_set_style_bg_color(solar_panel_cells[i], color, LV_PART_MAIN);
  }
  for (uint8_t i = 0; i < 7; ++i) {
    lv_obj_set_style_bg_color(solar_rays[i], color, LV_PART_MAIN);
  }
}

// ==================== CRÉATION DE L'INTERFACE ====================
static void ui_main_create() {
  const UiThemePalette &theme = ui_theme_palette(cfg_ui_theme);
  screen_main = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_main, ui_main_theme_color(theme.dashboard_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_main, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);

  // ======================================================
  // BANDEAU SUPÉRIEUR
  // ======================================================
  lv_obj_t *header = ui_main_make_card(screen_main, 10, 8, 460, 92);

  // À gauche : jour puis date. À droite : états de liaison puis paramètres.
  label_weekday = ui_main_label(
    header,
    "---",
    120,
    &lv_font_montserrat_16,
    ui_main_theme_color(theme.accent),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_weekday, LV_ALIGN_TOP_LEFT, 4, 2);

  label_date = ui_main_label(
    header,
    "--/--/----",
    120,
    &lv_font_montserrat_16,
    ui_main_theme_color(theme.accent),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_date, LV_ALIGN_TOP_LEFT, 4, 29);

  label_tomorrow = ui_main_label(
    header,
    "DEMAIN",
    120,
    &lv_font_montserrat_16,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_tomorrow, LV_ALIGN_TOP_LEFT, 4, 54);

  // Faux contour noir 1 px : LVGL 8 ne sait pas tracer le contour d'un texte.
  // Les huit copies autour de chaque libellé restent masquées hors thème clair.
  static const int8_t outline_offsets[8][2] = {
    {-1, -1}, {0, -1}, {1, -1}, {-1, 0},
    {1, 0}, {-1, 1}, {0, 1}, {1, 1}
  };
  for (uint8_t i = 0; i < 8; ++i) {
    label_weekday_outline[i] = ui_main_label(
      header, "", 120, &lv_font_montserrat_16, lv_color_black(), LV_TEXT_ALIGN_LEFT
    );
    lv_obj_set_style_text_opa(label_weekday_outline[i], LV_OPA_60, LV_PART_MAIN);
    lv_obj_align(
      label_weekday_outline[i], LV_ALIGN_TOP_LEFT,
      4 + outline_offsets[i][0], 2 + outline_offsets[i][1]
    );
    lv_obj_add_flag(label_weekday_outline[i], LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_background(label_weekday_outline[i]);

    label_tomorrow_outline[i] = ui_main_label(
      header, "", 120, &lv_font_montserrat_16, lv_color_black(), LV_TEXT_ALIGN_LEFT
    );
    lv_obj_set_style_text_opa(label_tomorrow_outline[i], LV_OPA_60, LV_PART_MAIN);
    lv_obj_align(
      label_tomorrow_outline[i], LV_ALIGN_TOP_LEFT,
      4 + outline_offsets[i][0], 54 + outline_offsets[i][1]
    );
    lv_obj_add_flag(label_tomorrow_outline[i], LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_background(label_tomorrow_outline[i]);
  }

  lv_obj_t *label_deye_monitor = ui_main_label(
    header,
    "DEYE MONITOR",
    150,
    &lv_font_montserrat_16,
    ui_main_theme_color(theme.accent),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_deye_monitor, LV_ALIGN_TOP_MID, 0, 3);

  // BOUTON CFG
  lv_obj_t *settings_btn = lv_btn_create(header);
  lv_obj_set_size(settings_btn, 44, 30);
  lv_obj_align(settings_btn, LV_ALIGN_TOP_RIGHT, -4, 35);
  lv_obj_set_style_bg_color(settings_btn, ui_main_theme_color(theme.accent), LV_PART_MAIN);
  lv_obj_set_style_radius(settings_btn, 6, LV_PART_MAIN);
  // Les enfants sont positionnés dans toute la surface du bouton, sans marge
  // interne LVGL : l'icône reste ainsi exactement centrée.
  lv_obj_set_style_pad_all(settings_btn, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(settings_btn, ui_show_settings, LV_EVENT_CLICKED, nullptr);
  // Trois curseurs verticaux, centrés dans le bouton de configuration.
  // Les rails et les boutons sont volontairement larges pour rester lisibles
  // sur l'écran 480 x 320.
  const lv_coord_t slider_x[] = {11, 21, 31};
  const lv_coord_t knob_y[] = {8, 15, 11};
  for (uint8_t i = 0; i < 3; ++i) {
    lv_obj_t *slider = lv_obj_create(settings_btn);
    lv_obj_set_size(slider, 2, 18);
    lv_obj_set_pos(slider, slider_x[i], 6);
    lv_obj_set_style_bg_color(slider, ui_main_theme_color(theme.accent_text), LV_PART_MAIN);
    lv_obj_set_style_border_opa(slider, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *knob = lv_obj_create(settings_btn);
    lv_obj_set_size(knob, 6, 6);
    lv_obj_set_pos(knob, slider_x[i] - 2, knob_y[i]);
    lv_obj_set_style_bg_color(knob, ui_main_theme_color(theme.accent_dark), LV_PART_MAIN);
    lv_obj_set_style_border_width(knob, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(knob, ui_main_theme_color(theme.accent_text), LV_PART_MAIN);
    lv_obj_set_style_radius(knob, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(knob, LV_OBJ_FLAG_CLICKABLE);
  }
  // LIGNE 2 : HEURE (centrée)
  label_time = ui_main_label(
    header,
    "--:--",
    160,
    &lv_font_montserrat_38,
    ui_main_theme_color(theme.highlight),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 4);

  // États de liaison : regroupés à droite, juste avant Paramètres.
  lv_obj_t *wifi_icon = ui_main_make_icon_container(header, 21, 21);
  lv_obj_align(wifi_icon, LV_ALIGN_TOP_RIGHT, -62, 11);
  wifi_arc_outer = ui_main_make_wifi_arc(wifi_icon, 21, 0);
  wifi_arc_middle = ui_main_make_wifi_arc(wifi_icon, 15, 3);
  wifi_arc_inner = ui_main_make_wifi_arc(wifi_icon, 9, 6);
  wifi_arc_bottom = ui_main_make_wifi_arc(wifi_icon, 5, 8);
  ui_main_set_wifi_icon_color(lv_palette_main(LV_PALETTE_RED));
  label_wifi = ui_main_label(
    header,
    "0%",
    40,
    &lv_font_montserrat_14,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_wifi, LV_ALIGN_TOP_RIGHT, -33, 8);

  inverter_icon = ui_main_make_icon_container(header, 24, 21);
  lv_obj_align(inverter_icon, LV_ALIGN_TOP_RIGHT, -4, 4);
  // Soleil rayonnant derrière un panneau incliné, sans image ni rotation.
  solar_sun = lv_obj_create(inverter_icon);
  lv_obj_set_size(solar_sun, 8, 8);
  lv_obj_set_pos(solar_sun, 4, 3);
  lv_obj_set_style_border_opa(solar_sun, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_radius(solar_sun, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_clear_flag(solar_sun, LV_OBJ_FLAG_CLICKABLE);

  const lv_coord_t solar_ray_x[] = {7, 12, 13, 12, 1, 0, 1};
  const lv_coord_t solar_ray_y[] = {0, 1, 4, 8, 8, 4, 1};
  const lv_coord_t solar_ray_w[] = {2, 3, 3, 2, 2, 3, 3};
  const lv_coord_t solar_ray_h[] = {2, 2, 2, 3, 3, 2, 2};
  for (uint8_t i = 0; i < 7; ++i) {
    solar_rays[i] = lv_obj_create(inverter_icon);
    lv_obj_set_size(solar_rays[i], solar_ray_w[i], solar_ray_h[i]);
    lv_obj_set_pos(solar_rays[i], solar_ray_x[i], solar_ray_y[i]);
    lv_obj_set_style_border_opa(solar_rays[i], LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(solar_rays[i], LV_OBJ_FLAG_CLICKABLE);
  }

  // Trois lignes régulières de quatre cellules pour un panneau bien lisible.
  for (uint8_t row = 0; row < 3; ++row) {
    for (uint8_t column = 0; column < 4; ++column) {
      const uint8_t index = row * 4 + column;
      solar_panel_cells[index] = lv_obj_create(inverter_icon);
      lv_obj_set_size(solar_panel_cells[index], 3, 3);
      lv_obj_set_pos(solar_panel_cells[index], 5 + column * 4, 10 + row * 4);
      lv_obj_set_style_border_opa(solar_panel_cells[index], LV_OPA_TRANSP, LV_PART_MAIN);
      lv_obj_set_style_radius(solar_panel_cells[index], 0, LV_PART_MAIN);
      lv_obj_clear_flag(solar_panel_cells[index], LV_OBJ_FLAG_CLICKABLE);
    }
  }
  ui_main_set_inverter_icon_color(lv_palette_main(LV_PALETTE_RED));
  // ======================================================
  // BLOC PRODUCTION PV
  // ======================================================
  lv_obj_t *pv_card = ui_main_make_card(screen_main, 10, 110, 225, 150);

  // TITRE PV
  lv_obj_t *pv_title = ui_main_label(
    pv_card,
    "PRODUCTION PV",
    205,
    &lv_font_montserrat_16,
    lv_color_hex(0x00FF88),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(pv_title, LV_ALIGN_TOP_MID, 0, 4);

  label_pv_total = ui_main_label(
    pv_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_total, LV_ALIGN_CENTER, 0, -12);

  label_pv_daily = ui_main_label(
    pv_card,
    "JOUR: 0.0 kWh",
    205,
    &lv_font_montserrat_14,
    ui_main_theme_color(theme.muted_text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_daily, LV_ALIGN_CENTER, 0, 20);

  label_pv_detail = ui_main_label(
    pv_card,
    "PV1 0W  PV2 0W  PV3 0W",
    205,
    &lv_font_montserrat_12,
    ui_main_theme_color(theme.detail_text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_pv_detail, LV_ALIGN_BOTTOM_MID, 0, -4);

  // ======================================================
  // BLOC BATTERIE
  // ======================================================
  lv_obj_t *bat_card = ui_main_make_card(screen_main, 245, 110, 225, 150);

  // TITRE BATTERIE
  lv_obj_t *bat_title = ui_main_label(
    bat_card,
    "BATTERIE",
    205,
    &lv_font_montserrat_16,
    ui_main_theme_color(theme.highlight),
    LV_TEXT_ALIGN_CENTER
  );
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
  lv_obj_set_style_arc_color(arc_soc, ui_main_theme_color(theme.arc_bg), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_soc, lv_color_hex(0x22C55E), LV_PART_INDICATOR);

  label_soc = ui_main_label(
    bat_card,
    "0%",
    90,
    &lv_font_montserrat_26,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_soc, LV_ALIGN_LEFT_MID, 10, 12);

  label_battery_voltage = ui_main_label(
    bat_card,
    "--.-- V",
    100,
    &lv_font_montserrat_20,
    ui_main_theme_color(theme.text),
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
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_battery_power, LV_ALIGN_TOP_RIGHT, -5, 101);

  // ======================================================
  // BLOC CONSOMMATION
  // ======================================================
  lv_obj_t *load_card = ui_main_make_card(screen_main, 10, 270, 225, 150);

  // TITRE CONSOMMATION
  lv_obj_t *load_title = ui_main_label(
    load_card,
    "CONSOMMATION",
    205,
    &lv_font_montserrat_16,
    lv_color_hex(0xFACC15),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(load_title, LV_ALIGN_TOP_MID, 0, 4);

  label_load = ui_main_label(
    load_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_load, LV_ALIGN_CENTER, 0, -12);

  label_load_daily = ui_main_label(
    load_card,
    "Jour: 0.0 kWh",
    205,
    &lv_font_montserrat_14,
    ui_main_theme_color(theme.muted_text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_load_daily, LV_ALIGN_CENTER, 0, 20);

  // Groupe VE centre dans la tuile : l'icone et la puissance partagent le
  // meme axe vertical, independamment de la hauteur de leur police.
  ev_charge_group = ui_main_make_icon_container(load_card, 130, 24);
  lv_obj_align(ev_charge_group, LV_ALIGN_BOTTOM_MID, 0, -5);

  // Petite voiture décorative sous la consommation journalière, dessinée
  // uniquement avec des primitives LVGL légères.
  car_icon = ui_main_make_icon_container(ev_charge_group, 32, 17);
  lv_obj_align(car_icon, LV_ALIGN_LEFT_MID, 0, -2);
  lv_obj_add_event_cb(car_icon, ui_show_ve_deye, LV_EVENT_CLICKED, nullptr);
  const lv_color_t car_color = lv_color_hex(0xFACC15);

  // Silhouette basse, habitacle vitré et phare : une forme plus moderne.
  lv_obj_t *car_body = lv_obj_create(car_icon);
  lv_obj_set_size(car_body, 30, 5);
  lv_obj_set_pos(car_body, 1, 10);
  lv_obj_set_style_bg_color(car_body, car_color, LV_PART_MAIN);
  lv_obj_set_style_border_opa(car_body, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_radius(car_body, 3, LV_PART_MAIN);
  lv_obj_clear_flag(car_body, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t *car_cabin = lv_obj_create(car_icon);
  lv_obj_set_size(car_cabin, 17, 6);
  lv_obj_set_pos(car_cabin, 8, 5);
  lv_obj_set_style_bg_color(car_cabin, car_color, LV_PART_MAIN);
  lv_obj_set_style_border_opa(car_cabin, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_radius(car_cabin, 3, LV_PART_MAIN);
  lv_obj_clear_flag(car_cabin, LV_OBJ_FLAG_CLICKABLE);

  const lv_coord_t window_x[] = {10, 18};
  const lv_coord_t window_w[] = {6, 5};
  for (uint8_t i = 0; i < 2; ++i) {
    lv_obj_t *window = lv_obj_create(car_icon);
    lv_obj_set_size(window, window_w[i], 3);
    lv_obj_set_pos(window, window_x[i], 7);
    lv_obj_set_style_bg_color(window, ui_card_color(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(window, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(window, 1, LV_PART_MAIN);
    lv_obj_clear_flag(window, LV_OBJ_FLAG_CLICKABLE);
  }

  lv_obj_t *car_headlight = lv_obj_create(car_icon);
  lv_obj_set_size(car_headlight, 3, 1);
  lv_obj_set_pos(car_headlight, 27, 11);
  lv_obj_set_style_bg_color(car_headlight, ui_main_theme_color(theme.text), LV_PART_MAIN);
  lv_obj_set_style_border_opa(car_headlight, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_radius(car_headlight, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_clear_flag(car_headlight, LV_OBJ_FLAG_CLICKABLE);

  const lv_coord_t wheel_x[] = {5, 21};
  for (uint8_t i = 0; i < 2; ++i) {
    lv_obj_t *wheel = lv_obj_create(car_icon);
    lv_obj_set_size(wheel, 6, 5);
    lv_obj_set_pos(wheel, wheel_x[i], 12);
    lv_obj_set_style_bg_color(wheel, ui_card_color(), LV_PART_MAIN);
    lv_obj_set_style_border_color(wheel, car_color, LV_PART_MAIN);
    lv_obj_set_style_border_width(wheel, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(wheel, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(wheel, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *hub = lv_obj_create(car_icon);
    lv_obj_set_size(hub, 2, 2);
    lv_obj_set_pos(hub, wheel_x[i] + 2, 14);
    lv_obj_set_style_bg_color(hub, car_color, LV_PART_MAIN);
    lv_obj_set_style_border_opa(hub, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_radius(hub, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(hub, LV_OBJ_FLAG_CLICKABLE);
  }
  label_ev_charge_power = ui_main_label(
    ev_charge_group, "VE: -- W", 86, &lv_font_montserrat_14,
    ui_main_theme_color(theme.muted_text), LV_TEXT_ALIGN_LEFT
  );
  lv_obj_align(label_ev_charge_power, LV_ALIGN_LEFT_MID, 40, 0);
  if (!cfg_ev_charger_enabled) lv_obj_add_flag(ev_charge_group, LV_OBJ_FLAG_HIDDEN);

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

  // Pastille Tempo compacte, en haut à droite de la tuile Réseau.
  tempo_card = ui_main_make_card(grid_card, 165, 4, 44, 27);
  lv_obj_set_style_pad_all(tempo_card, 3, LV_PART_MAIN);
  label_tempo = ui_main_label(
    tempo_card,
    "--",
    24,
    &lv_font_montserrat_14,
    ui_main_theme_color(theme.muted_text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_tempo, LV_ALIGN_CENTER, 0, 0);
  if (!cfg_tempo_enabled) lv_obj_add_flag(tempo_card, LV_OBJ_FLAG_HIDDEN);

  label_grid_power = ui_main_label(
    grid_card,
    "0 W",
    205,
    &lv_font_montserrat_32,
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_CENTER
  );
  lv_obj_align(label_grid_power, LV_ALIGN_CENTER, 0, -12);

  label_grid_daily = ui_main_label(
    grid_card,
    "D.Sell: 0.0kWh  D.Buy: 0.0kWh",
    205,
    &lv_font_montserrat_12,
    ui_main_theme_color(theme.muted_text),
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
    ui_main_theme_color(theme.text),
    LV_TEXT_ALIGN_RIGHT
  );
  lv_obj_align(label_temp, LV_ALIGN_RIGHT_MID, -3, 0);

  // Avertissement statique, sans transformation graphique ni bitmap.
  label_monitoring_unavailable = lv_label_create(screen_main);
  lv_label_set_text(label_monitoring_unavailable, "MONITORING INDISPONIBLE");
  lv_obj_set_size(label_monitoring_unavailable, 430, 38);
  lv_obj_set_style_text_font(label_monitoring_unavailable, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(label_monitoring_unavailable, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_text_opa(label_monitoring_unavailable, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_text_align(label_monitoring_unavailable, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(label_monitoring_unavailable, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(label_monitoring_unavailable, LV_OPA_90, LV_PART_MAIN);
  lv_obj_set_style_radius(label_monitoring_unavailable, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_top(label_monitoring_unavailable, 7, LV_PART_MAIN);
  // Les tuiles du haut finissent à y=260 et celles du bas commencent à y=270.
  // Le bandeau (38 px) est donc centré sur leur séparation, à y=265.
  lv_obj_set_pos(label_monitoring_unavailable, 25, 246);
  lv_obj_clear_flag(label_monitoring_unavailable, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(label_monitoring_unavailable, LV_OBJ_FLAG_HIDDEN);
}

// ==================== MISE À JOUR DE L'INTERFACE ====================
static void ui_main_update() {
  if (screen_main == nullptr) return;
  const UiThemePalette &theme = ui_theme_palette(cfg_ui_theme);

  tempo_api_process(cfg_tempo_enabled);

  static DashboardData data = {};
  static uint16_t pv_daily = 0;
  static bool pv_daily_valid = false;
  static uint16_t daily_load = 0;
  static uint16_t daily_buy = 0;
  static uint16_t daily_sell = 0;
  static bool on_grid = true;
  deye_copy_snapshot(
    &data, &pv_daily, &pv_daily_valid, &daily_load, &daily_buy, &daily_sell, &on_grid
  );
  const TempoNow tempo = tempo_api_get_now();

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
    static const char *const weekdays[] = {
      "DIMANCHE", "LUNDI", "MARDI", "MERCREDI", "JEUDI", "VENDREDI", "SAMEDI"
    };
    if (cfg_tempo_enabled && cfg_tempo_colorblind_mode) {
      // Le libellé inclut alors la couleur, tout en gardant la même taille
      // de police que dans le mode normal.
      lv_obj_set_width(label_weekday, 160);
      lv_obj_set_style_text_font(label_weekday, &lv_font_montserrat_16, LV_PART_MAIN);
      snprintf(
        text,
        sizeof(text),
        "%s : %s",
        weekdays[timeinfo.tm_wday],
        tempo.valid ? ui_tempo_color_name(tempo.color_code) : "--"
      );
      lv_label_set_text(label_weekday, text);
    } else {
      lv_obj_set_width(label_weekday, 120);
      lv_obj_set_style_text_font(label_weekday, &lv_font_montserrat_16, LV_PART_MAIN);
      lv_label_set_text(label_weekday, weekdays[timeinfo.tm_wday]);
    }
  }

  // Dans le thème clair, le contour noir fait ressortir les lettres blanches
  // sans le cartouche rectangulaire qui occupait tout le bandeau.
  const bool use_weekday_text_outline =
    cfg_ui_theme == UI_THEME_LIGHT && cfg_tempo_enabled && tempo.valid && tempo.color_code == 2;
  if (use_weekday_text_outline) {
    lv_obj_set_style_text_color(label_weekday, lv_color_white(), LV_PART_MAIN);
    for (uint8_t i = 0; i < 8; ++i) {
      lv_label_set_text(label_weekday_outline[i], lv_label_get_text(label_weekday));
      lv_obj_set_width(label_weekday_outline[i], cfg_tempo_colorblind_mode ? 160 : 120);
      lv_obj_clear_flag(label_weekday_outline[i], LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    for (uint8_t i = 0; i < 8; ++i) {
      lv_obj_add_flag(label_weekday_outline[i], LV_OBJ_FLAG_HIDDEN);
    }

    lv_color_t weekday_color = ui_main_theme_color(theme.text);
    if (cfg_tempo_enabled && tempo.valid) {
      weekday_color = ui_main_theme_color(theme.muted_text);
      if (tempo.color_code == 1) weekday_color = ui_tempo_blue_color();
      else if (tempo.color_code == 2) weekday_color = ui_tempo_white_color();
      else if (tempo.color_code == 3) weekday_color = ui_tempo_red_color();
    }
    lv_obj_set_style_text_color(label_weekday, weekday_color, LV_PART_MAIN);

  }
  if (cfg_tempo_enabled && cfg_tempo_colorblind_mode) {
    snprintf(
      text,
      sizeof(text),
      "DEMAIN : %s",
      ui_tempo_color_name(tempo.tomorrow_color_code)
    );
    lv_label_set_text(label_tomorrow, text);
  } else {
    lv_label_set_text(label_tomorrow, "DEMAIN");
  }
  if (cfg_tempo_enabled) {
    lv_obj_clear_flag(label_tomorrow, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(label_tomorrow, LV_OBJ_FLAG_HIDDEN);
  }
  const bool use_tomorrow_text_outline =
    cfg_ui_theme == UI_THEME_LIGHT && cfg_tempo_enabled && tempo.tomorrow_color_code == 2;
  if (use_tomorrow_text_outline) {
    lv_obj_set_style_text_color(label_tomorrow, lv_color_white(), LV_PART_MAIN);
    for (uint8_t i = 0; i < 8; ++i) {
      lv_label_set_text(label_tomorrow_outline[i], lv_label_get_text(label_tomorrow));
      lv_obj_clear_flag(label_tomorrow_outline[i], LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    for (uint8_t i = 0; i < 8; ++i) {
      lv_obj_add_flag(label_tomorrow_outline[i], LV_OBJ_FLAG_HIDDEN);
    }

    // Gris tant que demain n'est pas annoncé ; bleu ou rouge une fois connu.
    lv_color_t tomorrow_color = ui_main_theme_color(theme.text);
    if (cfg_tempo_enabled) {
      tomorrow_color = ui_main_theme_color(theme.muted_text);
      if (tempo.tomorrow_color_code == 1) tomorrow_color = ui_tempo_blue_color();
      else if (tempo.tomorrow_color_code == 2) tomorrow_color = ui_tempo_white_color();
      else if (tempo.tomorrow_color_code == 3) tomorrow_color = ui_tempo_red_color();
    }
    lv_obj_set_style_text_color(label_tomorrow, tomorrow_color, LV_PART_MAIN);
  }
  // WIFI
  if (WiFi.status() == WL_CONNECTED) {
    const int quality = wifi_quality_percent();
    const lv_color_t color = quality <= 20
      ? lv_palette_main(LV_PALETTE_RED)
      : lv_palette_main(LV_PALETTE_GREEN);
    snprintf(text, sizeof(text), "%d%%", quality);
    ui_main_set_wifi_icon_color(color);
    ui_main_set_wifi_icon_opa(LV_OPA_COVER);
    lv_obj_set_style_text_color(label_wifi, color, LV_PART_MAIN);
    lv_obj_set_style_text_opa(label_wifi, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(inverter_icon, LV_OBJ_FLAG_HIDDEN);
  } else {
    const bool blink_on = (millis() / 500UL) % 2 == 0;
    lv_label_set_text(label_wifi, "0%");
    lv_label_set_text(label_monitoring_unavailable, "PAS CONNECTE AU WI-FI");
    ui_main_set_wifi_icon_color(lv_palette_main(LV_PALETTE_RED));
    ui_main_set_wifi_icon_opa(blink_on ? LV_OPA_COVER : LV_OPA_TRANSP);
    lv_obj_set_style_text_color(label_wifi, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_set_style_text_opa(label_wifi, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_add_flag(inverter_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(label_monitoring_unavailable, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(label_monitoring_unavailable);
  }
  if (WiFi.status() == WL_CONNECTED) lv_label_set_text(label_wifi, text);

  if (WiFi.status() == WL_CONNECTED && !data.valid) {
    lv_label_set_text(label_monitoring_unavailable, "DEYE INJOIGNABLE");
    lv_obj_clear_flag(label_monitoring_unavailable, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(label_monitoring_unavailable);
  } else if (WiFi.status() == WL_CONNECTED) {
    lv_obj_add_flag(label_monitoring_unavailable, LV_OBJ_FLAG_HIDDEN);
  }

  // DEYE
  ui_main_set_inverter_icon_color(
    data.valid
      ? lv_palette_main(LV_PALETTE_GREEN)
      : lv_palette_main(LV_PALETTE_RED)
  );

  // PRODUCTION PV
  uint32_t pv_total = data.pv1_w + data.pv2_w + data.pv3_w;

  snprintf(text, sizeof(text), "%lu W", (unsigned long)pv_total);
  lv_label_set_text(label_pv_total, text);

  snprintf(
    text,
    sizeof(text),
    "PV1 %uW  PV2 %uW  PV3 %uW",
    data.pv1_w,
    data.pv2_w,
    data.pv3_w
  );
  lv_label_set_text(label_pv_detail, text);

  if (pv_daily_valid) {
    snprintf(text, sizeof(text), "JOUR: %.1f kWh", pv_daily * 0.1f);
  } else {
    snprintf(text, sizeof(text), "JOUR: --.- kWh");
  }
  lv_label_set_text(label_pv_daily, text);

  // BATTERIE
  uint16_t soc = constrain(data.battery_soc, 0, 100);
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

  snprintf(text, sizeof(text), "%.2f V", data.battery_voltage);
  lv_label_set_text(label_battery_voltage, text);

  const char *battery_state = "REPOS";
  lv_color_t battery_color = lv_color_hex(0xFACC15);

  if (data.battery_power < -5) {
    battery_state = "CHARGE";
    battery_color = lv_color_hex(0x22C55E);
  } else if (data.battery_power > 5) {
    battery_state = "DECHARGE";
    battery_color = lv_color_hex(0xFB7185);
  }

  lv_label_set_text(label_battery_state, battery_state);
  lv_obj_set_style_text_color(label_battery_state, battery_color, LV_PART_MAIN);

  snprintf(text, sizeof(text), "%d W", data.battery_power);
  lv_label_set_text(label_battery_power, text);

  // CONSOMMATION
  snprintf(text, sizeof(text), "%d W", data.load_power);
  lv_label_set_text(label_load, text);

  snprintf(text, sizeof(text), "Jour: %.1f kWh", daily_load * 0.1f);
  lv_label_set_text(label_load_daily, text);
  if (cfg_ev_charger_enabled) {
    lv_obj_clear_flag(ev_charge_group, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ev_charge_group, LV_OBJ_FLAG_HIDDEN);
  }

  if (cfg_ev_charger_enabled) {
    EvDeyeData ev = {};
    if (deye_copy_ev_snapshot(&ev) && ev.valid) {
      snprintf(text, sizeof(text), "VE: %u W", ev.charge_power_w);
    } else {
      snprintf(text, sizeof(text), "VE: -- W");
    }
    lv_label_set_text(label_ev_charge_power, text);
  }

  // L'endpoint /api/now tient compte de la bascule Tempo à 6 h.
  if (!cfg_tempo_enabled) {
    lv_obj_add_flag(tempo_card, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(tempo_card, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(tempo_card, 44, 27);
    lv_obj_set_pos(tempo_card, 165, 4);
    lv_obj_set_width(label_tempo, 24);

    if (!tempo.valid) {
      lv_label_set_text(label_tempo, "--");
      lv_obj_set_style_bg_color(tempo_card, ui_card_color(), LV_PART_MAIN);
      lv_obj_set_style_border_color(tempo_card, ui_border_color(), LV_PART_MAIN);
      lv_obj_set_style_text_color(label_tempo, ui_main_theme_color(theme.muted_text), LV_PART_MAIN);
    } else {
      lv_color_t tile_color = ui_tempo_blue_color();
      lv_color_t tile_border_color = tile_color;
      lv_color_t text_color = lv_color_white();
      if (tempo.color_code == 2) {
        tile_color = ui_tempo_white_color();
        if (cfg_ui_theme == UI_THEME_LIGHT) tile_border_color = lv_color_black();
        text_color = lv_color_hex(0x111827);
      } else if (tempo.color_code == 3) {
        tile_color = ui_tempo_red_color();
      }

      lv_label_set_text(label_tempo, tempo.hour_code == 1 ? "HP" : "HC");
      lv_obj_set_style_bg_color(tempo_card, tile_color, LV_PART_MAIN);
      lv_obj_set_style_border_color(tempo_card, tile_border_color, LV_PART_MAIN);
      lv_obj_set_style_text_color(label_tempo, text_color, LV_PART_MAIN);
    }
  }

  // RESEAU
  int16_t grid_power = data.grid_power;
  
  char sign = (grid_power < 0) ? '-' : ' ';
  int32_t grid_abs = abs(grid_power);
  
  snprintf(text, sizeof(text), "%c%ld W", sign, (long)grid_abs);
  lv_label_set_text(label_grid_power, text);

  snprintf(text, sizeof(text), "D.Sell: %.1fkWh  D.Buy: %.1fkWh", daily_sell * 0.1f, daily_buy * 0.1f);
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
    lv_obj_set_style_text_color(label_grid_power, ui_main_theme_color(theme.text), LV_PART_MAIN);
  }

  // SMARTLOAD ET TEMPÉRATURES
  // SMARTLOAD / GEN MO
  bool gen_smartload = settings_get_gen_mode();
  char smart_text[32];
  if (gen_smartload) {
    // Mode SmartLoad : afficher ON/OFF
    snprintf(smart_text, sizeof(smart_text), "SMARTLOAD : %s", 
             data.smartload_on ? "ON" : "OFF");
    lv_obj_set_style_text_color(label_smartload, 
                                data.smartload_on ? lv_color_hex(0x22C55E) : lv_color_hex(0xFB7185),
                                LV_PART_MAIN);
  } else {
    // Mode GEN MO : afficher la puissance
    snprintf(smart_text, sizeof(smart_text), "GEN : %d W", data.ups_power);
    lv_obj_set_style_text_color(label_smartload, ui_main_theme_color(theme.accent), LV_PART_MAIN);
  }
  lv_label_set_text(label_smartload, smart_text);

  snprintf(
    text,
    sizeof(text),
    "DC %.1fC  AC %.1fC  BAT %.1fC",
    data.dc_temperature,
    data.ac_temperature,
    data.battery_temperature
  );
  lv_label_set_text(label_temp, text);
}
