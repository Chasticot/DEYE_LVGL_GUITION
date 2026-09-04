#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "settings.h"

static bool wifi_manager_scan_active = false;

static void wifi_manager_set_scan_active(bool active) {
  wifi_manager_scan_active = active;
}

static void wifi_manager_resume_after_scan() {
  wifi_manager_scan_active = false;
  WiFi.begin(cfg_wifi_ssid.c_str(), cfg_wifi_password.c_str());
}

static void wifi_manager_begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg_wifi_ssid.c_str(), cfg_wifi_password.c_str());

  DBG.print("Connexion Wi-Fi : ");
  DBG.println(cfg_wifi_ssid);
}

static void wifi_manager_process() {
  // WiFi.begin() maintains the connection itself, as in the known-working project.
}

static int wifi_quality_percent() {
  if (WiFi.status() != WL_CONNECTED) return 0;

  int rssi = WiFi.RSSI();
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return 2 * (rssi + 100);
}
