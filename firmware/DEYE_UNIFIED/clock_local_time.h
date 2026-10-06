#pragma once

#include <time.h>

static bool clock_get_local_time(struct tm *local) {
  // Arduino-ESP32 getLocalTime(..., 0) can skip its entire loop when millis()
  // advances between the start timestamp and the first timeout check.
  // Read the system clock once, without a timeout or a network dependency.
  const time_t timestamp = time(nullptr);
  return localtime_r(&timestamp, local) != nullptr && local->tm_year > (2016 - 1900);
}
