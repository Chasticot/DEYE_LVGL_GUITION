// ui_ve_deye.h - Ecran de supervision du chargeur VE DEYE

#pragma once

#include <Arduino.h>
#include <lvgl.h>

#include "config.h"
#include "app_data.h"
#include "settings.h"
#include "ui_theme.h"
#include "ve_deye.h"

extern void deye_solarman_set_ui_active(bool active);
extern bool deye_copy_ev_snapshot(EvDeyeData *out);
extern bool deye_copy_snapshot(
  DashboardData *out,
  uint16_t *pv_daily,
  bool *pv_daily_valid_out,
  uint16_t *daily_load_out,
  uint16_t *daily_buy_out,
  uint16_t *daily_sell_out,
  bool *on_grid_out
);
extern void ui_show_dashboard(lv_event_t *e);

static lv_obj_t *screen_ve_deye = nullptr;
static lv_obj_t *label_ve_charge_power = nullptr;
static lv_obj_t *label_ve_max_power = nullptr;
static lv_obj_t *label_ve_grid_power = nullptr;
static lv_obj_t *label_ve_load_power = nullptr;
static lv_obj_t *label_ve_connection = nullptr;
static lv_obj_t *label_ve_charge_limit = nullptr;
static lv_obj_t *slider_ve_max_power = nullptr;
static lv_obj_t *textarea_ve_max_power = nullptr;
static lv_obj_t *keyboard_ve_deye = nullptr;
static bool ve_text_is_being_edited = false;
static uint16_t ve_last_max_power_raw = UINT16_MAX;
static bool ve_max_power_pending = false;
static uint16_t ve_pending_max_power_raw = 0;
static bool ve_slider_is_syncing = false;
static uint32_t ve_charge_power_limit_w = DEYE_EV_SINGLE_PHASE_MAX_POWER_W;

static void ui_ve_deye_create();

static const UiThemePalette &ui_ve_deye_theme() {
  return ui_theme_palette(cfg_ui_theme);
}

static lv_color_t ui_ve_deye_color(uint32_t color) {
  return lv_color_hex(color);
}

static lv_obj_t *ui_ve_deye_label(
  lv_obj_t *parent,
  const char *text,
  lv_coord_t x,
  lv_coord_t y,
  lv_coord_t width,
  const lv_font_t *font,
  uint32_t color,
  lv_text_align_t alignment = LV_TEXT_ALIGN_LEFT
) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_width(label, width);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, ui_ve_deye_color(color), LV_PART_MAIN);
  lv_obj_set_style_text_align(label, alignment, LV_PART_MAIN);
  return label;
}

static void ui_ve_deye_set_max_power(uint16_t raw_value, bool update_textarea) {
  if (slider_ve_max_power == nullptr) return;

  const uint16_t maximum_raw = static_cast<uint16_t>(
    ve_charge_power_limit_w / DEYE_EV_POWER_REGISTER_SCALE_W
  );
  if (raw_value > maximum_raw) raw_value = maximum_raw;
  const uint32_t watts = deye_ev_max_power_w(raw_value);
  ve_slider_is_syncing = true;
  lv_slider_set_value(slider_ve_max_power, watts, LV_ANIM_OFF);
  ve_slider_is_syncing = false;
  if (update_textarea && textarea_ve_max_power != nullptr) {
    char text[12];
    snprintf(text, sizeof(text), "%lu", (unsigned long)watts);
    lv_textarea_set_text(textarea_ve_max_power, text);
  }
}

// Le registre 260 donne le plafond accepte par le chargeur. Les chargeurs
// SUN-EVSE 22K sont limites physiquement a 7,4 kW en mono et 22 kW en tri.
static void ui_ve_deye_set_charge_limit(uint16_t max_power_raw) {
  uint32_t limit_w = deye_ev_max_power_w(max_power_raw);
  if (limit_w > DEYE_EV_THREE_PHASE_MAX_POWER_W) {
    limit_w = DEYE_EV_THREE_PHASE_MAX_POWER_W;
  }
  if (limit_w == ve_charge_power_limit_w) return;

  ve_charge_power_limit_w = limit_w;
  if (slider_ve_max_power != nullptr) {
    lv_slider_set_range(slider_ve_max_power, 0, ve_charge_power_limit_w);
    const uint32_t current_w = lv_slider_get_value(slider_ve_max_power);
    if (current_w > ve_charge_power_limit_w) {
      ui_ve_deye_set_max_power(
        static_cast<uint16_t>(ve_charge_power_limit_w / DEYE_EV_POWER_REGISTER_SCALE_W), true
      );
    }
  }
  if (label_ve_charge_limit != nullptr) {
    char text[64];
    snprintf(text, sizeof(text), "Limite chargeur : 0 - %lu W", (unsigned long)ve_charge_power_limit_w);
    lv_label_set_text(label_ve_charge_limit, text);
  }
}

static void ui_ve_deye_slider_changed(lv_event_t *e) {
  (void)e;
  if (ve_slider_is_syncing || slider_ve_max_power == nullptr || textarea_ve_max_power == nullptr) return;

  const uint32_t slider_watts = lv_slider_get_value(slider_ve_max_power);
  uint16_t raw_value = static_cast<uint16_t>(
    (slider_watts + DEYE_EV_POWER_REGISTER_SCALE_W / 2) / DEYE_EV_POWER_REGISTER_SCALE_W
  );
  const uint16_t maximum_raw = static_cast<uint16_t>(
    ve_charge_power_limit_w / DEYE_EV_POWER_REGISTER_SCALE_W
  );
  if (raw_value > maximum_raw) raw_value = maximum_raw;
  ve_pending_max_power_raw = raw_value;
  ve_max_power_pending = true;
  ui_ve_deye_set_max_power(raw_value, true);
}

static void ui_ve_deye_textarea_event(lv_event_t *e) {
  const lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *textarea = lv_event_get_target(e);

  if (code == LV_EVENT_FOCUSED) {
    ve_text_is_being_edited = true;
    lv_keyboard_set_textarea(keyboard_ve_deye, textarea);
    lv_obj_clear_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(keyboard_ve_deye);
    return;
  }

  if (code != LV_EVENT_READY && code != LV_EVENT_DEFOCUSED && code != LV_EVENT_CANCEL) return;

  const char *text = lv_textarea_get_text(textarea);
  char *end = nullptr;
  const unsigned long watts = strtoul(text, &end, 10);
  const unsigned long max_watts = ve_charge_power_limit_w;
  const bool is_valid = end != text && *end == '\0' && watts <= max_watts && (watts % DEYE_EV_POWER_REGISTER_SCALE_W) == 0;

  if (is_valid) {
    ve_pending_max_power_raw = static_cast<uint16_t>(watts / DEYE_EV_POWER_REGISTER_SCALE_W);
    ve_max_power_pending = true;
    ui_ve_deye_set_max_power(ve_pending_max_power_raw, true);
  } else {
    // Le registre est exprime par pas de 10 W : restaure la derniere valeur
    // coherente sans envoyer de commande a l'onduleur.
    ui_ve_deye_set_max_power(ve_last_max_power_raw == UINT16_MAX ? 0 : ve_last_max_power_raw, true);
    ve_max_power_pending = false;
  }

  ve_text_is_being_edited = false;
  lv_keyboard_set_textarea(keyboard_ve_deye, nullptr);
  lv_obj_add_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
}

void ui_show_ve_deye(lv_event_t *e) {
  (void)e;
  if (!cfg_ev_charger_enabled) return;
  if (screen_ve_deye == nullptr) ui_ve_deye_create();
  if (screen_ve_deye == nullptr) return;

  // Cet ecran est une supervision : la lecture Modbus reste active afin que
  // les valeurs VE et le tableau de bord restent a jour.
  deye_solarman_set_ui_active(false);
  lv_scr_load(screen_ve_deye);
}

static void ui_ve_deye_return(lv_event_t *e) {
  (void)e;
  ui_show_dashboard(nullptr);
}

static void ui_ve_deye_update() {
  if (screen_ve_deye == nullptr || !cfg_ev_charger_enabled) return;

  EvDeyeData ev = {};
  if (!deye_copy_ev_snapshot(&ev)) return;

  char text[48];

  DashboardData main = {};
  uint16_t unused_pv_daily = 0;
  uint16_t unused_daily_load = 0;
  uint16_t unused_daily_buy = 0;
  uint16_t unused_daily_sell = 0;
  bool unused_pv_daily_valid = false;
  bool unused_on_grid = false;
  if (deye_copy_snapshot(
        &main, &unused_pv_daily, &unused_pv_daily_valid, &unused_daily_load,
        &unused_daily_buy, &unused_daily_sell, &unused_on_grid)) {
    snprintf(text, sizeof(text), "%d W", main.grid_power);
    lv_label_set_text(label_ve_grid_power, text);
    snprintf(text, sizeof(text), "%d W", main.load_power);
    lv_label_set_text(label_ve_load_power, text);
  }

  if (!ev.valid) {
    lv_label_set_text(label_ve_charge_power, "-- W");
    lv_label_set_text(label_ve_max_power, "-- W");
    lv_label_set_text(label_ve_connection, "Etat connexion VE : indisponible");
    return;
  }

  snprintf(text, sizeof(text), "%u W", ev.charge_power_w);
  lv_label_set_text(label_ve_charge_power, text);
  snprintf(text, sizeof(text), "%lu W", (unsigned long)deye_ev_max_power_w(ev.max_charge_power_raw));
  lv_label_set_text(label_ve_max_power, text);
  snprintf(text, sizeof(text), "Etat connexion VE : 0x%04X", ev.connection_state_raw);
  lv_label_set_text(label_ve_connection, text);

  ui_ve_deye_set_charge_limit(ev.max_charge_power_raw);

  if (ve_max_power_pending && ev.max_charge_power_raw == ve_pending_max_power_raw) {
    ve_max_power_pending = false;
  }
  if (!ve_text_is_being_edited && !ve_max_power_pending && ev.max_charge_power_raw != ve_last_max_power_raw) {
    ve_last_max_power_raw = ev.max_charge_power_raw;
    ui_ve_deye_set_max_power(ev.max_charge_power_raw, true);
  }
}

static void ui_ve_deye_create() {
  if (screen_ve_deye != nullptr) return;

  const UiThemePalette &theme = ui_ve_deye_theme();
  screen_ve_deye = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_ve_deye, ui_ve_deye_color(theme.screen_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_ve_deye, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_ve_deye, LV_OBJ_FLAG_SCROLLABLE);

  ui_ve_deye_label(screen_ve_deye, "CHARGEUR VE DEYE", 0, 12, LCD_W, &lv_font_montserrat_20, theme.accent, LV_TEXT_ALIGN_CENTER);

  ui_ve_deye_label(screen_ve_deye, "PUISSANCE DE CHARGE", 20, 62, 210, &lv_font_montserrat_14, theme.muted_text);
  label_ve_charge_power = ui_ve_deye_label(screen_ve_deye, "-- W", 20, 84, 210, &lv_font_montserrat_20, theme.text);

  ui_ve_deye_label(screen_ve_deye, "PUISSANCE MAX.", 250, 62, 210, &lv_font_montserrat_14, theme.muted_text);
  label_ve_max_power = ui_ve_deye_label(screen_ve_deye, "-- W", 250, 84, 210, &lv_font_montserrat_20, theme.text);

  ui_ve_deye_label(screen_ve_deye, "CONSO RESEAU", 20, 135, 210, &lv_font_montserrat_14, theme.muted_text);
  label_ve_grid_power = ui_ve_deye_label(screen_ve_deye, "-- W", 20, 157, 210, &lv_font_montserrat_20, theme.text);

  ui_ve_deye_label(screen_ve_deye, "CONSOMMATION", 250, 135, 210, &lv_font_montserrat_14, theme.muted_text);
  label_ve_load_power = ui_ve_deye_label(screen_ve_deye, "-- W", 250, 157, 210, &lv_font_montserrat_20, theme.text);
  label_ve_connection = ui_ve_deye_label(
    screen_ve_deye, "Etat connexion VE : indisponible", 20, 206, 440,
    &lv_font_montserrat_16, theme.text
  );

  ui_ve_deye_label(screen_ve_deye, "Puissance max. de charge (W)", 20, 247, 440, &lv_font_montserrat_16, theme.accent);
  slider_ve_max_power = lv_slider_create(screen_ve_deye);
  lv_obj_set_pos(slider_ve_max_power, 20, 281);
  lv_obj_set_size(slider_ve_max_power, 290, 18);
  lv_slider_set_range(slider_ve_max_power, 0, ve_charge_power_limit_w);
  lv_obj_set_style_bg_color(slider_ve_max_power, ui_ve_deye_color(theme.control_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_ve_max_power, ui_ve_deye_color(theme.accent), LV_PART_INDICATOR);
  lv_obj_add_event_cb(slider_ve_max_power, ui_ve_deye_slider_changed, LV_EVENT_VALUE_CHANGED, nullptr);

  textarea_ve_max_power = lv_textarea_create(screen_ve_deye);
  lv_textarea_set_one_line(textarea_ve_max_power, true);
  lv_textarea_set_accepted_chars(textarea_ve_max_power, "0123456789");
  lv_textarea_set_max_length(textarea_ve_max_power, 6);
  lv_textarea_set_text(textarea_ve_max_power, "0");
  lv_obj_set_pos(textarea_ve_max_power, 325, 268);
  lv_obj_set_size(textarea_ve_max_power, 135, 43);
  lv_obj_set_style_bg_color(textarea_ve_max_power, ui_ve_deye_color(theme.control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(textarea_ve_max_power, ui_ve_deye_color(theme.text), LV_PART_MAIN);
  lv_obj_set_style_border_color(textarea_ve_max_power, ui_ve_deye_color(theme.control_border), LV_PART_MAIN);
  lv_obj_set_style_border_width(textarea_ve_max_power, 1, LV_PART_MAIN);

  keyboard_ve_deye = lv_keyboard_create(screen_ve_deye);
  lv_obj_set_size(keyboard_ve_deye, LCD_W, 160);
  lv_obj_align(keyboard_ve_deye, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(keyboard_ve_deye, ui_ve_deye_color(theme.control_bg), LV_PART_MAIN);
  lv_obj_add_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(textarea_ve_max_power, ui_ve_deye_textarea_event, LV_EVENT_ALL, nullptr);

  label_ve_charge_limit = ui_ve_deye_label(
    screen_ve_deye,
    "Limite chargeur : 0 - 7400 W",
    20, 322, 440, &lv_font_montserrat_12, theme.muted_text, LV_TEXT_ALIGN_CENTER
  );

  lv_obj_t *return_button = lv_btn_create(screen_ve_deye);
  lv_obj_set_pos(return_button, 145, 365);
  lv_obj_set_size(return_button, 190, 54);
  lv_obj_set_style_bg_color(return_button, ui_ve_deye_color(theme.accent), LV_PART_MAIN);
  lv_obj_set_style_radius(return_button, 8, LV_PART_MAIN);
  lv_obj_add_event_cb(return_button, ui_ve_deye_return, LV_EVENT_CLICKED, nullptr);
  lv_obj_t *return_label = ui_ve_deye_label(
    return_button, "RETOUR", 0, 0, 190, &lv_font_montserrat_16,
    theme.accent_text, LV_TEXT_ALIGN_CENTER
  );
  lv_obj_center(return_label);
}
