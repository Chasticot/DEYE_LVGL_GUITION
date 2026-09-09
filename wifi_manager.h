#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "settings.h"

static bool wifi_manager_scan_active = false;

static IPAddress wifi_network_address(uint32_t value) {
  return IPAddress(value >> 24, (value >> 16) & 255, (value >> 8) & 255, value & 255);
}

static void wifi_manager_apply_network() {
  // DHCP is the driver's default after boot. Changes always restart the device.
  if (!cfg_network.use_static) return;
  const uint32_t dns = cfg_network.dns ? cfg_network.dns : cfg_network.gateway;
  if (!WiFi.config(wifi_network_address(cfg_network.ip), wifi_network_address(cfg_network.gateway),
                   wifi_network_address(cfg_network.mask), wifi_network_address(dns))) {
    DBG.println("Configuration IP statique impossible.");
  }
}

static void wifi_manager_set_scan_active(bool active) {
  wifi_manager_scan_active = active;
}

static void wifi_manager_resume_after_scan() {
  wifi_manager_scan_active = false;
  wifi_manager_apply_network();
  WiFi.begin(cfg_wifi_ssid.c_str(), cfg_wifi_password.c_str());
}

static void wifi_manager_begin() {
  WiFi.mode(WIFI_STA);
  wifi_manager_apply_network();
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
