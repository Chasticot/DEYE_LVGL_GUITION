#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include "config.h"

static bool gt911_available = false;
static int16_t touch_x = LCD_W / 2;
static int16_t touch_y = LCD_H / 2;

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

static void gt911_clear_status() {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write((uint8_t)(GT911_STATUS_REG >> 8));
  Wire.write((uint8_t)(GT911_STATUS_REG & 0xFF));
  Wire.write((uint8_t)0x00);
  Wire.endTransmission(true);
}

static void touch_gt911_begin() {
  Wire.begin(GT911_SDA, GT911_SCL);
  Wire.setClock(100000);
  delay(100);

  Wire.beginTransmission(GT911_ADDR);
  gt911_available = (Wire.endTransmission() == 0);

  DBG.println(gt911_available ? "GT911 OK" : "GT911 non detecte");
}

static bool touch_gt911_read() {
  if (!gt911_available) return false;

  uint8_t status = 0;
  if (!gt911_read_register(GT911_STATUS_REG, &status, 1)) return false;

  if ((status & 0x80) == 0 || (status & 0x0F) == 0) {
    if (status & 0x80) gt911_clear_status();
    return false;
  }

  uint8_t point[8];
  if (!gt911_read_register(GT911_POINT1_REG, point, 8)) {
    gt911_clear_status();
    return false;
  }

  uint16_t raw_x = (uint16_t)point[1] | ((uint16_t)point[2] << 8);
  uint16_t raw_y = (uint16_t)point[3] | ((uint16_t)point[4] << 8);

  touch_x = constrain((int16_t)raw_y, 0, LCD_W - 1);
  touch_y = constrain((int16_t)(LCD_H - 1 - raw_x), 0, LCD_H - 1);

  gt911_clear_status();
  return true;
}

static void lvgl_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  (void)drv;

  bool pressed = touch_gt911_read();
  data->point.x = touch_x;
  data->point.y = touch_y;
  data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
