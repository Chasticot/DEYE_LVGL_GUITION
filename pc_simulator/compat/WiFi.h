#pragma once

#include "Arduino.h"

constexpr int WL_CONNECTED = 3;
constexpr int WIFI_STA = 1;
constexpr int WIFI_SCAN_RUNNING = -1;
constexpr int WIFI_SCAN_FAILED = -2;
class WiFiMock {
 public:
  void mode(int) {}
  void begin(const char *, const char *) {}
  void persistent(bool) {}
  void setAutoReconnect(bool) {}
  bool reconnect() { return true; }
  void disconnect() {}
  int status() const { return WL_CONNECTED; }
  int RSSI() const { return -65; }
  int RSSI(int index) const { return index == 0 ? -61 : -72; }
  String SSID() const { return String("WiFi simulation"); }
  String SSID(int index) const { return String(index == 0 ? "WiFi simulation" : "Reseau voisin"); }
  int scanNetworks() const { return 2; }
  int scanNetworks(bool, bool) const { return WIFI_SCAN_RUNNING; }
  int scanComplete() const { return 2; }
  void scanDelete() {}
};
inline WiFiMock WiFi;
