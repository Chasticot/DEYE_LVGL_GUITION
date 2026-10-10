#pragma once
#include <lvgl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void ui_gen_footer_layout(lv_obj_t *status, lv_obj_t *temperatures, bool show_daily) {
  lv_obj_t *card = lv_obj_get_parent(status);
  lv_obj_set_style_pad_top(card, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(card, 8, LV_PART_MAIN);
  lv_obj_set_style_text_font(status,
    show_daily ? &lv_font_montserrat_14 : &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_set_width(status, show_daily ? 230 : 175);
  lv_obj_set_width(temperatures, show_daily ? 200 : 270);
  lv_obj_align(status, LV_ALIGN_LEFT_MID, 3, 0);
  lv_obj_align(temperatures, LV_ALIGN_RIGHT_MID, -3, 0);
}

static void ui_gen_footer_append_daily(char *text, size_t capacity, bool valid, float kwh) {
  char energy[24] = "--";
  if (valid && isfinite(kwh) && kwh >= 0) {
    snprintf(energy, sizeof(energy), "%.1f", kwh);
    const size_t length = strlen(energy);
    if (length >= 2 && strcmp(energy + length - 2, ".0") == 0) energy[length - 2] = 0;
  }
  const size_t length = strlen(text);
  if (length < capacity) snprintf(text + length, capacity - length, " / %s kWh", energy);
}
