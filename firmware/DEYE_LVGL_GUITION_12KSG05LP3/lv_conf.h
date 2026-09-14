#ifndef LV_CONF_H
#define LV_CONF_H

/* ESP32-S3 Guition ESP32-4848S040 / LVGL 8 */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

/* loop() calls lv_tick_inc(1), therefore no custom tick source is used. */
#define LV_TICK_CUSTOM 0
#define LV_DISP_DEF_REFR_PERIOD 15
#define LV_INDEV_DEF_READ_PERIOD 15

/* Use the board's 8 MiB PSRAM for LVGL objects. */
#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_INCLUDE <esp_heap_caps.h>
#define LV_MEM_CUSTOM_ALLOC(size) heap_caps_malloc((size), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
#define LV_MEM_CUSTOM_FREE(ptr) heap_caps_free((ptr))
#define LV_MEM_CUSTOM_REALLOC(ptr, size) heap_caps_realloc((ptr), (size), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)

#define LV_DRAW_COMPLEX 1
#define LV_LAYER_SIMPLE_BUF_SIZE (8 * 1024)
#define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE (2 * 1024)

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_38 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#endif
