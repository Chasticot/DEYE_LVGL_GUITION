#define DEYE_V2_UI_TEST
#include <lvgl.h>
#include <assert.h>
#include "../v2_logic.h"
#include "../inverter_profiles.h"
#include "../app_data.h"
#include "../ui_theme.h"
#include "../ve_deye.h"
#include "../ui_v2_navigation.h"

static V2Config cfg_v2;
static bool cfg_tempo_enabled = false, cfg_tempo_colorblind_mode = false, cfg_ev_charger_enabled = true;
static UiThemeId cfg_ui_theme = UI_THEME_DARK;
static struct TestClock { bool load() const { return true; } } ntp_received;
static V2RelayState v2_relay;
static bool v2_save(const V2Config &) { return true; }
static bool settings_save_tempo(bool, bool, bool) { return true; }
static bool settings_get_gen_mode() { return true; }
static bool settings_set_gen_mode(bool) { return true; }
static bool settings_save_model(uint16_t index) { return index < DEYE_PROFILE_COUNT && deye_profiles[index].available; }
static uint32_t millis() { return lv_tick_get(); }
struct TestMeasurements { V2RelaySample relay; };
static TestMeasurements v2_measure_snapshot() { return {{true, 85, millis()}}; }
struct TestTariff { bool valid; bool hc; bool red; bool grid_allowed; };
static TestTariff v2_tariff() { return {true, false, true, false}; }
struct TestEvRegisters { uint16_t mode_register = 489; uint16_t max_power_register = 490; } cfg_ev_registers;
static const UiThemePalette &ui_settings_theme() { return ui_theme_palette(cfg_ui_theme); }
static void ui_settings_make_title(lv_obj_t *p, const char *text) {
  auto label = lv_label_create(p); lv_label_set_text(label, text);
  lv_obj_set_pos(label, 0, 12); lv_obj_set_width(label, 480);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(ui_settings_theme().accent), 0);
}
static lv_obj_t *ui_settings_make_button(lv_obj_t *p, const char *text, int x, int y, int w, int h, lv_event_cb_t cb) {
  auto button = lv_btn_create(p); lv_obj_set_pos(button, x, y); lv_obj_set_size(button, w, h);
  lv_obj_set_style_bg_color(button, lv_color_hex(ui_settings_theme().accent), 0);
  lv_obj_set_style_radius(button, 8, 0); lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, nullptr);
  auto label = lv_label_create(button); lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(ui_settings_theme().accent_text), 0);
  lv_obj_center(label); return button;
}
void ui_show_settings_screen(lv_event_t *) {}
void ui_show_dashboard(lv_event_t *) {}
void ui_show_deye_screen(lv_event_t *) {}
void ui_show_registers(lv_event_t *) {}
void ui_show_wifi_screen(lv_event_t *) {}
void ui_show_ntp_screen(lv_event_t *) {}
void ui_show_theme_screen(lv_event_t *) {}
void deye_solarman_set_ui_active(bool) {}
bool deye_ev_inverter_profile_verified() { return true; }
bool deye_copy_ev_snapshot(EvDeyeData *out) {
  *out = {}; out->valid = true; out->mode_raw = 0x5f01; out->max_charge_power_raw = 3200; return true;
}
bool deye_copy_snapshot(DashboardData *out, uint16_t *, bool *, uint16_t *, uint16_t *, uint16_t *, bool *) {
  *out = {}; out->valid = true; out->grid_power = -1250; out->load_power = 2400; return true;
}
bool deye_submit_ev_command(EvDeyeCommand) { return true; }
#include "../ui_ve_deye.h"
#include "../ui_v2.h"

static lv_color_t pixels[480 * 480], draw_buffer[480 * 40];
static void flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *colors) {
  for (int y = area->y1; y <= area->y2; ++y)
    for (int x = area->x1; x <= area->x2; ++x) pixels[y * 480 + x] = *colors++;
  lv_disp_flush_ready(driver);
}
static void snapshot(const char *name) {
  lv_tick_inc(500); lv_timer_handler();
  lv_obj_update_layout(lv_scr_act()); lv_obj_invalidate(lv_scr_act()); lv_refr_now(nullptr);
  FILE *file = fopen(name, "wb"); assert(file);
  uint8_t header[54] = {'B','M'};
  const uint32_t size = 54 + 480 * 480 * 3, offset = 54, dib = 40, width = 480, height = 480;
  memcpy(header + 2, &size, 4); memcpy(header + 10, &offset, 4); memcpy(header + 14, &dib, 4);
  memcpy(header + 18, &width, 4); memcpy(header + 22, &height, 4); header[26] = 1; header[28] = 24;
  fwrite(header, 1, 54, file);
  for (int y = 479; y >= 0; --y) for (int x = 0; x < 480; ++x) {
    lv_color32_t c; c.full = lv_color_to32(pixels[y * 480 + x]);
    uint8_t rgb[] = {c.ch.blue, c.ch.green, c.ch.red}; fwrite(rgb, 1, 3, file);
  }
  fclose(file);
}
static void check_form() {
  lv_obj_update_layout(v2_screen);
  for (uint8_t i = 0; i < v2_field_count; ++i) {
    const V2Field &field = v2_fields[i];
    lv_area_t bounds; lv_obj_get_coords(field.object, &bounds);
    assert(bounds.x1 >= 20 && bounds.x2 < 460);
  }
}
int main() {
  lv_init();
  static lv_disp_draw_buf_t buffer; lv_disp_draw_buf_init(&buffer, draw_buffer, nullptr, 480 * 40);
  static lv_disp_drv_t driver; lv_disp_drv_init(&driver);
  driver.hor_res = 480; driver.ver_res = 480; driver.draw_buf = &buffer; driver.flush_cb = flush; lv_disp_drv_register(&driver);
  ui_show_v2_deye(nullptr); snapshot("menu-deye.bmp");
  ui_show_inverter_model(nullptr); snapshot("modele-deye.bmp");
  assert(lv_dropdown_get_option_cnt(model_dropdown) == DEYE_PROFILE_COUNT);
  lv_dropdown_open(model_dropdown); snapshot("modeles-liste.bmp");
  lv_dropdown_close(model_dropdown);
  for (unsigned i = 0; i < DEYE_PROFILE_COUNT; ++i) {
    lv_dropdown_set_selected(model_dropdown, i);
    lv_event_send(model_dropdown, LV_EVENT_VALUE_CHANGED, nullptr);
    assert(lv_obj_has_state(model_save_button, LV_STATE_DISABLED) == !deye_profiles[i].available);
  }
  cfg_inverter_model = 2;
  ui_show_v2_sources(nullptr); check_form(); snapshot("sources-hp3.bmp");
  assert(v2_field_count == 5);
  cfg_inverter_model = 3;
  ui_show_v2_sources(nullptr); check_form(); snapshot("sources-aiw51.bmp");
  assert(v2_field_count == 2);
  ui_show_v2_ev_options(nullptr); assert(v2_field_count == 0);
  cfg_inverter_model = 0;
  ui_show_v2_sources(nullptr); check_form(); snapshot("sources.bmp");
  assert(v2_sources_page && v2_gen_mode_draft == 0);
  auto gen_selector = lv_obj_get_child(v2_body, 1);
  lv_btnmatrix_set_selected_btn(gen_selector, 1);
  lv_event_send(gen_selector, LV_EVENT_VALUE_CHANGED, nullptr);
  assert(v2_gen_mode_draft == 1);
  snapshot("sources-gen-mo.bmp");
  ui_show_v2_tariffs(nullptr); check_form(); snapshot("tarifs.bmp");
  ui_show_v2_relay(nullptr); ui_v2_update(); check_form(); snapshot("relais.bmp");
  assert(strstr(lv_label_get_text(v2_rule), "> seuil"));
  lv_dropdown_set_selected(v2_fields[4].object, 0);
  lv_event_send(v2_fields[4].object, LV_EVENT_VALUE_CHANGED, nullptr);
  assert(strstr(lv_label_get_text(v2_rule), "< seuil"));
  lv_obj_scroll_to_view(v2_fields[4].object, LV_ANIM_OFF);
  snapshot("relais-comparateur.bmp");
  ui_show_v2_sleep(nullptr); check_form(); snapshot("veille.bmp");
  lv_event_send(v2_fields[1].object, LV_EVENT_FOCUSED, nullptr); snapshot("veille-clavier.bmp");
  lv_textarea_set_text(v2_fields[1].object, "25:10"); assert(!v2_parse_form());
  lv_textarea_set_text(v2_fields[1].object, "23:10"); assert(v2_parse_form());
  ui_show_v2_ev_options(nullptr); check_form(); snapshot("options-ve.bmp");
  ui_show_ve_deye(nullptr); ui_ve_deye_update(); snapshot("recharge-ve.bmp");
  lv_event_send(ve_power_plus, LV_EVENT_CLICKED, nullptr); assert(ve_draft_w == 3300);
  lv_event_send(ve_power_minus, LV_EVENT_CLICKED, nullptr); assert(ve_draft_w == 3200);
  cfg_ui_theme = UI_THEME_LIGHT;
  ui_show_v2_sleep(nullptr); check_form(); snapshot("veille-clair.bmp");
  puts("PASS: LVGL pages rendered, control bounds, time input validation, VE +/- controls");
}
