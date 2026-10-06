#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "../v2_logic.h"

static struct tm system_local = {};
static bool conversion_ok = true;
static uint32_t tick = 1000;
static unsigned clock_reads = 0;
static time_t test_time(time_t *) { ++clock_reads; return 1791320400; }
static struct tm *test_localtime_r(const time_t *, struct tm *local) {
  if (!conversion_ok) return nullptr;
  *local = system_local;
  return local;
}
#define time test_time
#define localtime_r test_localtime_r
#include "../clock_local_time.h"
#undef localtime_r
#undef time

// Timeout loop from the installed Arduino-ESP32 esp32-hal-time.c. The two
// millis() reads straddle a tick although the system time is already valid.
static bool legacy_get_local_time(struct tm *local, uint32_t timeout) {
  const uint32_t start = tick++;
  while ((tick++ - start) <= timeout) {
    const time_t timestamp = test_time(nullptr);
    test_localtime_r(&timestamp, local);
    if (local->tm_year > (2016 - 1900)) return true;
    tick += 10;
  }
  return false;
}

int main() {
  system_local.tm_year = 126;
  system_local.tm_hour = 23;
  struct tm local = {};
  assert(!legacy_get_local_time(&local, 0));
  assert(clock_reads == 0); // valid time was never read
  assert(clock_get_local_time(&local));
  assert(clock_reads == 1 && local.tm_hour == 23);

  // Old false clock result wakes the screen and grants another 60 seconds.
  V2SleepState legacy;
  assert(!legacy.update(true, 0, 60000));
  assert(legacy.update(true, 60000, 60000));
  assert(!legacy.update(legacy_get_local_time(&local, 0), 60001, 60000));
  assert(!legacy.update(true, 60002, 60000));
  assert(!legacy.update(true, 120001, 60000));
  assert(legacy.update(true, 120002, 60000));

  V2SleepState fixed;
  assert(!fixed.update(true, 0, 60000));
  for (uint32_t now = 60000; now < 61000; ++now) {
    assert(clock_get_local_time(&local));
    const bool due = v2_in_window(local.tm_hour * 60 + local.tm_min, 1320, 360);
    assert(fixed.update(due, now, 60000));
  }
  // Normal schedule exit and manual wake continue to work.
  system_local.tm_hour = 6;
  assert(clock_get_local_time(&local));
  assert(!fixed.update(v2_in_window(local.tm_hour * 60, 1320, 360), 61000, 60000));
  system_local.tm_hour = 23;
  assert(clock_get_local_time(&local));
  assert(!fixed.update(true, 62000, 60000));
  assert(fixed.update(true, 122000, 60000));
  fixed.touch(true, 123000);
  assert(!fixed.update(true, 182999, 60000));
  assert(fixed.update(true, 183000, 60000));
  system_local.tm_year = 70;
  assert(!clock_get_local_time(&local));
  system_local.tm_year = 116;
  assert(!clock_get_local_time(&local));
  system_local.tm_year = 126; conversion_ok = false;
  assert(!clock_get_local_time(&local));
  puts("PASS: zero-timeout clock race reproduced, stable sleep, wake duration, invalid clock");
}
