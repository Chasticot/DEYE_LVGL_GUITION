#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <SPI.h>
#include <esp_heap_caps.h>

#include "config.h"
#include "ui_registres_perso.h"

Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED,
  39,
  48,
  47,
  GFX_NOT_DEFINED
);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  18, 17, 16, 21,
  11, 12, 13, 14, 0,
  8, 20, 3, 46, 9, 10,
  4, 5, 6, 7, 15,
  1, 10, 8, 50,
  1, 10, 8, 20
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  LCD_W,
  LCD_H,
  rgbpanel,
  0,
  true,
  bus,
  GFX_NOT_DEFINED,
  st7701_type9_init_operations,
  sizeof(st7701_type9_init_operations)
);

#include "app_data.h"
#include "settings.h"
#include "touch_gt911.h"
#include "wifi_manager.h"
#include "ntp_manager.h"
#include "deye_solarman.h"
#include "ui_main.h"
#include "ui_settings.h"
#include "ui_ve_deye.h"

static lv_color_t *draw_buf = nullptr;
static lv_disp_draw_buf_t lv_draw_buf;
static lv_disp_drv_t lv_disp_drv;
static lv_indev_drv_t lv_indev_drv;

void lvgl_flush_cb(
  lv_disp_drv_t *disp,
  const lv_area_t *area,
  lv_color_t *color_p
) {
  uint32_t width = area->x2 - area->x1 + 1;
  uint32_t height = area->y2 - area->y1 + 1;

  gfx->draw16bitRGBBitmap(
    area->x1,
    area->y1,
    (uint16_t *)&color_p->full,
    width,
    height
  );

  lv_disp_flush_ready(disp);
}


// =============================================
// SETUP
// =============================================
void setup() {
  DBG.begin(115200);
  delay(300);
  DBG.println();
  DBG.println("=== DEYE LVGL V3 - UI ONLY ===");

  if (!psramFound()) {
    DBG.println("ERREUR : PSRAM absente ou desactivee (OPI PSRAM requis).");
    while (true) delay(1000);
  }
  DBG.printf("PSRAM detectee : %u octets\n", ESP.getPsramSize());

  settings_load();

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  if (!gfx->begin()) {
    DBG.println("ERREUR : initialisation LCD");
    while (true) delay(1000);
  }

  gfx->setRotation(0);
  gfx->fillScreen(0x0000);
  touch_gt911_begin();

  lv_init();

  draw_buf = (lv_color_t *)heap_caps_malloc(
    DRAW_BUF_PIXELS * sizeof(lv_color_t),
    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
  );

  if (draw_buf == nullptr) {
    DBG.println("Buffer LVGL PSRAM indisponible, essai RAM interne...");
    draw_buf = (lv_color_t *)heap_caps_malloc(
      DRAW_BUF_PIXELS * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    );
  }
  if (draw_buf == nullptr) {
    DBG.println("ERREUR : allocation buffer LVGL impossible");
    while (true) delay(1000);
  }

  lv_disp_draw_buf_init(
    &lv_draw_buf,
    draw_buf,
    nullptr,
    DRAW_BUF_PIXELS
  );

  lv_disp_drv_init(&lv_disp_drv);
  lv_disp_drv.hor_res = LCD_W;
  lv_disp_drv.ver_res = LCD_H;
  lv_disp_drv.flush_cb = lvgl_flush_cb;
  lv_disp_drv.draw_buf = &lv_draw_buf;
  lv_disp_drv_register(&lv_disp_drv);

  lv_indev_drv_init(&lv_indev_drv);
  lv_indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv.read_cb = lvgl_touch_read_cb;
  lv_indev_drv_register(&lv_indev_drv);

  ui_main_create();
  
  if (screen_main != nullptr) {
    lv_scr_load(screen_main);
    lv_refr_now(nullptr);
    DBG.println("Ecran principal charge");
  }

  wifi_manager_begin();
  ntp_manager_begin();
  deye_solarman_begin();

  DBG.println("Interface LVGL prete");
}

// =============================================
// LOOP - ULTRA SIMPLIFIÉE
// =============================================
void loop() {
  static uint32_t last_display_update = 0;
  
  // LVGL Timer Handler - PRIORITAIRE
  lv_tick_inc(1);
  lv_timer_handler();
  wifi_manager_process();
  
  // Mise à jour de l'affichage - toutes les 500ms
  uint32_t now = millis();
  if (now - last_display_update >= 500) {
    last_display_update = now;
    ui_main_update();
    ui_ve_deye_update();
  }

  delay(1);
}
