#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

static Preferences preferences;

static String cfg_wifi_ssid;
static String cfg_wifi_password;
static String cfg_deye_host;
static uint32_t cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
static String cfg_ntp_primary;
static String cfg_ntp_secondary;
static String cfg_tz_rule;

static void settings_load() {
  preferences.begin("deye-ui", true);

  cfg_wifi_ssid = preferences.getString("wifi_ssid", DEFAULT_WIFI_SSID);
  cfg_wifi_password = preferences.getString("wifi_pwd", DEFAULT_WIFI_PASSWORD);
  cfg_deye_host = preferences.getString("deye_host", DEFAULT_DEYE_HOST);
  cfg_logger_serial = preferences.getUInt("logger", DEFAULT_LOGGER_SERIAL);
  cfg_ntp_primary = preferences.getString("ntp_1", DEFAULT_NTP_PRIMARY);
  cfg_ntp_secondary = preferences.getString("ntp_2", DEFAULT_NTP_SECONDARY);
  cfg_tz_rule = preferences.getString("tz_rule", DEFAULT_TZ_RULE);

  preferences.end();
}

static void settings_save_wifi(const String &ssid, const String &password) {
  preferences.begin("deye-ui", false);
  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_pwd", password);
  preferences.end();
}

static void settings_save_ntp(
  const String &tz_rule,
  const String &server_primary,
  const String &server_secondary
) {
  preferences.begin("deye-ui", false);
  preferences.putString("tz_rule", tz_rule);
  preferences.putString("ntp_1", server_primary);
  preferences.putString("ntp_2", server_secondary);
  preferences.end();
}

static void settings_save_deye(const String &host, uint32_t logger_serial) {
  preferences.begin("deye-ui", false);
  preferences.putString("deye_host", host);
  preferences.putUInt("logger", logger_serial);
  preferences.end();
}
