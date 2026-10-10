#pragma once
#include <errno.h>
#include "ev_backend.h"
#ifndef DEYE_V2_UI_TEST
#include "v2_runtime.h"
#endif

static lv_obj_t *v2_screen = nullptr, *v2_body = nullptr, *v2_keyboard = nullptr;
static lv_obj_t *v2_status = nullptr;
static lv_obj_t *v2_rule = nullptr;
static bool v2_sources_page = false;
static uint8_t v2_gen_mode_draft = 0;
static V2Config v2_draft;
static bool v2_tempo_draft, v2_colorblind_draft;
static uint8_t v2_ev_backend_draft = 0;
static bool v2_ev_options_page = false;
enum V2FieldKind { V2_BOOL, V2_UINT, V2_FLOAT, V2_TIME, V2_CHOICE };
struct V2Field { lv_obj_t *object; void *value; V2FieldKind kind; };
static V2Field v2_fields[16];
static uint8_t v2_field_count = 0;
static int v2_row_y = 0;

static lv_obj_t *v2_label(lv_obj_t *parent, const char *text, int x, int y, int width) {
  auto label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y); lv_obj_set_width(label, width);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(ui_settings_theme().text), 0);
  return label;
}
static void v2_input_event(lv_event_t *event) {
  const auto code = lv_event_get_code(event);
  if (code == LV_EVENT_FOCUSED) {
    lv_keyboard_set_textarea(v2_keyboard, lv_event_get_target(event));
    lv_obj_clear_flag(v2_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_height(v2_body, 210);
    lv_obj_scroll_to_view(lv_event_get_target(event), LV_ANIM_ON);
    lv_obj_move_foreground(v2_keyboard);
  } else if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(v2_keyboard, nullptr);
    lv_obj_add_flag(v2_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_height(v2_body, 338);
  }
}
static void v2_new_page(const char *title, lv_event_cb_t back, bool form = false) {
  lv_obj_t *old = v2_screen;
  v2_screen = lv_obj_create(nullptr);
  lv_obj_set_style_pad_all(v2_screen, 0, 0);
  lv_obj_set_style_bg_color(v2_screen, lv_color_hex(ui_settings_theme().screen_bg), 0);
  lv_obj_clear_flag(v2_screen, LV_OBJ_FLAG_SCROLLABLE);
  ui_settings_make_title(v2_screen, title);
  v2_body = lv_obj_create(v2_screen);
  lv_obj_set_pos(v2_body, 20, 62); lv_obj_set_size(v2_body, 440, 338);
  lv_obj_set_style_pad_all(v2_body, 0, 0);
  lv_obj_set_style_border_width(v2_body, 0, 0);
  lv_obj_set_style_radius(v2_body, 0, 0);
  lv_obj_set_style_bg_opa(v2_body, LV_OPA_TRANSP, 0);
  lv_obj_set_scroll_dir(v2_body, LV_DIR_VER);
  ui_settings_make_button(v2_screen, LV_SYMBOL_LEFT " RETOUR", 20, 422, form ? 190 : 440, 44, back);
  v2_field_count = 0; v2_row_y = 0; v2_status = nullptr;
  v2_rule = nullptr; v2_sources_page = false;
  v2_ev_options_page = false;
  v2_gen_mode_draft = settings_get_gen_mode() ? 0 : 1;
  v2_draft = cfg_v2;
  v2_tempo_draft = cfg_tempo_enabled;
  v2_colorblind_draft = cfg_tempo_colorblind_mode;
  v2_ev_backend_draft = static_cast<uint8_t>(cfg_ev_backend);
  if (form) {
    v2_keyboard = lv_keyboard_create(v2_screen);
    lv_obj_set_size(v2_keyboard, 480, 190);
    lv_obj_align(v2_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(v2_keyboard, LV_KEYBOARD_MODE_NUMBER);
    static const char *keys[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n",
      "-", "0", ":", ".", "\n", LV_SYMBOL_BACKSPACE, LV_SYMBOL_CLOSE, LV_SYMBOL_OK, ""};
    static const lv_btnmatrix_ctrl_t controls[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    lv_keyboard_set_map(v2_keyboard, LV_KEYBOARD_MODE_NUMBER, keys, controls);
    lv_obj_set_style_bg_color(v2_keyboard, lv_color_hex(ui_settings_theme().screen_bg), 0);
    lv_obj_set_style_bg_color(v2_keyboard, lv_color_hex(ui_settings_theme().control_bg), LV_PART_ITEMS);
    lv_obj_set_style_text_color(v2_keyboard, lv_color_hex(ui_settings_theme().text), LV_PART_ITEMS);
    lv_obj_add_flag(v2_keyboard, LV_OBJ_FLAG_HIDDEN);
  }
  lv_scr_load(v2_screen);
  if (old) lv_obj_del_async(old);
}
static void v2_field(const char *name, void *value, V2FieldKind kind, const char *options = nullptr) {
  if (v2_field_count >= 16) return;
  v2_label(v2_body, name, 0, v2_row_y + 10, 270);
  lv_obj_t *object;
  char text[32];
  if (kind == V2_BOOL) {
    object = lv_switch_create(v2_body);
    lv_obj_set_pos(object, 356, v2_row_y + 4); lv_obj_set_size(object, 64, 34);
    lv_obj_set_style_pad_all(object, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(object, lv_color_hex(ui_settings_theme().switch_off), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(object, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);
    if (*static_cast<bool *>(value)) lv_obj_add_state(object, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(object, lv_color_hex(ui_settings_theme().switch_on), LV_PART_INDICATOR | LV_STATE_CHECKED);
  } else if (kind == V2_CHOICE) {
    object = lv_dropdown_create(v2_body);
    lv_dropdown_set_options(object, options);
    lv_dropdown_set_selected(object, *static_cast<uint8_t *>(value));
    lv_obj_set_pos(object, 280, v2_row_y); lv_obj_set_size(object, 140, 44);
  } else {
    object = lv_textarea_create(v2_body);
    lv_textarea_set_one_line(object, true);
    lv_textarea_set_max_length(object, 12);
    if (kind == V2_FLOAT) snprintf(text, sizeof(text), "%.3g", *static_cast<float *>(value));
    else if (kind == V2_TIME) {
      unsigned minute = *static_cast<uint16_t *>(value);
      snprintf(text, sizeof(text), "%02u:%02u", minute / 60, minute % 60);
    } else snprintf(text, sizeof(text), "%u", *static_cast<uint16_t *>(value));
    lv_textarea_set_text(object, text);
    lv_textarea_set_accepted_chars(object, kind == V2_TIME ? "0123456789:" : kind == V2_FLOAT ? "0123456789.-" : "0123456789");
    lv_obj_set_pos(object, 290, v2_row_y); lv_obj_set_size(object, 130, 44);
    lv_obj_add_event_cb(object, v2_input_event, LV_EVENT_ALL, nullptr);
  }
  lv_obj_set_style_text_font(object, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(object, lv_color_hex(ui_settings_theme().text), 0);
  if (kind != V2_BOOL) lv_obj_set_style_bg_color(object, lv_color_hex(ui_settings_theme().control_bg), 0);
  v2_fields[v2_field_count++] = {object, value, kind};
  v2_row_y += 58;
}
static bool v2_parse_form() {
  for (uint8_t i = 0; i < v2_field_count; ++i) {
    const V2Field &f = v2_fields[i];
    if (f.kind == V2_BOOL) *static_cast<bool *>(f.value) = lv_obj_has_state(f.object, LV_STATE_CHECKED);
    else if (f.kind == V2_CHOICE) *static_cast<uint8_t *>(f.value) = lv_dropdown_get_selected(f.object);
    else {
      const char *text = lv_textarea_get_text(f.object);
      if (!text || !*text) return false;
      if (f.kind == V2_TIME) {
        if (strlen(text) != 5 || text[2] != ':' || text[0] < '0' || text[0] > '9' ||
          text[1] < '0' || text[1] > '9' || text[3] < '0' || text[3] > '9' || text[4] < '0' || text[4] > '9') return false;
        const unsigned hour = (text[0] - '0') * 10 + text[1] - '0';
        const unsigned minute = (text[3] - '0') * 10 + text[4] - '0';
        if (hour > 23 || minute > 59) return false;
        *static_cast<uint16_t *>(f.value) = hour * 60 + minute;
      } else {
        char *end = nullptr;
        errno = 0;
        const float number = strtof(text, &end);
        if (errno || end == text || *end || !isfinite(number)) return false;
        if (f.kind == V2_UINT) {
          if (number < 0 || number > 65535 || number != floorf(number)) return false;
          *static_cast<uint16_t *>(f.value) = uint16_t(number);
        } else *static_cast<float *>(f.value) = number;
      }
    }
  }
  return v2_config_valid(v2_draft);
}
static void v2_save_form(lv_event_t *) {
  const char *error = nullptr;
  if (!v2_parse_form()) error = "Saisie invalide. Horaires HH:MM, reveil 1-3600 s, delai 0-3600 s.";
  else if (v2_ev_options_page && !ev_backend_supported(static_cast<EvBackend>(v2_ev_backend_draft), inverter_profile().ev_supported))
    error = "Deye LoRa indisponible sur ce modele. Choisir Aucune ou Vetronic WB01.";
  else if (!v2_save(v2_draft) || !(v2_ev_options_page ?
           settings_save_charging(v2_tempo_draft, v2_colorblind_draft, static_cast<EvBackend>(v2_ev_backend_draft)) :
           settings_save_tempo(v2_tempo_draft, v2_colorblind_draft, cfg_ev_charger_enabled)) ||
           (v2_sources_page && !settings_set_gen_mode(v2_gen_mode_draft == 0)))
    error = "Sauvegarde incomplete. Reessayer avant de quitter.";
  if (error) { lv_msgbox_create(nullptr, "Reglages", error, nullptr, true); return; }
  lv_obj_t *message = lv_msgbox_create(nullptr, "Sauvegarde", "Reglages enregistres. Redemarrage...", nullptr, false);
  lv_obj_center(message);
  lv_timer_create([](lv_timer_t *timer) { lv_timer_del(timer); ESP.restart(); }, 900, nullptr);
}
static void v2_save_button() {
  ui_settings_make_button(v2_screen, LV_SYMBOL_SAVE " ENREGISTRER", 230, 422, 230, 44, v2_save_form);
}

void ui_show_v2_sources(lv_event_t *) {
  v2_new_page("PRODUCTION PV / GEN", ui_show_v2_deye, true);
  v2_sources_page = true;
  v2_label(v2_body, "Mode GEN", 0, 10, 160);
  static const char *gen_options[] = {"SMARTLOAD", "GEN MO", ""};
  auto selector = lv_btnmatrix_create(v2_body);
  lv_btnmatrix_set_map(selector, gen_options);
  lv_obj_set_pos(selector, 160, 0); lv_obj_set_size(selector, 260, 44);
  lv_obj_set_style_pad_all(selector, 4, LV_PART_MAIN);
  lv_obj_set_style_text_font(selector, &lv_font_montserrat_14, LV_PART_ITEMS);
  lv_obj_set_style_bg_color(selector, lv_color_hex(ui_settings_theme().control_bg), LV_PART_MAIN);
  lv_obj_set_style_bg_color(selector, lv_color_hex(ui_settings_theme().control_bg), LV_PART_ITEMS);
  lv_obj_set_style_text_color(selector, lv_color_hex(ui_settings_theme().text), LV_PART_ITEMS);
  lv_obj_set_style_bg_color(selector, lv_color_hex(ui_settings_theme().accent), LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_obj_set_style_text_color(selector, lv_color_hex(ui_settings_theme().accent_text), LV_PART_ITEMS | LV_STATE_CHECKED);
  lv_btnmatrix_set_btn_ctrl_all(selector, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_one_checked(selector, true);
  lv_btnmatrix_set_btn_ctrl(selector, v2_gen_mode_draft, LV_BTNMATRIX_CTRL_CHECKED);
  lv_obj_add_event_cb(selector, [](lv_event_t *event) {
    const auto selected = lv_btnmatrix_get_selected_btn(lv_event_get_target(event));
    if (selected < 2) v2_gen_mode_draft = selected;
  }, LV_EVENT_VALUE_CHANGED, nullptr);
  if (!inverter_profile().gen_supported) {
    lv_obj_add_flag(selector, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_obj_get_child(v2_body, 0), LV_OBJ_FLAG_HIDDEN);
  }
  v2_row_y = inverter_profile().gen_supported ? 58 : 0;
  v2_field("Afficher PV1", &v2_draft.pv_visible[0], V2_BOOL);
  v2_field("Afficher PV2", &v2_draft.pv_visible[1], V2_BOOL);
  if (inverter_profile().pv_count >= 3) v2_field("Afficher PV3", &v2_draft.pv_visible[2], V2_BOOL);
  if (inverter_profile().pv_count >= 4) v2_field("Afficher PV4", &v2_draft.pv4_visible, V2_BOOL);
  if (inverter_profile().gen_supported) v2_field("Cumuler GEN MO + PV", &v2_draft.add_gen, V2_BOOL);
  if (inverter_profile().gen_supported) v2_field("Afficher les kWh GEN Daily", &v2_draft.show_gen_daily, V2_BOOL);
  if (inverter_profile().gen_supported) v2_label(v2_body, "Sans registre : estimation GEN (~ kWh)", 0, v2_row_y, 420);
  v2_save_button();
}
void ui_show_v2_tariffs(lv_event_t *) {
  v2_new_page("TARIFS / HEURES CREUSES", ui_show_settings_screen, true);
  v2_field("Mode heures creuses", &v2_draft.tariff_mode, V2_CHOICE, "2 plages\nTempo");
  v2_field("HC 1 debut", &v2_draft.hc_start[0], V2_TIME);
  v2_field("HC 1 fin", &v2_draft.hc_end[0], V2_TIME);
  v2_field("HC 2 debut", &v2_draft.hc_start[1], V2_TIME);
  v2_field("HC 2 fin", &v2_draft.hc_end[1], V2_TIME);
  v2_field("Afficher Tempo", &v2_tempo_draft, V2_BOOL);
  v2_field("Couleurs accessibles", &v2_colorblind_draft, V2_BOOL);
  v2_save_button();
}
void ui_show_v2_relay(lv_event_t *) {
  v2_new_page("RELAIS INTEGRE", ui_show_settings_screen, true);
  v2_status = v2_label(v2_body, "", 0, 0, 420);
  v2_rule = v2_label(v2_body, "", 0, 48, 420); v2_row_y = 100;
  v2_field("Activer la regle", &v2_draft.relay_enabled, V2_BOOL);
  v2_field("Registre Modbus", &v2_draft.relay_register, V2_UINT);
  v2_field("Valeur signee", &v2_draft.relay_signed, V2_BOOL);
  v2_field("Coefficient", &v2_draft.relay_coefficient, V2_FLOAT);
  v2_field("Comparateur", &v2_draft.relay_comparator, V2_CHOICE, "<\n=\n>");
  auto comparison = v2_fields[v2_field_count - 1].object;
  lv_obj_add_event_cb(comparison, [](lv_event_t *event) {
    const unsigned op = lv_dropdown_get_selected(lv_event_get_target(event));
    lv_label_set_text_fmt(v2_rule, "ON si (registre x coeff) %s seuil\nOFF sinon", op == 0 ? "<" : op == 1 ? "=" : ">");
  }, LV_EVENT_VALUE_CHANGED, nullptr);
  lv_event_send(comparison, LV_EVENT_VALUE_CHANGED, nullptr);
  v2_field("Seuil", &v2_draft.relay_threshold, V2_FLOAT);
  v2_field("Delai commutation (s)", &v2_draft.relay_delay_s, V2_UINT);
  v2_save_button();
}
void ui_show_v2_sleep(lv_event_t *) {
  v2_new_page("VEILLE HORAIRE", ui_show_v2_display, true);
  v2_field("Activer la veille", &v2_draft.sleep_enabled, V2_BOOL);
  v2_field("Debut de veille", &v2_draft.sleep_start, V2_TIME);
  v2_field("Fin de veille", &v2_draft.sleep_end, V2_TIME);
  v2_field("Duree de reveil (s)", &v2_draft.wake_seconds, V2_UINT);
  v2_label(v2_body, ntp_received.load() ? "Heure NTP synchronisee" : "En attente de l'heure NTP", 0, 250, 420);
  v2_save_button();
}
void ui_show_v2_ev_options(lv_event_t *) {
  v2_new_page("RECHARGE / TARIFS", ui_show_v2_vehicle, true);
  v2_ev_options_page = true;
  v2_field("Borne de recharge", &v2_ev_backend_draft, V2_CHOICE, "Aucune\nDeye LoRa\nVetronic WB01");
  // This selector has longer labels than the compact numeric choices.
  lv_obj_set_width(lv_obj_get_child(v2_body, 0), 190);
  lv_obj_set_pos(v2_fields[0].object, 200, 0);
  lv_obj_set_size(v2_fields[0].object, 220, 44);
  v2_field("Appliquer les tarifs", &v2_draft.ev_tariff_enabled, V2_BOOL);
  v2_field("Reseau en HC seulement", &v2_draft.ev_hc_only, V2_BOOL);
  v2_field("Interdire HP rouge Tempo", &v2_draft.ev_block_red_hp, V2_BOOL);
  v2_label(v2_body, cfg_ev_backend_needs_selection ?
    (inverter_profile().ev_supported ? "Choisir la borne puis enregistrer.\nChoix applique apres redemarrage." :
     "Choisir la borne puis enregistrer.\nDeye LoRa indisponible pour ce modele.") : inverter_profile().ev_supported ?
    "Choix applique apres enregistrement et redemarrage." :
    "Vetronic WB01 disponible. Deye LoRa indisponible\npour ce modele. Choix applique apres redemarrage.",
    0, v2_row_y, 420);
  v2_save_button();
}
#include "ui_inverter_model.h"

static void v2_menu_item(const char *name, lv_event_cb_t action) {
  ui_settings_make_button(v2_body, name, 0, v2_row_y, 420, 56, action);
  v2_row_y += 72;
}
void ui_show_v2_deye(lv_event_t *) {
  v2_new_page("DEYE / SOLARMAN", ui_show_settings_screen);
  v2_menu_item("MODELE DEYE", ui_show_inverter_model);
  v2_menu_item("CONNEXION / LOGGER", ui_show_deye_screen);
  v2_menu_item("PRODUCTION PV / GEN", ui_show_v2_sources);
  v2_menu_item("REGISTRES PERSO", ui_show_registers);
}
void ui_show_v2_network(lv_event_t *) {
  v2_new_page("RESEAU / HEURE", ui_show_settings_screen);
  v2_menu_item("WIFI / RESEAU", ui_show_wifi_screen);
  v2_menu_item("HEURE / NTP", ui_show_ntp_screen);
}
void ui_show_v2_display(lv_event_t *) {
  v2_new_page("AFFICHAGE", ui_show_settings_screen);
  v2_menu_item("THEME / LUMINOSITE", ui_show_theme_screen);
  v2_menu_item("VEILLE HORAIRE", ui_show_v2_sleep);
}
void ui_show_v2_vehicle(lv_event_t *) {
  v2_new_page("VEHICULE ELECTRIQUE", ui_show_settings_screen);
  if (ev_deye_enabled()) v2_menu_item("RECHARGE DEYE LORA", ui_show_ev_charger);
  else if (ev_vetronic_enabled()) v2_menu_item("VE TRONIC WB01", ui_show_ev_charger);
  v2_menu_item("ACTIVATION / TARIFS", ui_show_v2_ev_options);
  if (!cfg_ev_charger_enabled) v2_label(v2_body, cfg_ev_backend_needs_selection ?
    "Choisir la borne puis enregistrer." : "Aucune borne selectionnee", 0, v2_row_y, 420);
}
void ui_show_ev_charger(lv_event_t *event) {
  if (ev_deye_enabled()) ui_show_ve_deye(event);
  else if (ev_vetronic_enabled()) ui_show_vetronic(event);
  else ui_show_v2_ev_options(event);
}
static void ui_v2_update() {
  if (!v2_screen || lv_scr_act() != v2_screen || !v2_status) return;
  const auto m = v2_measure_snapshot();
  char text[80];
  if (m.relay.valid(millis())) snprintf(text, sizeof(text), "Relais : %s   Valeur : %.2f\nDerniere lecture : %lu s", v2_relay.output ? "ON" : "OFF", m.relay.value, (unsigned long)(uint32_t(millis() - m.relay.at) / 1000));
  else snprintf(text, sizeof(text), "Relais : OFF   Mesure indisponible");
  lv_label_set_text(v2_status, text);
}
