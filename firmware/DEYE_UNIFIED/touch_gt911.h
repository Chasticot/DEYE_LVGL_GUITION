#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include "config.h"
#include "display_manager.h"

static bool gt911_available = false;
static int16_t touch_x = LCD_W / 2;
static int16_t touch_y = LCD_H / 2;

// ==================== GESTION DE L'ACTIVITÉ TACTILE ====================
static bool touch_is_active = false;
static uint32_t touch_last_activity = 0;
#define TOUCH_INACTIVITY_TIMEOUT 2000

// ==================== DÉCLARATION EXTERNE ====================
extern void deye_solarman_set_touch_active(bool active);

static bool gt911_read_register(uint16_t reg, uint8_t *buffer, uint8_t length) {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write((uint8_t)(reg >> 8));
  Wire.write((uint8_t)(reg & 0xFF));

  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint8_t)GT911_ADDR, length, (uint8_t)true) != length) return false;

  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = Wire.read();
  }

  return true;
}

static bool gt911_clear_status() {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write((uint8_t)(GT911_STATUS_REG >> 8));
  Wire.write((uint8_t)(GT911_STATUS_REG & 0xFF));
  Wire.write((uint8_t)0x00);
  return Wire.endTransmission(true) == 0;
}

static void touch_gt911_begin() {
  Wire.begin(GT911_SDA, GT911_SCL);
  Wire.setClock(100000);
  delay(100);

  Wire.beginTransmission(GT911_ADDR);
  gt911_available = (Wire.endTransmission() == 0);

  DBG.println(gt911_available ? "GT911 OK" : "GT911 non detecte");
}

static TouchSample touch_gt911_read() {
  if (!gt911_available) return TouchSample::Invalid;

  uint8_t status = 0;
  if (!gt911_read_register(GT911_STATUS_REG, &status, 1)) return TouchSample::Invalid;

  const TouchSample sample = gt911_status_sample(status);
  if (sample == TouchSample::Pending) return sample;
  if (sample != TouchSample::Pressed) {
    return gt911_clear_status() ? sample : TouchSample::Invalid;
  }

  uint8_t point[8];
  if (!gt911_read_register(GT911_POINT1_REG, point, 8)) {
    gt911_clear_status();
    return TouchSample::Invalid;
  }

  // Rotation 0: use raw coordinates only after validating the frame.
  int16_t x = 0, y = 0;
  const bool valid = gt911_decode_point(point, LCD_W, LCD_H, x, y);
  // A failed acknowledgement can leave the same frame ready on every poll.
  if (!gt911_clear_status() || !valid) return TouchSample::Invalid;
  touch_x = x;
  touch_y = y;
  return TouchSample::Pressed;
}

// ==================== GESTION DE L'ACTIVITÉ TACTILE ====================

static void touch_set_active() {
  touch_is_active = true;
  touch_last_activity = millis();
}

bool is_touch_active() {
  uint32_t now = millis();
  if (touch_is_active) {
    if (now - touch_last_activity > TOUCH_INACTIVITY_TIMEOUT) {
      touch_is_active = false;
      deye_solarman_set_touch_active(false);
    }
  }
  return touch_is_active;
}

// ==================== CALLBACK LVGL OPTIMISÉ ====================

static void lvgl_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  (void)drv;

  static bool last_pressed = false;

  const TouchSample sample = touch_gt911_read();
  bool pressed = sample == TouchSample::Pressed;
  if (display_touch(sample)) pressed = false;
  uint32_t now = millis();

  // Pause the reader directly from the touch driver so the UI remains responsive.
  // The pause is released after a real period of inactivity, including after release.
  if (pressed && !last_pressed) {
    touch_set_active();
    deye_solarman_set_touch_active(true);
  } else if (pressed && last_pressed) {
    touch_set_active();
  }

  if (!pressed && touch_is_active && now - touch_last_activity >= TOUCH_INACTIVITY_TIMEOUT) {
    touch_is_active = false;
    deye_solarman_set_touch_active(false);
  }

  last_pressed = pressed;

  data->point.x = touch_x;
  data->point.y = touch_y;
  data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
