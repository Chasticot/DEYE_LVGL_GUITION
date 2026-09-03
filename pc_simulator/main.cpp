#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <lvgl.h>
#include <sdl/sdl.h>

#include "Arduino.h"
#include "WiFi.h"
#include "Preferences.h"

void deye_solarman_set_ui_active(bool) {}
void deye_solarman_set_touch_active(bool) {}

#include "../app_data.h"

bool deye_copy_snapshot(
  DashboardData *out,
  uint16_t *pv_daily,
  bool *pv_daily_valid,
  uint16_t *daily_load,
  uint16_t *daily_buy,
  uint16_t *daily_sell,
  bool *on_grid
) {
  *out = dashboard_data;
  *pv_daily = 84;
  *pv_daily_valid = true;
  *daily_load = 127;
  *daily_buy = 0;
  *daily_sell = 0;
  *on_grid = true;
  return true;
}

#include "../settings.h"
#include "../ui_main.h"
#include "../ui_settings.h"

static void install_simulated_data() {
  dashboard_data.valid = true;
  dashboard_data.pv1_w = 3366;
  dashboard_data.pv2_w = 3203;
  dashboard_data.pv3_w = 0;
  dashboard_data.battery_soc = 47;
  dashboard_data.battery_voltage = 52.66f;
  dashboard_data.battery_power = 303;
  dashboard_data.battery_temperature = 28.0f;
  dashboard_data.grid_power = 0;
  dashboard_data.load_power = 0;
  dashboard_data.ups_power = 6472;
  dashboard_data.dc_temperature = 0.0f;
  dashboard_data.ac_temperature = 0.0f;
  dashboard_data.smartload_on = true;
}

int main() {
  SDL_SetMainReady();
  lv_init();
  sdl_init();
  static lv_color_t draw_buffer[480 * 40];
  static lv_disp_draw_buf_t draw_buffer_descriptor;
  lv_disp_draw_buf_init(&draw_buffer_descriptor, draw_buffer, nullptr, 480 * 40);

  lv_disp_drv_t display_driver;
  lv_disp_drv_init(&display_driver);
  display_driver.hor_res = 480;
  display_driver.ver_res = 480;
  display_driver.flush_cb = sdl_display_flush;
  display_driver.draw_buf = &draw_buffer_descriptor;
  lv_disp_drv_register(&display_driver);

  lv_indev_drv_t pointer_driver;
  lv_indev_drv_init(&pointer_driver);
  pointer_driver.type = LV_INDEV_TYPE_POINTER;
  pointer_driver.read_cb = sdl_mouse_read;
  lv_indev_drv_register(&pointer_driver);

  settings_load();
  install_simulated_data();
  ui_main_create();
  ui_settings_create();
  ui_registers_create();
  lv_scr_load(screen_main);

  while (true) {
    lv_tick_inc(5);
    ui_main_update();
    lv_timer_handler();
    SDL_Delay(5);
  }
}
