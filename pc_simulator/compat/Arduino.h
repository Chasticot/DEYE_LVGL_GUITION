#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

class String {
 public:
  String() = default;
  String(const char *value) : value_(value ? value : "") {}
  String(const std::string &value) : value_(value) {}
  String(int value) : value_(std::to_string(value)) {}
  String(unsigned int value) : value_(std::to_string(value)) {}
  String(long value) : value_(std::to_string(value)) {}
  String(unsigned long value) : value_(std::to_string(value)) {}
  const char *c_str() const { return value_.c_str(); }
  size_t length() const { return value_.length(); }
  String &operator+=(const char *value) { value_ += value ? value : ""; return *this; }
  String &operator+=(const String &value) { value_ += value.value_; return *this; }
  bool operator==(const String &other) const { return value_ == other.value_; }
  bool operator==(const char *other) const { return value_ == (other ? other : ""); }
  friend String operator+(const String &left, const String &right) { return String(left.value_ + right.value_); }
  friend String operator+(const String &left, const char *right) { return String(left.value_ + (right ? right : "")); }
  friend String operator+(const char *left, const String &right) { return String((left ? left : "") + right.value_); }
 private:
  std::string value_;
};

class SerialMock {
 public:
  void begin(unsigned long) {}
  void print(const char *) {}
  void print(const String &) {}
  void println() {}
  void println(const char *) {}
  void println(const String &) {}
  template <typename... Args> void printf(const char *, Args...) {}
};

inline SerialMock Serial;
inline uint32_t millis() {
  static const auto start = std::chrono::steady_clock::now();
  return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());
}
inline void delay(uint32_t) {}
template <typename T, typename L, typename H>
constexpr T constrain(T value, L low, H high) {
  const T lower = static_cast<T>(low);
  const T upper = static_cast<T>(high);
  return std::min(std::max(value, lower), upper);
}

inline void configTzTime(const char *, const char *, const char *) {}
inline bool getLocalTime(struct tm *timeinfo, uint32_t) {
  if (!timeinfo) return false;
  const std::time_t now = std::time(nullptr);
  return localtime_s(timeinfo, &now) == 0;
}

class EspMock { public: void restart() {} };
inline EspMock ESP;
