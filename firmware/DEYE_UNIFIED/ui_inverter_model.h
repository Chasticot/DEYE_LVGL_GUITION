#pragma once
static lv_obj_t *model_dropdown = nullptr, *model_description = nullptr, *model_save_button = nullptr;
static void ui_model_description(lv_event_t *) {
  const auto index = lv_dropdown_get_selected(model_dropdown);
  if (index >= DEYE_PROFILE_COUNT) return;
  const auto &p = deye_profiles[index];
  char text[420];
  snprintf(text, sizeof(text), "%s\n\n%s\n%s\n%s\n\n%s",
    p.experimental ? "Profil experimental, mesures a confirmer." : "Reglages adaptes automatiquement au modele.",
    p.gen_supported ? "Production PV et port GEN" : "Production PV, sans port GEN",
    p.ev_supported ? "Recharge VE disponible" : "Recharge VE non prise en charge",
    p.available ? p.note : "",
    p.available ? "Enregistrer redemarre l'ecran. Les reglages\npersonnels de chaque modele sont conserves." : p.note);
  lv_label_set_text(model_description, text);
  if (p.available) lv_obj_clear_state(model_save_button, LV_STATE_DISABLED);
  else lv_obj_add_state(model_save_button, LV_STATE_DISABLED);
}
static void ui_save_inverter_model(lv_event_t *) {
  const auto index = lv_dropdown_get_selected(model_dropdown);
  if (!settings_save_model(index)) {
    auto message = lv_msgbox_create(nullptr, "Modele Deye", "Modele indisponible ou sauvegarde impossible.", nullptr, true);
    lv_obj_center(message); return;
  }
  auto message = lv_msgbox_create(nullptr, "Modele Deye", "Modele enregistre. Redemarrage...", nullptr, false);
  lv_obj_center(message);
  lv_timer_create([](lv_timer_t *timer) { lv_timer_del(timer); ESP.restart(); }, 900, nullptr);
}
void ui_show_inverter_model(lv_event_t *) {
  v2_new_page("MODELE DEYE", ui_show_v2_deye, true);
  v2_label(v2_body, "Choisissez la reference de votre onduleur", 0, 0, 420);
  model_dropdown = lv_dropdown_create(v2_body);
  lv_obj_set_pos(model_dropdown, 0, 38); lv_obj_set_size(model_dropdown, 420, 48);
  lv_dropdown_clear_options(model_dropdown);
  for (const auto &profile : deye_profiles) lv_dropdown_add_option(model_dropdown, profile.name, LV_DROPDOWN_POS_LAST);
  lv_dropdown_set_selected(model_dropdown, cfg_inverter_model);
  lv_obj_set_style_text_font(model_dropdown, &lv_font_montserrat_16, 0);
  lv_obj_set_style_bg_color(model_dropdown, lv_color_hex(ui_settings_theme().control_bg), 0);
  lv_obj_set_style_text_color(model_dropdown, lv_color_hex(ui_settings_theme().text), 0);
  auto list = lv_dropdown_get_list(model_dropdown);
  lv_obj_set_style_max_height(list, 260, 0);
  lv_obj_set_style_text_font(list, &lv_font_montserrat_16, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex(ui_settings_theme().control_bg), 0);
  lv_obj_set_style_text_color(list, lv_color_hex(ui_settings_theme().text), 0);
  model_description = v2_label(v2_body, "", 0, 104, 420);
  model_save_button = ui_settings_make_button(v2_screen, LV_SYMBOL_SAVE " ENREGISTRER", 230, 422, 230, 44, ui_save_inverter_model);
  lv_obj_add_event_cb(model_dropdown, ui_model_description, LV_EVENT_VALUE_CHANGED, nullptr);
  ui_model_description(nullptr);
}
