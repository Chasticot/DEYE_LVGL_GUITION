#pragma once

#include <Arduino.h>

#define DBG Serial0

#define LCD_W 480
#define LCD_H 480
#define GFX_BL 38

#define GT911_SDA 19
#define GT911_SCL 45
#define GT911_ADDR 0x5D
#define GT911_STATUS_REG 0x814E
#define GT911_POINT1_REG 0x814F

#define DRAW_BUF_PIXELS (LCD_W * 40)

#define DEFAULT_WIFI_SSID "wifi-ssid"
#define DEFAULT_WIFI_PASSWORD "wifi_password"
#define DEFAULT_DEYE_HOST "192.168.1.47"
#define DEFAULT_LOGGER_SERIAL 0123456789
#define DEFAULT_NTP_PRIMARY "pool.ntp.org"
#define DEFAULT_NTP_SECONDARY "time.google.com"
#define DEFAULT_TZ_RULE "CET-1CEST,M3.5.0,M10.5.0/3"

