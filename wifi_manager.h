#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "settings.h"

static void wifi_manager_begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg_wifi_ssid.c_str(), cfg_wifi_password.c_str());

  DBG.print("Connexion Wi-Fi : ");
  DBG.println(cfg_wifi_ssid);
}

static int wifi_quality_percent() {
  if (WiFi.status() != WL_CONNECTED) return 0;

  int rssi = WiFi.RSSI();
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return 2 * (rssi + 100);
}
