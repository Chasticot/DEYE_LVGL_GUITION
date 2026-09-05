// Supervision et reglages de l'onduleur, utilisables avant installation de la borne.
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
extern bool deye_submit_ev_command(EvDeyeCommand command);
extern void ui_show_dashboard(lv_event_t *e);

static lv_obj_t *screen_ve_deye = nullptr;
static lv_obj_t *label_ve_requested = nullptr;
static lv_obj_t *label_ve_max_power = nullptr;
static lv_obj_t *label_ve_grid_power = nullptr;
static lv_obj_t *label_ve_load_power = nullptr;
static lv_obj_t *label_ve_diagnostic = nullptr;
static lv_obj_t *label_ve_mode_read = nullptr;
static lv_obj_t *label_ve_result = nullptr;
static lv_obj_t *slider_ve_max_power = nullptr;
static lv_obj_t *textarea_ve_max_power = nullptr;
static lv_obj_t *dropdown_ve_mode = nullptr;
static lv_obj_t *button_ve_apply = nullptr;
static lv_obj_t *keyboard_ve_deye = nullptr;
static bool ve_syncing = false;
static bool ve_editing = false;
static bool ve_power_dirty = false;
static bool ve_mode_dirty = false;
static bool ve_submitted = false;
static uint32_t ve_draft_w = 1400;
static uint8_t ve_draft_mode = 0;
static bool ve_local_error = false;
static bool ve_show_command_result = false;

static const UiThemePalette &ui_ve_deye_theme() { return ui_theme_palette(cfg_ui_theme); }
static lv_obj_t *ui_ve_deye_label(const char *text, int x, int y, int width, const lv_font_t *font, uint32_t color) {
  lv_obj_t *label = lv_label_create(screen_ve_deye);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_width(label, width);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
  return label;
}
static void ui_ve_error(const char *text) {
  ve_local_error = true;
  lv_label_set_text(label_ve_result, text);
}
static void ui_ve_sync_power(uint32_t watts) {
  ve_syncing = true;
  lv_slider_set_value(slider_ve_max_power, watts, LV_ANIM_OFF);
  char text[12];
  snprintf(text, sizeof(text), "%lu", (unsigned long)watts);
  lv_textarea_set_text(textarea_ve_max_power, text);
  ve_syncing = false;
}
static bool ui_ve_parse_power(uint32_t *out) {
  const char *text = lv_textarea_get_text(textarea_ve_max_power);
  if (!text || !*text) return false;
  for (const char *p = text; *p; ++p) if (*p < '0' || *p > '9') return false;
  const unsigned long watts = strtoul(text, nullptr, 10);
  if (watts < 1400 || watts > DEYE_EV_INSTALLATION_MAX_POWER_W ||
      watts % DEYE_EV_POWER_REGISTER_SCALE_W) return false;
  *out = watts;
  return true;
}
static void ui_ve_slider_changed(lv_event_t *e) {
  (void)e;
  if (ve_syncing) return;
  // Pas ergonomique de 10 W, distinct de l'unite Modbus.
  ve_draft_w = ((lv_slider_get_value(slider_ve_max_power) + 5) / 10) * 10;
  ve_power_dirty = true;
  ve_local_error = false;
  ve_show_command_result = false;
  ui_ve_sync_power(ve_draft_w);
}
static void ui_ve_text_event(lv_event_t *e) {
  const lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_FOCUSED) {
    ve_editing = true;
    lv_keyboard_set_textarea(keyboard_ve_deye, textarea_ve_max_power);
    lv_obj_clear_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(keyboard_ve_deye);
  } else if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED || code == LV_EVENT_CANCEL) {
    if (!ve_editing) return;
    if (code == LV_EVENT_CANCEL) {
      ui_ve_sync_power(ve_draft_w);
    } else {
      uint32_t watts = 0;
      if (ui_ve_parse_power(&watts)) {
        ve_power_dirty = ve_power_dirty || watts != ve_draft_w;
        ve_draft_w = watts;
        ve_local_error = false;
        ve_show_command_result = false;
        ui_ve_sync_power(watts);
      } else ui_ve_error("Puissance hors plage. Corriger la saisie.");
    }
    ve_editing = false;
    lv_keyboard_set_textarea(keyboard_ve_deye, nullptr);
    lv_obj_add_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
  }
}
static void ui_ve_mode_changed(lv_event_t *e) {
  (void)e;
  if (ve_syncing) return;
  ve_draft_mode = lv_dropdown_get_selected(dropdown_ve_mode);
  ve_mode_dirty = true;
  ve_local_error = false;
  ve_show_command_result = false;
}
static void ui_ve_apply(lv_event_t *e) {
  (void)e;
  if (ve_submitted || (!ve_power_dirty && !ve_mode_dirty)) return;
  uint32_t watts = ve_draft_w;
  if (ve_power_dirty && !ui_ve_parse_power(&watts)) {
    ui_ve_error("Puissance hors plage. Corriger la saisie.");
    return;
  }
  if (ve_mode_dirty && ve_draft_mode != 1 && ve_draft_mode != 2) {
    ui_ve_error("Choisir Solaire uniquement ou Libre.");
    return;
  }
  EvDeyeCommand command = {};
  command.set_power = ve_power_dirty;
  command.set_mode = ve_mode_dirty;
  command.power_raw = watts / DEYE_EV_POWER_REGISTER_SCALE_W;
  command.mode = ve_draft_mode;
  if (!deye_submit_ev_command(command)) {
    ui_ve_error("Commande refusee : attendre une lecture valide.");
    return;
  }
  ve_submitted = true;
  ve_show_command_result = true;
  ve_local_error = false;
  lv_obj_add_state(button_ve_apply, LV_STATE_DISABLED);
  lv_label_set_text(label_ve_result, "Commande en attente...");
}
static void ui_ve_return(lv_event_t *e) {
  (void)e;
  // RETOUR abandonne uniquement le brouillon ; une commande deja envoyee termine.
  ve_editing = false;
  ve_power_dirty = false;
  ve_mode_dirty = false;
  ve_local_error = false;
  lv_keyboard_set_textarea(keyboard_ve_deye, nullptr);
  lv_obj_add_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
  ui_show_dashboard(nullptr);
}
static lv_obj_t *ui_ve_button(const char *text, int x, lv_event_cb_t callback) {
  lv_obj_t *button = lv_btn_create(screen_ve_deye);
  lv_obj_set_pos(button, x, 420);
  lv_obj_set_size(button, 205, 44);
  lv_obj_set_style_bg_color(button, lv_color_hex(ui_ve_deye_theme().accent), LV_PART_MAIN);
  lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, lv_color_hex(ui_ve_deye_theme().accent_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_center(label);
  return button;
}
static void ui_ve_deye_create() {
  const UiThemePalette &t = ui_ve_deye_theme();
  screen_ve_deye = lv_obj_create(nullptr);
  lv_obj_set_style_pad_all(screen_ve_deye, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_color(screen_ve_deye, lv_color_hex(t.screen_bg), LV_PART_MAIN);
  lv_obj_clear_flag(screen_ve_deye, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *title = ui_ve_deye_label("VE / ONDULEUR DEYE", 0, 12, LCD_W, &lv_font_montserrat_20, t.accent);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  ui_ve_deye_label("CONSIGNE VERS BORNE", 20, 53, 220, &lv_font_montserrat_14, t.muted_text);
  label_ve_requested = ui_ve_deye_label("-- W", 20, 75, 210, &lv_font_montserrat_20, t.text);
  ui_ve_deye_label("PLAFOND REGLE", 250, 53, 210, &lv_font_montserrat_14, t.muted_text);
  label_ve_max_power = ui_ve_deye_label("-- W", 250, 75, 210, &lv_font_montserrat_20, t.text);
  ui_ve_deye_label("RESEAU", 20, 111, 210, &lv_font_montserrat_14, t.muted_text);
  label_ve_grid_power = ui_ve_deye_label("-- W", 20, 131, 210, &lv_font_montserrat_20, t.text);
  ui_ve_deye_label("CONSOMMATION", 250, 111, 210, &lv_font_montserrat_14, t.muted_text);
  label_ve_load_power = ui_ve_deye_label("-- W", 250, 131, 210, &lv_font_montserrat_20, t.text);
  label_ve_diagnostic = ui_ve_deye_label("R259/260 : en attente\nR709 : en attente", 20, 164, 440, &lv_font_montserrat_12, t.muted_text);
  char range[80];
  snprintf(range, sizeof(range), "Plafond : 1400 - %lu W (mono)", (unsigned long)DEYE_EV_INSTALLATION_MAX_POWER_W);
  ui_ve_deye_label(range, 20, 212, 440, &lv_font_montserrat_16, t.accent);
  slider_ve_max_power = lv_slider_create(screen_ve_deye);
  lv_obj_set_pos(slider_ve_max_power, 25, 254);
  lv_obj_set_size(slider_ve_max_power, 280, 18);
  lv_slider_set_range(slider_ve_max_power, 1400, DEYE_EV_INSTALLATION_MAX_POWER_W);
  lv_obj_set_style_bg_color(slider_ve_max_power, lv_color_hex(t.control_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_ve_max_power, lv_color_hex(t.accent), LV_PART_INDICATOR);
  lv_obj_add_event_cb(slider_ve_max_power, ui_ve_slider_changed, LV_EVENT_VALUE_CHANGED, nullptr);
  textarea_ve_max_power = lv_textarea_create(screen_ve_deye);
  lv_obj_set_pos(textarea_ve_max_power, 325, 239);
  lv_obj_set_size(textarea_ve_max_power, 135, 43);
  lv_textarea_set_one_line(textarea_ve_max_power, true);
  lv_textarea_set_accepted_chars(textarea_ve_max_power, "0123456789");
  lv_textarea_set_max_length(textarea_ve_max_power, 5);
  lv_obj_set_style_bg_color(textarea_ve_max_power, lv_color_hex(t.control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(textarea_ve_max_power, lv_color_hex(t.text), LV_PART_MAIN);
  dropdown_ve_mode = lv_dropdown_create(screen_ve_deye);
  lv_obj_set_pos(dropdown_ve_mode, 20, 289);
  lv_obj_set_size(dropdown_ve_mode, 440, 39);
  lv_dropdown_set_options(dropdown_ve_mode, "Mode non reconnu\nSolaire uniquement\nLibre");
  lv_obj_set_style_bg_color(dropdown_ve_mode, lv_color_hex(t.control_bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(dropdown_ve_mode, lv_color_hex(t.text), LV_PART_MAIN);
  lv_obj_set_style_text_font(dropdown_ve_mode, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_add_event_cb(dropdown_ve_mode, ui_ve_mode_changed, LV_EVENT_VALUE_CHANGED, nullptr);
  label_ve_mode_read = ui_ve_deye_label("Mode lu : --", 20, 337, 440, &lv_font_montserrat_14, t.text);
  label_ve_result = ui_ve_deye_label("En attente des parametres de l'onduleur.", 20, 360, 440, &lv_font_montserrat_14, t.accent);
  ui_ve_deye_label("Mesure / etat de la borne : indisponibles", 20, 400, 440, &lv_font_montserrat_12, t.muted_text);
  ui_ve_button("RETOUR", 20, ui_ve_return);
  button_ve_apply = ui_ve_button("APPLIQUER", 255, ui_ve_apply);
  lv_obj_add_state(button_ve_apply, LV_STATE_DISABLED);
  keyboard_ve_deye = lv_keyboard_create(screen_ve_deye);
  lv_obj_set_size(keyboard_ve_deye, LCD_W, 160);
  lv_obj_align(keyboard_ve_deye, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_mode(keyboard_ve_deye, LV_KEYBOARD_MODE_NUMBER);
  lv_obj_add_flag(keyboard_ve_deye, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(textarea_ve_max_power, ui_ve_text_event, LV_EVENT_ALL, nullptr);
  ui_ve_sync_power(ve_draft_w);
}
void ui_show_ve_deye(lv_event_t *e) {
  (void)e;
  if (!cfg_ev_charger_enabled) return;
  if (!screen_ve_deye) ui_ve_deye_create();
  deye_solarman_set_ui_active(false);
  lv_scr_load(screen_ve_deye);
}
static void ui_ve_deye_update() {
  if (!screen_ve_deye || !cfg_ev_charger_enabled || lv_scr_act() != screen_ve_deye) return;
  EvDeyeData ev = {};
  if (!deye_copy_ev_snapshot(&ev)) {
    lv_obj_add_state(button_ve_apply, LV_STATE_DISABLED);
    return;
  }
  char text[160];
  if (ev.requested_power_valid) snprintf(text, sizeof(text), "%u W", ev.requested_power_w);
  else snprintf(text, sizeof(text), "-- W");
  lv_label_set_text(label_ve_requested, text);
  if (ev.valid) snprintf(text, sizeof(text), "%lu W", (unsigned long)deye_ev_max_power_w(ev.max_charge_power_raw));
  else snprintf(text, sizeof(text), "-- W");
  lv_label_set_text(label_ve_max_power, text);
  snprintf(text, sizeof(text), "R259=0x%04X R260=%u [%s]\nR709=%u [%s]", ev.mode_raw, ev.max_charge_power_raw,
    ev.valid ? "OK" : "indisponible", ev.requested_power_w, ev.requested_power_valid ? "OK" : "indisponible");
  lv_label_set_text(label_ve_diagnostic, text);
  snprintf(text, sizeof(text), "Mode lu : %s", ev.valid ? deye_ev_mode_name(ev.mode_raw) : "--");
  lv_label_set_text(label_ve_mode_read, text);
  DashboardData main = {};
  uint16_t a = 0, b = 0, c = 0, d = 0;
  bool f = false, g = false;
  const bool main_valid = deye_copy_snapshot(&main, &a, &f, &b, &c, &d, &g) && main.valid;
  if (main_valid) snprintf(text, sizeof(text), "%d W", main.grid_power);
  else snprintf(text, sizeof(text), "-- W");
  lv_label_set_text(label_ve_grid_power, text);
  if (main_valid) snprintf(text, sizeof(text), "%d W", main.load_power);
  else snprintf(text, sizeof(text), "-- W");
  lv_label_set_text(label_ve_load_power, text);
  const bool busy = deye_ev_command_busy(ev.command_state);
  if (ve_submitted && !busy) {
    ve_submitted = false;
    if (ev.command_state == EV_COMMAND_CONFIRMED) { ve_power_dirty = false; ve_mode_dirty = false; }
  }
  if (ev.valid && !busy) {
    if (!ve_power_dirty && !ve_editing) {
      ve_draft_w = deye_ev_max_power_w(ev.max_charge_power_raw);
      ui_ve_sync_power(ve_draft_w);
    }
    if (!ve_mode_dirty) {
      ve_draft_mode = ev.mode_raw & 3;
      if (ve_draft_mode > 2) ve_draft_mode = 0;
      ve_syncing = true;
      lv_dropdown_set_selected(dropdown_ve_mode, ve_draft_mode);
      ve_syncing = false;
    }
  }
  const bool controls_enabled = ev.valid && !busy;
  lv_obj_t *controls[] = {slider_ve_max_power, textarea_ve_max_power, dropdown_ve_mode};
  for (lv_obj_t *control : controls) {
    if (controls_enabled) lv_obj_clear_state(control, LV_STATE_DISABLED);
    else lv_obj_add_state(control, LV_STATE_DISABLED);
  }
  if (controls_enabled && (ve_power_dirty || ve_mode_dirty) && !ve_local_error)
    lv_obj_clear_state(button_ve_apply, LV_STATE_DISABLED);
  else lv_obj_add_state(button_ve_apply, LV_STATE_DISABLED);
  if (!ve_local_error) {
    if (busy || ve_show_command_result) lv_label_set_text(label_ve_result, ev.command_message);
    else if (!ev.valid) lv_label_set_text(label_ve_result, "Parametres indisponibles : verifier la liaison.");
    else if (ve_power_dirty || ve_mode_dirty) lv_label_set_text(label_ve_result, "Modification locale : appuyer sur APPLIQUER.");
    else lv_label_set_text(label_ve_result, "Valeurs lues. Aucun changement envoye.");
  }
}
