#pragma once
#include <lvgl.h>
#include "ev_dashboard_status.h"

struct UiEvBadges { lv_obj_t *cable = nullptr; lv_obj_t *solar = nullptr; };

static inline UiEvBadges ui_ev_badges_create(lv_obj_t *parent) {
  UiEvBadges badges;
  badges.cable = lv_label_create(parent);
  lv_label_set_text(badges.cable, "-");
  lv_obj_set_width(badges.cable, 18);
  lv_obj_set_style_text_font(badges.cable, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_align(badges.cable, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(badges.cable, LV_ALIGN_LEFT_MID, 35, 0);
  lv_obj_clear_flag(badges.cable, LV_OBJ_FLAG_CLICKABLE);
  badges.solar = lv_obj_create(parent);
  lv_obj_set_size(badges.solar, 17, 17);
  lv_obj_set_style_bg_opa(badges.solar, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(badges.solar, 0, 0);
  lv_obj_set_style_pad_all(badges.solar, 0, 0);
  lv_obj_clear_flag(badges.solar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(badges.solar, LV_ALIGN_LEFT_MID, 55, 0);
  const lv_coord_t x[] = {5, 7, 7, 0, 14, 2, 12, 2, 12};
  const lv_coord_t y[] = {5, 0, 14, 7, 7, 2, 2, 12, 12};
  const lv_coord_t size[] = {7, 3, 3, 3, 3, 2, 2, 2, 2};
  for (unsigned i = 0; i < 9; ++i) {
    lv_obj_t *part = lv_obj_create(badges.solar);
    lv_obj_set_size(part, size[i], size[i]);
    lv_obj_set_pos(part, x[i], y[i]);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFACC15), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  }
  lv_obj_add_flag(badges.cable, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(badges.solar, LV_OBJ_FLAG_HIDDEN);
  return badges;
}

static inline void ui_ev_badges_update(const UiEvBadges &badges,
    const EvDashboardIndicators &status) {
  if (status.show_cable) lv_obj_clear_flag(badges.cable, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_add_flag(badges.cable, LV_OBJ_FLAG_HIDDEN);
  const bool connected = status.cable == EvCableState::Connected;
  const bool disconnected = status.cable == EvCableState::Disconnected;
  lv_label_set_text(badges.cable, connected ? LV_SYMBOL_OK : disconnected ? LV_SYMBOL_CLOSE : "-");
  lv_obj_set_style_text_color(badges.cable,
    lv_color_hex(connected ? 0x22C55E : disconnected ? 0xEF4444 : 0x94A3B8), 0);
  if (status.solar) lv_obj_clear_flag(badges.solar, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_add_flag(badges.solar, LV_OBJ_FLAG_HIDDEN);
}
