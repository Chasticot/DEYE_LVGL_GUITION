#pragma once

#include "Arduino.h"

class Preferences {
 public:
  bool begin(const char *, bool) { return true; }
  void end() {}
  String getString(const char *key, const char *fallback) const { return strings_.count(key) ? strings_.at(key) : String(fallback); }
  uint32_t getUInt(const char *key, uint32_t fallback) const { return uints_.count(key) ? uints_.at(key) : fallback; }
  uint16_t getUShort(const char *key, uint16_t fallback) const { return ushorts_.count(key) ? ushorts_.at(key) : fallback; }
  uint8_t getUChar(const char *key, uint8_t fallback) const {
    if (uchars_.count(key)) return uchars_.at(key);
    // Le processus relancé par ESP.restart() hérite de cette valeur.
    if (std::strcmp(key, "ui_theme") == 0) {
      const char *value = std::getenv("DEYE_SIM_UI_THEME");
      if (value != nullptr && *value != '\0') {
        const unsigned long parsed = std::strtoul(value, nullptr, 10);
        if (parsed <= UINT8_MAX) return static_cast<uint8_t>(parsed);
      }
    }
    return fallback;
  }
  float getFloat(const char *key, float fallback) const { return floats_.count(key) ? floats_.at(key) : fallback; }
  bool getBool(const char *key, bool fallback) const {
    if (bools_.count(key)) return bools_.at(key);
    if (std::strcmp(key, "ui_theme_v2") == 0) {
      const char *value = std::getenv("DEYE_SIM_UI_THEME_V2");
      return value != nullptr && std::strcmp(value, "1") == 0;
    }
    return fallback;
  }
  void putString(const char *key, const String &value) { strings_[key] = value; }
  void putUInt(const char *key, uint32_t value) { uints_[key] = value; }
  void putUShort(const char *key, uint16_t value) { ushorts_[key] = value; }
  void putUChar(const char *key, uint8_t value) {
    uchars_[key] = value;
    if (std::strcmp(key, "ui_theme") == 0) {
      _putenv_s("DEYE_SIM_UI_THEME", std::to_string(value).c_str());
    }
  }
  void putFloat(const char *key, float value) { floats_[key] = value; }
  void putBool(const char *key, bool value) {
    bools_[key] = value;
    if (std::strcmp(key, "ui_theme_v2") == 0) {
      _putenv_s("DEYE_SIM_UI_THEME_V2", value ? "1" : "0");
    }
  }
 private:
  inline static std::map<std::string, String> strings_;
  inline static std::map<std::string, uint32_t> uints_;
  inline static std::map<std::string, uint16_t> ushorts_;
  inline static std::map<std::string, uint8_t> uchars_;
  inline static std::map<std::string, float> floats_;
  inline static std::map<std::string, bool> bools_;
};
