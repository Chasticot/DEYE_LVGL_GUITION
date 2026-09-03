// ui_registres_perso.h - Version avec colonne Coeff

#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include <math.h>
#include "config.h"
#include "settings.h"

// ==================== DÉCLARATIONS EXTERNES ====================
extern void ui_show_settings_screen(lv_event_t *e);
extern void deye_solarman_set_ui_active(bool active);

// ==================== VARIABLES GLOBALES ====================
lv_obj_t *screen_registers = nullptr;

// Textareas pour les valeurs
static lv_obj_t *ta_pv1_power, *ta_pv2_power, *ta_pv3_power, *ta_pv_daily;
static lv_obj_t *ta_battery_soc, *ta_battery_voltage, *ta_battery_power, *ta_battery_temp;
static lv_obj_t *ta_grid_power, *ta_grid_status, *ta_grid_buy_daily, *ta_grid_sell_daily;
static lv_obj_t *ta_load_power, *ta_ups_power, *ta_load_daily;
static lv_obj_t *ta_dc_temp, *ta_ac_temp, *ta_smartload;
static lv_obj_t *ta_connect_timeout, *ta_response_window, *ta_frame_timeout, *ta_block_interval;

// Textareas pour les coefficients (uniquement pour les 4 registres souhaités)
static lv_obj_t *ta_coeff_grid_power;
static lv_obj_t *ta_coeff_load_power;
static lv_obj_t *ta_coeff_ups_power;
static lv_obj_t *ta_coeff_smartload;

// Clavier global
static lv_obj_t *global_keyboard = nullptr;

// ==================== FONCTIONS DE CALLBACK (sans lambda) ====================

static void textarea_focus_cb(lv_event_t *e) {
  lv_obj_t *ta = lv_event_get_target(e);
  if (global_keyboard != nullptr) {
    lv_keyboard_set_textarea(global_keyboard, ta);
    lv_obj_clear_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(global_keyboard);
  }
}

static void textarea_defocus_cb(lv_event_t *e) {
  lv_obj_t *ta = lv_event_get_target(e);
  (void)ta;
  if (global_keyboard != nullptr) {
    lv_keyboard_set_textarea(global_keyboard, nullptr);
    lv_obj_add_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

// ==================== FONCTION UTILITAIRE ====================

// Ajoute une ligne avec 3 colonnes : label, textarea valeur, textarea coeff (optionnel)
static void add_reg_line(
  lv_obj_t *parent,
  const char *label_text,
  lv_obj_t **textarea_val,
  int default_val,
  lv_obj_t **textarea_coeff,
  float default_coeff,
  int y_pos
) {
  // Label (colonne 1)
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, label_text);
  lv_obj_set_pos(label, 5, y_pos);
  lv_obj_set_width(label, 130);
  lv_obj_set_style_text_color(label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);

  // Textarea Valeur (colonne 2)
  *textarea_val = lv_textarea_create(parent);
  lv_textarea_set_one_line(*textarea_val, true);
  lv_textarea_set_max_length(*textarea_val, 6);
  char buf[10];
  snprintf(buf, sizeof(buf), "%d", default_val);
  lv_textarea_set_text(*textarea_val, buf);

  lv_obj_set_pos(*textarea_val, 145, y_pos - 4);
  lv_obj_set_size(*textarea_val, 55, 32);
  lv_obj_set_style_bg_color(*textarea_val, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(*textarea_val, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(*textarea_val, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
  lv_obj_set_style_border_width(*textarea_val, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(*textarea_val, 4, LV_PART_MAIN);
  lv_obj_set_scrollbar_mode(*textarea_val, LV_SCROLLBAR_MODE_OFF);

  lv_obj_add_event_cb(*textarea_val, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(*textarea_val, textarea_defocus_cb, LV_EVENT_DEFOCUSED, NULL);
  lv_obj_add_event_cb(*textarea_val, textarea_defocus_cb, LV_EVENT_READY, NULL);
  lv_obj_add_event_cb(*textarea_val, textarea_defocus_cb, LV_EVENT_CANCEL, NULL);

  // Si un textarea coeff est fourni, on le crée (colonne 3)
  if (textarea_coeff != nullptr) {
    *textarea_coeff = lv_textarea_create(parent);
    lv_textarea_set_one_line(*textarea_coeff, true);
    lv_textarea_set_max_length(*textarea_coeff, 6);
    char cbuf[10];
    snprintf(cbuf, sizeof(cbuf), "%.1f", default_coeff);
    lv_textarea_set_text(*textarea_coeff, cbuf);

    lv_obj_set_pos(*textarea_coeff, 215, y_pos - 4);
    lv_obj_set_size(*textarea_coeff, 50, 32);
    lv_obj_set_style_bg_color(*textarea_coeff, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
    lv_obj_set_style_text_color(*textarea_coeff, lv_color_hex(0xFFD700), LV_PART_MAIN);
    lv_obj_set_style_border_color(*textarea_coeff, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
    lv_obj_set_style_border_width(*textarea_coeff, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(*textarea_coeff, 4, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(*textarea_coeff, LV_SCROLLBAR_MODE_OFF);

    lv_obj_add_event_cb(*textarea_coeff, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(*textarea_coeff, textarea_defocus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(*textarea_coeff, textarea_defocus_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(*textarea_coeff, textarea_defocus_cb, LV_EVENT_CANCEL, NULL);
  }
}

// ==================== SAUVEGARDE ====================

static void ui_registers_show_error(const char *message) {
  lv_obj_t *msg = lv_msgbox_create(NULL, "Valeur invalide", message, NULL, true);
  lv_obj_center(msg);
}

static bool ui_read_uint16(lv_obj_t *textarea, uint16_t *out) {
  const char *text = lv_textarea_get_text(textarea);
  if (text == nullptr || *text == '\0' || *text == '-') return false;
  char *end = nullptr;
  const unsigned long value = strtoul(text, &end, 10);
  if (end == text || *end != '\0' || value > UINT16_MAX) return false;
  *out = (uint16_t)value;
  return true;
}

static bool ui_read_timeout(lv_obj_t *textarea, uint32_t *out) {
  const char *text = lv_textarea_get_text(textarea);
  if (text == nullptr || *text == '\0' || *text == '-') return false;
  char *end = nullptr;
  const unsigned long value = strtoul(text, &end, 10);
  if (end == text || *end != '\0' || value < 50 || value > 60000) return false;
  *out = (uint32_t)value;
  return true;
}

static bool ui_read_coefficient(lv_obj_t *textarea, float *out) {
  const char *text = lv_textarea_get_text(textarea);
  if (text == nullptr || *text == '\0') return false;
  char *end = nullptr;
  const float value = strtof(text, &end);
  if (end == text || *end != '\0' || !isfinite(value) || value < -100.0f || value > 100.0f) return false;
  *out = value;
  return true;
}

static bool ui_register_blocks_valid(const CustomRegisters &regs) {
  const uint16_t block1[] = {
    regs.grid_buy_daily, regs.grid_sell_daily, regs.load_daily,
    regs.dc_temp, regs.ac_temp, regs.pv_daily
  };
  const uint16_t block2[] = {
    regs.grid_power, regs.ups_power, regs.load_power, regs.battery_temp,
    regs.battery_voltage, regs.battery_soc, regs.pv1_power, regs.pv2_power,
    regs.pv3_power, regs.battery_power, regs.grid_status, regs.smartload
  };
  uint16_t min1 = block1[0], max1 = block1[0], min2 = block2[0], max2 = block2[0];
  for (size_t i = 1; i < sizeof(block1) / sizeof(block1[0]); ++i) {
    if (block1[i] < min1) min1 = block1[i];
    if (block1[i] > max1) max1 = block1[i];
  }
  for (size_t i = 1; i < sizeof(block2) / sizeof(block2[0]); ++i) {
    if (block2[i] < min2) min2 = block2[i];
    if (block2[i] > max2) max2 = block2[i];
  }
  return (uint32_t)max1 - min1 + 1 <= 125 && (uint32_t)max2 - min2 + 1 <= 125;
}

static void ui_registers_save(lv_event_t *e) {
  (void)e;

  CustomRegisters regs;
  #define READ_REGISTER(field, textarea) if (!ui_read_uint16(textarea, &regs.field)) { ui_registers_show_error("Les registres doivent etre entre 0 et 65535."); return; }
  READ_REGISTER(pv1_power, ta_pv1_power);
  READ_REGISTER(pv2_power, ta_pv2_power);
  READ_REGISTER(pv3_power, ta_pv3_power);
  READ_REGISTER(pv_daily, ta_pv_daily);
  READ_REGISTER(battery_soc, ta_battery_soc);
  READ_REGISTER(battery_voltage, ta_battery_voltage);
  READ_REGISTER(battery_power, ta_battery_power);
  READ_REGISTER(battery_temp, ta_battery_temp);
  READ_REGISTER(grid_power, ta_grid_power);
  READ_REGISTER(grid_status, ta_grid_status);
  READ_REGISTER(grid_buy_daily, ta_grid_buy_daily);
  READ_REGISTER(grid_sell_daily, ta_grid_sell_daily);
  READ_REGISTER(load_power, ta_load_power);
  READ_REGISTER(ups_power, ta_ups_power);
  READ_REGISTER(load_daily, ta_load_daily);
  READ_REGISTER(dc_temp, ta_dc_temp);
  READ_REGISTER(ac_temp, ta_ac_temp);
  READ_REGISTER(smartload, ta_smartload);
  #undef READ_REGISTER

  if (!ui_read_timeout(ta_connect_timeout, &regs.connect_timeout) ||
      !ui_read_timeout(ta_response_window, &regs.response_window) ||
      !ui_read_timeout(ta_frame_timeout, &regs.frame_timeout) ||
      !ui_read_timeout(ta_block_interval, &regs.block_interval)) {
    ui_registers_show_error("Les timeouts doivent etre compris entre 50 et 60000 ms.");
    return;
  }
  if (!ui_read_coefficient(ta_coeff_grid_power, &regs.coeff_grid_power) ||
      !ui_read_coefficient(ta_coeff_load_power, &regs.coeff_load_power) ||
      !ui_read_coefficient(ta_coeff_ups_power, &regs.coeff_ups_power) ||
      !ui_read_coefficient(ta_coeff_smartload, &regs.coeff_smartload)) {
    ui_registers_show_error("Les coefficients doivent etre entre -100 et 100.");
    return;
  }
  if (!ui_register_blocks_valid(regs)) {
    ui_registers_show_error("Chaque bloc de lecture doit contenir au maximum 125 registres.");
    return;
  }

  settings_save_registers(regs);

  // Message de confirmation
  static const char *btn_txts[] = {"OK", NULL};
  lv_obj_t *msg = lv_msgbox_create(NULL, "Sauvegarde", "Registres et coefficients sauvegardes !\nRedemarrage en cours...", btn_txts, false);
  lv_obj_center(msg);
  lv_timer_create([](lv_timer_t *timer) {
    lv_timer_del(timer);
    ESP.restart();
  }, 300, NULL);
}

// ==================== CHARGEMENT DES VALEURS ====================

static void ui_registers_load_values() {
  CustomRegisters regs = get_custom_registers();
  char buf[10];

  #define SET_TEXT(ta, val) snprintf(buf, sizeof(buf), "%d", val); lv_textarea_set_text(ta, buf)

  SET_TEXT(ta_pv1_power, regs.pv1_power);
  SET_TEXT(ta_pv2_power, regs.pv2_power);
  SET_TEXT(ta_pv3_power, regs.pv3_power);
  SET_TEXT(ta_pv_daily, regs.pv_daily);
  SET_TEXT(ta_battery_soc, regs.battery_soc);
  SET_TEXT(ta_battery_voltage, regs.battery_voltage);
  SET_TEXT(ta_battery_power, regs.battery_power);
  SET_TEXT(ta_battery_temp, regs.battery_temp);
  SET_TEXT(ta_grid_power, regs.grid_power);
  SET_TEXT(ta_grid_status, regs.grid_status);
  SET_TEXT(ta_grid_buy_daily, regs.grid_buy_daily);
  SET_TEXT(ta_grid_sell_daily, regs.grid_sell_daily);
  SET_TEXT(ta_load_power, regs.load_power);
  SET_TEXT(ta_ups_power, regs.ups_power);
  SET_TEXT(ta_load_daily, regs.load_daily);
  SET_TEXT(ta_dc_temp, regs.dc_temp);
  SET_TEXT(ta_ac_temp, regs.ac_temp);
  SET_TEXT(ta_smartload, regs.smartload);

  // Timeouts
  snprintf(buf, sizeof(buf), "%lu", regs.connect_timeout);
  lv_textarea_set_text(ta_connect_timeout, buf);
  snprintf(buf, sizeof(buf), "%lu", regs.response_window);
  lv_textarea_set_text(ta_response_window, buf);
  snprintf(buf, sizeof(buf), "%lu", regs.frame_timeout);
  lv_textarea_set_text(ta_frame_timeout, buf);
  snprintf(buf, sizeof(buf), "%lu", regs.block_interval);
  lv_textarea_set_text(ta_block_interval, buf);

  // Coefficients
  #define SET_FLOAT(ta, val) snprintf(buf, sizeof(buf), "%.1f", val); lv_textarea_set_text(ta, buf)
  SET_FLOAT(ta_coeff_grid_power, regs.coeff_grid_power);
  SET_FLOAT(ta_coeff_load_power, regs.coeff_load_power);
  SET_FLOAT(ta_coeff_ups_power, regs.coeff_ups_power);
  SET_FLOAT(ta_coeff_smartload, regs.coeff_smartload);
}

// ==================== CRÉATION DE L'ÉCRAN ====================

void ui_registers_create() {
  if (screen_registers != nullptr) return;

  screen_registers = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(screen_registers, lv_color_hex(0x0a0a1a), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen_registers, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(screen_registers, LV_OBJ_FLAG_SCROLLABLE);

  // Titre
  lv_obj_t *title = lv_label_create(screen_registers);
  lv_label_set_text(title, "REGISTRES PERSONNALISES");
  lv_obj_set_pos(title, 0, 12);
  lv_obj_set_width(title, LCD_W);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(title, lv_color_hex(0x55D6FF), LV_PART_MAIN);

  // Entêtes de colonnes
  lv_obj_t *h1 = lv_label_create(screen_registers);
  lv_label_set_text(h1, "Registre");
  lv_obj_set_pos(h1, 10, 52);
  lv_obj_set_style_text_color(h1, lv_color_hex(0x55D6FF), LV_PART_MAIN);
  lv_obj_set_style_text_font(h1, &lv_font_montserrat_14, LV_PART_MAIN);

  lv_obj_t *h2 = lv_label_create(screen_registers);
  lv_label_set_text(h2, "Valeur");
  lv_obj_set_pos(h2, 155, 52);
  lv_obj_set_style_text_color(h2, lv_color_hex(0x55D6FF), LV_PART_MAIN);
  lv_obj_set_style_text_font(h2, &lv_font_montserrat_14, LV_PART_MAIN);

  lv_obj_t *h3 = lv_label_create(screen_registers);
  lv_label_set_text(h3, "Coeff");
  lv_obj_set_pos(h3, 220, 52);
  lv_obj_set_style_text_color(h3, lv_color_hex(0xFFD700), LV_PART_MAIN);
  lv_obj_set_style_text_font(h3, &lv_font_montserrat_14, LV_PART_MAIN);

  // Conteneur défilant
  lv_obj_t *cont = lv_obj_create(screen_registers);
  lv_obj_set_size(cont, LCD_W, 310);
  lv_obj_set_pos(cont, 0, 72);
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(cont, 5, LV_PART_MAIN);
  lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

  int y = 5;
  int step = 34;

  // Registres sans coefficient (colonne Coeff masquée)
  add_reg_line(cont, "PV1 Power", &ta_pv1_power, 186, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "PV2 Power", &ta_pv2_power, 187, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "PV3 Power", &ta_pv3_power, 188, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "PV Daily", &ta_pv_daily, 108, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Battery SOC", &ta_battery_soc, 184, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Battery Volt", &ta_battery_voltage, 183, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Battery Power", &ta_battery_power, 190, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Battery Temp", &ta_battery_temp, 182, nullptr, 1.0f, y); y += step;

  // Registres AVEC coefficient
  add_reg_line(cont, "Grid Power", &ta_grid_power, 169, &ta_coeff_grid_power, 1.0f, y); y += step;
  add_reg_line(cont, "Grid Status", &ta_grid_status, 194, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Grid Buy", &ta_grid_buy_daily, 76, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Grid Sell", &ta_grid_sell_daily, 77, nullptr, 1.0f, y); y += step;

  add_reg_line(cont, "Load Power", &ta_load_power, 178, &ta_coeff_load_power, 1.0f, y); y += step;
  add_reg_line(cont, "UPS Power", &ta_ups_power, 172, &ta_coeff_ups_power, 1.0f, y); y += step;
  add_reg_line(cont, "Load Daily", &ta_load_daily, 84, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "DC Temp", &ta_dc_temp, 90, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "AC Temp", &ta_ac_temp, 91, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "SmartLoad", &ta_smartload, 195, &ta_coeff_smartload, 1.0f, y); y += step;

  // Timeouts (sans coeff)
  y += 10;
  lv_obj_t *timeout_label = lv_label_create(cont);
  lv_label_set_text(timeout_label, "--- TIMEOUTS (ms) ---");
  lv_obj_set_pos(timeout_label, 5, y);
  lv_obj_set_style_text_color(timeout_label, lv_color_hex(0x55D6FF), LV_PART_MAIN);
  y += 30;
  add_reg_line(cont, "Connect", &ta_connect_timeout, 10000, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Response", &ta_response_window, 10000, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Frame", &ta_frame_timeout, 7000, nullptr, 1.0f, y); y += step;
  add_reg_line(cont, "Block Interval", &ta_block_interval, 100, nullptr, 1.0f, y); y += step;

  // Mode GEN (SmartLoad / MO) - Version avec LV_BTNMATRIX_CTRL_CHECKED
  y += 10;
  lv_obj_t *gen_label = lv_label_create(cont);
  lv_label_set_text(gen_label, "Mode GEN :");
  lv_obj_set_pos(gen_label, 5, y);
  lv_obj_set_style_text_color(gen_label, lv_color_hex(0x9CA3AF), LV_PART_MAIN);
  lv_obj_set_style_text_font(gen_label, &lv_font_montserrat_14, LV_PART_MAIN);

  // Créer une ligne avec deux boutons radio (btnmatrix)
  static const char *gen_opts[] = {"SmartLoad", "GEN MO", ""};
  lv_obj_t *gen_btnmatrix = lv_btnmatrix_create(cont);
  lv_btnmatrix_set_map(gen_btnmatrix, gen_opts);
  // Rendre les boutons cliquables et coché
  lv_btnmatrix_set_btn_ctrl(gen_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED);
  lv_btnmatrix_set_btn_ctrl(gen_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
  // Aucun flag "CHECK_STATE" n'existe, on utilise CHECKED pour marquer le bouton sélectionné
  lv_obj_set_pos(gen_btnmatrix, 130, y - 4);
  lv_obj_set_size(gen_btnmatrix, 200, 34);
  lv_obj_set_style_bg_color(gen_btnmatrix, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
  lv_obj_set_style_text_color(gen_btnmatrix, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(gen_btnmatrix, lv_color_hex(0x3a3a5e), LV_PART_MAIN);
  lv_obj_set_style_border_width(gen_btnmatrix, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(gen_btnmatrix, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_all(gen_btnmatrix, 2, LV_PART_MAIN);

  // Récupérer l'état actuel et cocher le bon bouton
  bool is_smartload = settings_get_gen_mode();
  // Par défaut le premier bouton est coché, on ajuste
  if (!is_smartload) {
    // Si mode GEN MO, on décoche le premier et on coche le second
    lv_btnmatrix_clear_btn_ctrl(gen_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(gen_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKED);
  } else {
    // Sinon on s'assure que le premier est coché et le second non
    lv_btnmatrix_set_btn_ctrl(gen_btnmatrix, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_clear_btn_ctrl(gen_btnmatrix, 1, LV_BTNMATRIX_CTRL_CHECKED);
  }

  // Callback lors du clic sur un bouton
  lv_obj_add_event_cb(gen_btnmatrix, [](lv_event_t *e) {
    lv_obj_t *btnm = lv_event_get_target(e);
    uint32_t idx = lv_btnmatrix_get_selected_btn(btnm);
    if (idx == 0) {
      settings_set_gen_mode(true);   // SmartLoad
      // Coche le premier, décoche le second
      lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
      lv_btnmatrix_clear_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
    } else if (idx == 1) {
      settings_set_gen_mode(false);  // GEN MO
      // Coche le second, décoche le premier
      lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
      lv_btnmatrix_clear_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
    }
  }, LV_EVENT_CLICKED, NULL);

  y += 40; // passer à la ligne suivante

  // Espace pour le défilement
  lv_obj_t *spacer = lv_obj_create(cont);
  lv_obj_set_size(spacer, LCD_W, 100);
  lv_obj_set_pos(spacer, 0, y + 20);
  lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(spacer, 0, LV_PART_MAIN);

  // Clavier global
  global_keyboard = lv_keyboard_create(screen_registers);
  lv_keyboard_set_mode(global_keyboard, LV_KEYBOARD_MODE_NUMBER);
  lv_obj_set_size(global_keyboard, LCD_W, 150);
  lv_obj_align(global_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_bg_color(global_keyboard, lv_color_hex(0x1a1a2e), LV_PART_MAIN);

  // Boutons en bas
  lv_obj_t *default_btn = lv_btn_create(screen_registers);
  lv_obj_set_size(default_btn, 110, 40);
  lv_obj_set_pos(default_btn, 20, 432);
  lv_obj_set_style_bg_color(default_btn, lv_color_hex(0x1D4ED8), LV_PART_MAIN);
  lv_obj_set_style_radius(default_btn, 8, LV_PART_MAIN);
  lv_obj_add_event_cb(default_btn, [](lv_event_t *e) {
    (void)e;
    // Remettre les valeurs par défaut
    lv_textarea_set_text(ta_pv1_power, "186");
    lv_textarea_set_text(ta_pv2_power, "187");
    lv_textarea_set_text(ta_pv3_power, "188");
    lv_textarea_set_text(ta_pv_daily, "108");
    lv_textarea_set_text(ta_battery_soc, "184");
    lv_textarea_set_text(ta_battery_voltage, "183");
    lv_textarea_set_text(ta_battery_power, "190");
    lv_textarea_set_text(ta_battery_temp, "182");
    lv_textarea_set_text(ta_grid_power, "169");
    lv_textarea_set_text(ta_grid_status, "194");
    lv_textarea_set_text(ta_grid_buy_daily, "76");
    lv_textarea_set_text(ta_grid_sell_daily, "77");
    lv_textarea_set_text(ta_load_power, "178");
    lv_textarea_set_text(ta_ups_power, "172");
    lv_textarea_set_text(ta_load_daily, "84");
    lv_textarea_set_text(ta_dc_temp, "90");
    lv_textarea_set_text(ta_ac_temp, "91");
    lv_textarea_set_text(ta_smartload, "195");
    lv_textarea_set_text(ta_connect_timeout, "10000");
    lv_textarea_set_text(ta_response_window, "10000");
    lv_textarea_set_text(ta_frame_timeout, "7000");
    lv_textarea_set_text(ta_block_interval, "100");
    // Coefficients par défaut
    lv_textarea_set_text(ta_coeff_grid_power, "1.0");
    lv_textarea_set_text(ta_coeff_load_power, "1.0");
    lv_textarea_set_text(ta_coeff_ups_power, "1.0");
    lv_textarea_set_text(ta_coeff_smartload, "1.0");
  }, LV_EVENT_CLICKED, NULL);
  lv_obj_t *dl = lv_label_create(default_btn);
  lv_label_set_text(dl, "DEFAULT");
  lv_obj_center(dl);
  lv_obj_set_style_text_color(dl, lv_color_white(), LV_PART_MAIN);

  lv_obj_t *save_btn = lv_btn_create(screen_registers);
  lv_obj_set_size(save_btn, 130, 40);
  lv_obj_set_pos(save_btn, 160, 432);
  lv_obj_set_style_bg_color(save_btn, lv_color_hex(0x22C55E), LV_PART_MAIN);
  lv_obj_set_style_radius(save_btn, 8, LV_PART_MAIN);
  lv_obj_add_event_cb(save_btn, ui_registers_save, LV_EVENT_CLICKED, NULL);
  lv_obj_t *sl = lv_label_create(save_btn);
  lv_label_set_text(sl, "SAUVEGARDER");
  lv_obj_center(sl);
  lv_obj_set_style_text_color(sl, lv_color_white(), LV_PART_MAIN);

  lv_obj_t *retour_btn = lv_btn_create(screen_registers);
  lv_obj_set_size(retour_btn, 120, 40);
  lv_obj_set_pos(retour_btn, 320, 432);
  lv_obj_set_style_bg_color(retour_btn, lv_color_hex(0xEF4444), LV_PART_MAIN);
  lv_obj_set_style_radius(retour_btn, 8, LV_PART_MAIN);
  lv_obj_add_event_cb(retour_btn, ui_show_settings_screen, LV_EVENT_CLICKED, NULL);
  lv_obj_t *rl = lv_label_create(retour_btn);
  lv_label_set_text(rl, "RETOUR");
  lv_obj_center(rl);
  lv_obj_set_style_text_color(rl, lv_color_white(), LV_PART_MAIN);

  // Charger les valeurs sauvegardées
  ui_registers_load_values();
}
