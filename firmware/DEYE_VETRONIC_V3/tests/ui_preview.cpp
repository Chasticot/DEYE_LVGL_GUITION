#define DEYE_V2_UI_TEST
#include <lvgl.h>
#include <assert.h>
#include "../v2_logic.h"
#include "../inverter_profiles.h"
#include "../app_data.h"
#include "../ui_theme.h"
template<class T> static T min(T a, T b) { return a < b ? a : b; }
template<class T> static T max(T a, T b) { return a > b ? a : b; }
struct String {
  char text[16];
  String(const char *value) { snprintf(text, sizeof(text), "%s", value); }
  const char *c_str() const { return text; }
};
#include "../vetronic_data.h"
#define VETRONIC_MAX_CURRENT_A 32
#define WL_CONNECTED 3
static struct { int status() { return WL_CONNECTED; } } WiFi;
static String cfg_vetronic_host = "192.168.1.130";
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
static VtSnapshot preview_vt;
static int preview_submitted = 0, preview_soc_submitted = 0;
static VtMode preview_mode = VT_UNKNOWN;
static VtSnapshot vt_snapshot() { return preview_vt; }
static bool vt_submit(VtMode mode, int) { ++preview_submitted; preview_mode = mode; return true; }
static bool vt_submit_soc_guard(bool, int stop, int resume) { ++preview_soc_submitted; return vt_soc_guard_valid(stop, resume); }
static bool vt_save_host(const char *host) { cfg_vetronic_host = host; return true; }
#include "../ui_vetronic.h"
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
  preview_vt.online = true; preview_vt.measured = true; preview_vt.soc_control_api = true;
  preview_vt.amps = 10; preview_vt.target = 10; preview_vt.limit = 32; preview_vt.mode = VT_MANUAL;
  ui_show_vetronic(nullptr); ui_vetronic_update(); snapshot("vetronic.bmp");
  assert(vt_current_draft == 10);
  lv_event_send(vt_current_buttons[1], LV_EVENT_CLICKED, nullptr); assert(vt_current_draft == 11);
  lv_event_send(vt_current_buttons[0], LV_EVENT_CLICKED, nullptr); assert(vt_current_draft == 10);
  assert(preview_submitted == 0); // Adjusting the current alone never sends a command.
  lv_event_send(vt_mode_buttons[VT_LEGACY], LV_EVENT_CLICKED, nullptr);
  assert(preview_mode == VT_LEGACY && preview_submitted == 1);
  preview_vt.mode = VT_LEGACY; preview_vt.target = -1; ui_vetronic_update();
  assert(strstr(lv_label_get_text(vt_setpoint_label), "Main rendue"));
  lv_obj_update_layout(screen_vetronic);
  auto return_label = lv_obj_get_child(vt_mode_buttons[VT_LEGACY], 0);
  lv_area_t label_bounds, button_bounds;
  lv_obj_get_coords(return_label, &label_bounds); lv_obj_get_coords(vt_mode_buttons[VT_LEGACY], &button_bounds);
  assert(label_bounds.x1 >= button_bounds.x1 && label_bounds.x2 <= button_bounds.x2);
  assert(label_bounds.y1 >= button_bounds.y1 && label_bounds.y2 <= button_bounds.y2);
  snapshot("main-rendue.bmp");
  lv_event_send(vt_soc_buttons[0], LV_EVENT_CLICKED, nullptr);
  assert(vt_soc_stop_draft == 29 && vt_soc_resume_draft == 35 && preview_soc_submitted == 0);
  lv_event_send(vt_soc_save_button, LV_EVENT_CLICKED, nullptr); assert(preview_soc_submitted == 1);
  preview_vt.manual_allowed = false; ui_vetronic_update();
  assert(lv_obj_has_state(vt_mode_buttons[VT_MANUAL], LV_STATE_DISABLED));
  assert(!lv_obj_has_state(vt_mode_buttons[VT_LEGACY], LV_STATE_DISABLED));
  preview_vt.busy = true; ui_vetronic_update();
  for (auto button : vt_mode_buttons) assert(lv_obj_has_state(button, LV_STATE_DISABLED));
  preview_vt.busy = false; preview_vt.online = false; preview_vt.measured = false; ui_vetronic_update();
  assert(lv_obj_has_state(vt_mode_buttons[VT_LEGACY], LV_STATE_DISABLED));
  assert(!lv_obj_has_state(vt_mode_buttons[VT_STOP], LV_STATE_DISABLED));
  assert(strstr(lv_label_get_text(vt_measure_label), "-- A"));
  snapshot("vetronic-hors-ligne.bmp");
  preview_vt.online = true; preview_vt.measured = true; preview_vt.manual_allowed = true;
  vt_ui_open_address(nullptr); snapshot("vetronic-reseau.bmp");
  lv_event_send(vt_address_textarea, LV_EVENT_FOCUSED, nullptr); snapshot("vetronic-clavier.bmp");
  vt_ui_close_address(nullptr);
  cfg_ui_theme = UI_THEME_LIGHT;
  ui_show_v2_sleep(nullptr); check_form(); snapshot("veille-clair.bmp");
  lv_obj_del(screen_vetronic); screen_vetronic = nullptr;
  vt_draft_dirty = false; vt_soc_dirty = false; vt_ui_pending = false;
  ui_show_vetronic(nullptr); snapshot("vetronic-clair.bmp");
  puts("PASS: LVGL V3/WB01 pages, bounds, return control, current/SOC drafts, tariffs, busy/offline states");
}
