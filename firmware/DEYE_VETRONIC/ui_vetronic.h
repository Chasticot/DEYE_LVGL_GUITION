#pragma once
#include <lvgl.h>
#include "vetronic.h"
#include "ui_theme.h"

extern void ui_show_dashboard(lv_event_t *e);
static lv_obj_t *screen_vetronic = nullptr, *vt_address_panel = nullptr;
static lv_obj_t *vt_address_textarea = nullptr, *vt_address_keyboard = nullptr;
static lv_obj_t *vt_address_save_button = nullptr, *vt_address_result_label = nullptr;
static lv_obj_t *vt_setpoint_label = nullptr, *vt_current_label = nullptr;
static lv_obj_t *vt_result_label = nullptr, *vt_current_buttons[2] = {nullptr};
static lv_obj_t *vt_soc_guard_label = nullptr, *vt_soc_guard_switch = nullptr;
static lv_obj_t *vt_soc_min_label = nullptr, *vt_soc_max_label = nullptr;
static lv_obj_t *vt_soc_hint_label = nullptr, *vt_soc_save_button = nullptr;
static lv_obj_t *vt_soc_buttons[4] = {nullptr}, *vt_mode_buttons[4] = {nullptr};
static bool vt_draft_dirty = false, vt_ui_pending = false, vt_soc_dirty = false;
static bool vt_soc_pending = false, vt_soc_syncing = false, vt_soc_guard_draft = false;
static int vt_current_draft = 6, vt_soc_stop_draft = 30, vt_soc_resume_draft = 35;
static void ui_vetronic_update();

static lv_obj_t *vt_label(const char *text, int x, int y, int width, const lv_font_t *font,
                          lv_obj_t *parent = nullptr) {
  lv_obj_t *o = lv_label_create(parent ? parent : screen_vetronic);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_width(o, width);
  lv_label_set_text(o, text);
  lv_obj_set_style_text_font(o, font, 0);
  lv_obj_set_style_text_color(o, lv_color_hex(ui_theme_palette(cfg_ui_theme).text), 0);
  return o;
}

static lv_obj_t *vt_button(const char *text, int x, int y, int width, int height,
                           lv_event_cb_t callback, void *user_data = nullptr,
                           lv_obj_t *parent = nullptr) {
  const auto &t = ui_theme_palette(cfg_ui_theme);
  lv_obj_t *o = lv_btn_create(parent ? parent : screen_vetronic);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_size(o, width, height);
  lv_obj_set_style_radius(o, 8, 0);
  lv_obj_set_style_pad_all(o, 0, 0);
  lv_obj_set_style_shadow_width(o, 0, 0);
  lv_obj_set_style_bg_color(o, lv_color_hex(t.accent), 0);
  lv_obj_set_style_border_color(o, lv_color_hex(t.text), 0);
  lv_obj_set_style_border_width(o, 0, 0);
  lv_obj_set_style_bg_opa(o, LV_OPA_40, LV_STATE_DISABLED);
  lv_obj_add_event_cb(o, callback, LV_EVENT_CLICKED, user_data);
  lv_obj_t *label = lv_label_create(o);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(t.accent_text), 0);
  lv_obj_center(label);
  return o;
}

static void vt_enable(lv_obj_t *o, bool enabled) {
  if (enabled) lv_obj_clear_state(o, LV_STATE_DISABLED);
  else lv_obj_add_state(o, LV_STATE_DISABLED);
}

static void vt_card(int y, int height) {
  const auto &t = ui_theme_palette(cfg_ui_theme);
  lv_obj_t *o = lv_obj_create(screen_vetronic);
  lv_obj_set_pos(o, 20, y);
  lv_obj_set_size(o, 440, height);
  lv_obj_set_style_radius(o, 8, 0);
  lv_obj_set_style_bg_color(o, lv_color_hex(t.card_bg), 0);
  lv_obj_set_style_border_color(o, lv_color_hex(t.border), 0);
  lv_obj_set_style_border_width(o, 1, 0);
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static void vt_ui_current_adjust(lv_event_t *e) {
  const VtSnapshot s = vt_snapshot();
  if (!s.online || s.busy || s.limit < 6) return;
  const int delta = static_cast<int>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));
  const int amps = max(6, min(min(s.limit, VETRONIC_MAX_CURRENT_A), vt_current_draft + delta));
  if (amps == vt_current_draft) return;
  vt_current_draft = amps;
  vt_draft_dirty = true;
  lv_label_set_text(vt_result_label, "Appuyer sur CHARGE IMMEDIATE pour appliquer.");
  ui_vetronic_update();
}

static void vt_ui_select_mode(lv_event_t *e) {
  const VtMode mode = static_cast<VtMode>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e)));
  if (vt_submit(mode, mode == VT_MANUAL ? vt_current_draft : 0)) {
    vt_ui_pending = true;
    vt_draft_dirty = false;
    lv_label_set_text(vt_result_label, "Commande en attente...");
  } else {
    lv_label_set_text(vt_result_label, "Commande refusee : verifier la liaison et les limites.");
  }
  ui_vetronic_update();
}

static void vt_ui_soc_draft_changed() {
  vt_soc_dirty = true;
  lv_label_set_text(vt_result_label, "SOC modifie : appuyer sur ENREGISTRER SOC.");
  ui_vetronic_update();
}

static void vt_ui_soc_guard_change(lv_event_t *) {
  if (vt_soc_syncing) return;
  vt_soc_guard_draft = lv_obj_has_state(vt_soc_guard_switch, LV_STATE_CHECKED);
  vt_ui_soc_draft_changed();
}

static void vt_ui_soc_adjust(lv_event_t *e) {
  const int action = static_cast<int>(reinterpret_cast<intptr_t>(lv_event_get_user_data(e)));
  int stop = vt_soc_stop_draft, resume = vt_soc_resume_draft;
  if (action == -1 || action == 1) stop = vt_soc_stop_adjust(stop, resume, action);
  else if (action == -2 || action == 2) resume = vt_soc_resume_adjust(stop, resume, action / 2);
  if (!vt_soc_guard_valid(stop, resume) || (stop == vt_soc_stop_draft && resume == vt_soc_resume_draft)) return;
  vt_soc_stop_draft = stop;
  vt_soc_resume_draft = resume;
  vt_ui_soc_draft_changed();
}

static void vt_ui_save_soc_guard(lv_event_t *) {
  if (vt_submit_soc_guard(vt_soc_guard_draft, vt_soc_stop_draft, vt_soc_resume_draft)) {
    vt_soc_pending = true;
    vt_ui_pending = true;
    lv_label_set_text(vt_result_label, "Enregistrement SOC en cours...");
  } else {
    lv_label_set_text(vt_result_label, "SOC non enregistre : passerelle indisponible ou a mettre a jour.");
  }
  ui_vetronic_update();
}

static void vt_address_event(lv_event_t *e) {
  const lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_FOCUSED) {
    lv_keyboard_set_textarea(vt_address_keyboard, vt_address_textarea);
    lv_obj_clear_flag(vt_address_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(vt_address_keyboard);
  } else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
    lv_keyboard_set_textarea(vt_address_keyboard, nullptr);
    lv_obj_add_flag(vt_address_keyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

static void vt_ui_close_address(lv_event_t *) {
  lv_obj_clear_state(vt_address_textarea, LV_STATE_FOCUSED);
  lv_keyboard_set_textarea(vt_address_keyboard, nullptr);
  lv_obj_add_flag(vt_address_keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(vt_address_panel, LV_OBJ_FLAG_HIDDEN);
}

static void vt_ui_open_address(lv_event_t *) {
  lv_textarea_set_text(vt_address_textarea, cfg_vetronic_host.c_str());
  lv_label_set_text(vt_address_result_label, "");
  lv_obj_clear_flag(vt_address_panel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(vt_address_panel);
}

static void vt_ui_save_address(lv_event_t *) {
  const char *address = lv_textarea_get_text(vt_address_textarea);
  if (vt_save_host(address)) {
    vt_ui_close_address(nullptr);
    // Drafts belong to the previous gateway and must not carry over.
    vt_draft_dirty = false;
    vt_soc_dirty = false;
    lv_label_set_text(vt_result_label, "Adresse enregistree. Connexion en cours...");
    ui_vetronic_update();
  } else {
    lv_label_set_text(vt_address_result_label, "Adresse IPv4 invalide. Exemple : 192.168.1.130");
  }
}

static void vt_ui_return(lv_event_t *) {
  vt_draft_dirty = false;
  ui_show_dashboard(nullptr);
}

static void ui_vetronic_create() {
  const auto &t = ui_theme_palette(cfg_ui_theme);
  screen_vetronic = lv_obj_create(nullptr);
  lv_obj_set_style_pad_all(screen_vetronic, 0, 0);
  lv_obj_set_style_bg_color(screen_vetronic, lv_color_hex(t.screen_bg), 0);
  lv_obj_clear_flag(screen_vetronic, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *title = vt_label("VE TRONIC / WB01", 20, 12, 440, &lv_font_montserrat_20);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(t.accent), 0);
  vt_setpoint_label = vt_label("Consigne : -- A", 20, 51, 315, &lv_font_montserrat_16);
  vt_button("RESEAU", 350, 42, 110, 34, vt_ui_open_address);

  vt_card(84, 66);
  vt_label("Intensite de charge", 32, 95, 175, &lv_font_montserrat_14);
  lv_obj_t *hint = vt_label("Reglage par pas de 1 A", 32, 120, 175, &lv_font_montserrat_12);
  lv_obj_set_style_text_color(hint, lv_color_hex(t.muted_text), 0);
  vt_current_buttons[0] = vt_button("-", 214, 95, 44, 44, vt_ui_current_adjust,
                                  reinterpret_cast<void *>(static_cast<intptr_t>(-1)));
  vt_current_label = vt_label("6 A", 266, 104, 126, &lv_font_montserrat_26);
  lv_obj_set_style_text_align(vt_current_label, LV_TEXT_ALIGN_CENTER, 0);
  vt_current_buttons[1] = vt_button("+", 400, 95, 44, 44, vt_ui_current_adjust,
                                  reinterpret_cast<void *>(static_cast<intptr_t>(1)));

  vt_mode_buttons[VT_STOP] = vt_button("ARRET", 20, 160, 212, 42, vt_ui_select_mode,
    reinterpret_cast<void *>(static_cast<uintptr_t>(VT_STOP)));
  // Stop keeps its familiar red cue; other actions use the settings palette.
  lv_obj_set_style_bg_color(vt_mode_buttons[VT_STOP], lv_color_hex(0xB91C1C), 0);
  lv_obj_set_style_text_color(lv_obj_get_child(vt_mode_buttons[VT_STOP], 0), lv_color_hex(0xFFFFFF), 0);
  vt_mode_buttons[VT_MANUAL] = vt_button("CHARGE IMMEDIATE", 248, 160, 212, 42, vt_ui_select_mode,
    reinterpret_cast<void *>(static_cast<uintptr_t>(VT_MANUAL)));
  vt_mode_buttons[VT_SOLAR] = vt_button("SOLAIRE", 20, 210, 212, 42, vt_ui_select_mode,
    reinterpret_cast<void *>(static_cast<uintptr_t>(VT_SOLAR)));
  vt_mode_buttons[VT_LEGACY] = vt_button("BORNE / JEEDOM", 248, 210, 212, 42, vt_ui_select_mode,
    reinterpret_cast<void *>(static_cast<uintptr_t>(VT_LEGACY)));

  vt_card(262, 120);
  vt_label("Protection SOC solaire", 32, 274, 255, &lv_font_montserrat_16);
  vt_soc_guard_label = vt_label("INACTIVE", 301, 276, 88, &lv_font_montserrat_12);
  lv_obj_set_style_text_align(vt_soc_guard_label, LV_TEXT_ALIGN_RIGHT, 0);
  vt_soc_guard_switch = lv_switch_create(screen_vetronic);
  lv_obj_set_pos(vt_soc_guard_switch, 400, 270);
  lv_obj_set_size(vt_soc_guard_switch, 44, 26);
  lv_obj_set_style_bg_color(vt_soc_guard_switch, lv_color_hex(t.switch_off), LV_PART_MAIN);
  lv_obj_set_style_bg_color(vt_soc_guard_switch, lv_color_hex(t.switch_on), LV_PART_INDICATOR | LV_STATE_CHECKED);
  lv_obj_add_event_cb(vt_soc_guard_switch, vt_ui_soc_guard_change, LV_EVENT_VALUE_CHANGED, nullptr);

  hint = vt_label("MIN / arret", 32, 302, 192, &lv_font_montserrat_12);
  lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
  hint = vt_label("MAX / reprise", 256, 302, 192, &lv_font_montserrat_12);
  lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
  vt_soc_buttons[0] = vt_button("-", 32, 320, 40, 38, vt_ui_soc_adjust, reinterpret_cast<void *>(static_cast<intptr_t>(-1)));
  vt_soc_min_label = vt_label("30 %", 78, 327, 100, &lv_font_montserrat_20);
  lv_obj_set_style_text_align(vt_soc_min_label, LV_TEXT_ALIGN_CENTER, 0);
  vt_soc_buttons[1] = vt_button("+", 184, 320, 40, 38, vt_ui_soc_adjust, reinterpret_cast<void *>(static_cast<intptr_t>(1)));
  vt_soc_buttons[2] = vt_button("-", 256, 320, 40, 38, vt_ui_soc_adjust, reinterpret_cast<void *>(static_cast<intptr_t>(-2)));
  vt_soc_max_label = vt_label("35 %", 302, 327, 100, &lv_font_montserrat_20);
  lv_obj_set_style_text_align(vt_soc_max_label, LV_TEXT_ALIGN_CENTER, 0);
  vt_soc_buttons[3] = vt_button("+", 408, 320, 40, 38, vt_ui_soc_adjust, reinterpret_cast<void *>(static_cast<intptr_t>(2)));
  vt_soc_hint_label = vt_label("Ecart MIN / MAX : 5 points minimum", 32, 364, 416, &lv_font_montserrat_12);
  lv_obj_set_style_text_color(vt_soc_hint_label, lv_color_hex(t.muted_text), 0);
  lv_obj_set_style_text_align(vt_soc_hint_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(vt_soc_hint_label, LV_LABEL_LONG_SCROLL_CIRCULAR);

  // Long gateway replies remain readable by scrolling inside the status area.
  lv_obj_t *result_area = lv_obj_create(screen_vetronic);
  lv_obj_set_pos(result_area, 20, 388);
  lv_obj_set_size(result_area, 440, 36);
  lv_obj_set_style_pad_all(result_area, 0, 0);
  lv_obj_set_style_border_width(result_area, 0, 0);
  lv_obj_set_style_bg_opa(result_area, LV_OPA_TRANSP, 0);
  lv_obj_set_scroll_dir(result_area, LV_DIR_VER);
  vt_result_label = vt_label("Choisir une action pour piloter la charge.", 0, 0, 426,
                             &lv_font_montserrat_14, result_area);
  lv_label_set_long_mode(vt_result_label, LV_LABEL_LONG_WRAP);
  vt_button("RETOUR", 20, 432, 164, 38, vt_ui_return);
  vt_soc_save_button = vt_button("ENREGISTRER SOC", 200, 432, 260, 38, vt_ui_save_soc_guard);

  // Network setup is occasional; its overlay leaves the daily controls spacious.
  vt_address_panel = lv_obj_create(screen_vetronic);
  lv_obj_set_pos(vt_address_panel, 0, 0);
  lv_obj_set_size(vt_address_panel, 480, 480);
  lv_obj_set_style_pad_all(vt_address_panel, 0, 0);
  lv_obj_set_style_border_width(vt_address_panel, 0, 0);
  lv_obj_set_style_radius(vt_address_panel, 0, 0);
  lv_obj_set_style_bg_color(vt_address_panel, lv_color_hex(t.screen_bg), 0);
  lv_obj_set_style_bg_opa(vt_address_panel, LV_OPA_COVER, 0);
  lv_obj_clear_flag(vt_address_panel, LV_OBJ_FLAG_SCROLLABLE);
  title = vt_label("RESEAU / VE TRONIC", 20, 12, 440, &lv_font_montserrat_20, vt_address_panel);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(t.accent), 0);
  vt_label("Adresse IP de la passerelle", 20, 73, 440, &lv_font_montserrat_16, vt_address_panel);
  vt_address_textarea = lv_textarea_create(vt_address_panel);
  lv_textarea_set_one_line(vt_address_textarea, true);
  lv_textarea_set_max_length(vt_address_textarea, 15);
  lv_textarea_set_accepted_chars(vt_address_textarea, "0123456789.");
  lv_textarea_set_text(vt_address_textarea, cfg_vetronic_host.c_str());
  lv_obj_set_pos(vt_address_textarea, 20, 108);
  lv_obj_set_size(vt_address_textarea, 440, 44);
  lv_obj_set_style_bg_color(vt_address_textarea, lv_color_hex(t.control_bg), 0);
  lv_obj_set_style_text_color(vt_address_textarea, lv_color_hex(t.text), 0);
  lv_obj_set_style_border_color(vt_address_textarea, lv_color_hex(t.control_border), 0);
  lv_obj_set_style_border_width(vt_address_textarea, 1, 0);
  lv_obj_set_style_radius(vt_address_textarea, 6, 0);
  vt_button("RETOUR", 20, 176, 212, 44, vt_ui_close_address, nullptr, vt_address_panel);
  vt_address_save_button = vt_button("ENREGISTRER", 248, 176, 212, 44, vt_ui_save_address, nullptr, vt_address_panel);
  vt_address_result_label = vt_label("", 20, 238, 440, &lv_font_montserrat_14, vt_address_panel);
  vt_address_keyboard = lv_keyboard_create(vt_address_panel);
  lv_keyboard_set_mode(vt_address_keyboard, LV_KEYBOARD_MODE_NUMBER);
  lv_obj_set_size(vt_address_keyboard, 480, 190);
  lv_obj_align(vt_address_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(vt_address_keyboard, lv_color_hex(t.control_bg), LV_PART_MAIN);
  lv_obj_add_flag(vt_address_keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(vt_address_textarea, vt_address_event, LV_EVENT_ALL, nullptr);
  lv_obj_add_flag(vt_address_panel, LV_OBJ_FLAG_HIDDEN);
}

static void ui_vetronic_update() {
  if (!screen_vetronic || lv_scr_act() != screen_vetronic) return;
  const VtSnapshot s = vt_snapshot();
  char text[96];
  if (s.online) snprintf(text, sizeof(text), "Consigne : %d A%s", max(0, s.target), s.soc_blocked ? " / Pause SOC" : "");
  else snprintf(text, sizeof(text), "Passerelle hors ligne / -- A");
  lv_label_set_text(vt_setpoint_label, text);
  if (s.online && !s.busy && !vt_draft_dirty)
    vt_current_draft = max(6, min(min(s.limit, VETRONIC_MAX_CURRENT_A), s.target));
  snprintf(text, sizeof(text), "%d A", vt_current_draft);
  lv_label_set_text(vt_current_label, text);

  if (s.busy || vt_ui_pending) {
    if (*s.result) lv_label_set_text(vt_result_label, s.result);
    if (!s.busy) vt_ui_pending = false;
  }
  if (!s.busy && vt_soc_pending) {
    vt_soc_pending = false;
    if (s.online && s.soc_guard == vt_soc_guard_draft && s.soc_stop == vt_soc_stop_draft && s.soc_resume == vt_soc_resume_draft)
      vt_soc_dirty = false;
  }
  if (!vt_soc_pending && !vt_soc_dirty && s.online && vt_soc_guard_valid(s.soc_stop, s.soc_resume)) {
    vt_soc_guard_draft = s.soc_guard;
    vt_soc_stop_draft = s.soc_stop;
    vt_soc_resume_draft = s.soc_resume;
    vt_soc_syncing = true;
    if (s.soc_guard) lv_obj_add_state(vt_soc_guard_switch, LV_STATE_CHECKED);
    else lv_obj_clear_state(vt_soc_guard_switch, LV_STATE_CHECKED);
    vt_soc_syncing = false;
  }
  lv_label_set_text(vt_soc_guard_label, vt_soc_guard_draft ? "ACTIVE" : "INACTIVE");
  snprintf(text, sizeof(text), "%d %%", vt_soc_stop_draft);
  lv_label_set_text(vt_soc_min_label, text);
  snprintf(text, sizeof(text), "%d %%", vt_soc_resume_draft);
  lv_label_set_text(vt_soc_max_label, text);

  const bool available = cfg_ev_charger_enabled && WiFi.status() == WL_CONNECTED && !s.busy;
  for (uint8_t mode = VT_STOP; mode <= VT_LEGACY; ++mode) {
    vt_enable(vt_mode_buttons[mode], available && (mode == VT_STOP || s.online) &&
      (mode != VT_MANUAL || vt_manual_allowed(vt_current_draft, s.limit)));
    lv_obj_set_style_border_width(vt_mode_buttons[mode], s.online && s.mode == mode ? 3 : 0, 0);
  }
  const bool current_available = available && s.online && s.limit >= 6;
  vt_enable(vt_current_buttons[0], current_available && vt_current_draft > 6);
  vt_enable(vt_current_buttons[1], current_available && vt_current_draft < min(s.limit, VETRONIC_MAX_CURRENT_A));
  const bool soc_available = available && s.online && s.soc_control_api;
  if (!s.online) lv_label_set_text(vt_soc_hint_label, "SOC indisponible : passerelle hors ligne");
  else if (!s.soc_control_api) lv_label_set_text(vt_soc_hint_label, "SOC : mise a jour de la passerelle requise");
  else if (vt_soc_pending) lv_label_set_text(vt_soc_hint_label, "Enregistrement SOC en cours...");
  else if (vt_soc_dirty) lv_label_set_text(vt_soc_hint_label, "Modifications SOC a enregistrer / ecart mini : 5 pts");
  else lv_label_set_text(vt_soc_hint_label, "Ecart MIN / MAX : 5 points minimum");
  vt_enable(vt_soc_guard_switch, soc_available);
  // Thresholds can be prepared even when the guard is switched off.
  vt_enable(vt_soc_buttons[0], soc_available && vt_soc_stop_draft > 0);
  vt_enable(vt_soc_buttons[1], soc_available && vt_soc_stop_draft < vt_soc_resume_draft - 5);
  vt_enable(vt_soc_buttons[2], soc_available && vt_soc_resume_draft > vt_soc_stop_draft + 5);
  vt_enable(vt_soc_buttons[3], soc_available && vt_soc_resume_draft < 100);
  vt_enable(vt_soc_save_button, soc_available && vt_soc_dirty);
  vt_enable(vt_address_save_button, !s.busy);
}

void ui_show_vetronic(lv_event_t *) {
  if (!cfg_ev_charger_enabled) return;
  if (!screen_vetronic) ui_vetronic_create();
  deye_solarman_set_ui_active(false);
  lv_scr_load(screen_vetronic);
  ui_vetronic_update();
}
