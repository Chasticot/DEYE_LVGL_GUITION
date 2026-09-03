#pragma once

#include "Arduino.h"

class Preferences {
 public:
  bool begin(const char *, bool) { return true; }
  void end() {}
  String getString(const char *key, const char *fallback) const { return strings_.count(key) ? strings_.at(key) : String(fallback); }
  uint32_t getUInt(const char *key, uint32_t fallback) const { return uints_.count(key) ? uints_.at(key) : fallback; }
  uint16_t getUShort(const char *key, uint16_t fallback) const { return ushorts_.count(key) ? ushorts_.at(key) : fallback; }
  float getFloat(const char *key, float fallback) const { return floats_.count(key) ? floats_.at(key) : fallback; }
  bool getBool(const char *key, bool fallback) const { return bools_.count(key) ? bools_.at(key) : fallback; }
  void putString(const char *key, const String &value) { strings_[key] = value; }
  void putUInt(const char *key, uint32_t value) { uints_[key] = value; }
  void putUShort(const char *key, uint16_t value) { ushorts_[key] = value; }
  void putFloat(const char *key, float value) { floats_[key] = value; }
  void putBool(const char *key, bool value) { bools_[key] = value; }
 private:
  inline static std::map<std::string, String> strings_;
  inline static std::map<std::string, uint32_t> uints_;
  inline static std::map<std::string, uint16_t> ushorts_;
  inline static std::map<std::string, float> floats_;
  inline static std::map<std::string, bool> bools_;
};
