#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <atomic>
#include <string.h>
#ifdef _WIN32
static struct tm *localtime_r(const time_t *epoch, struct tm *out) {
  return localtime_s(out, epoch) == 0 ? out : nullptr;
}
#endif
#include "../manual_clock_logic.h"
static bool fail_set_clock = false;
static time_t applied_epoch = 0;
struct TestTimeval { time_t tv_sec; int tv_usec; };
#define timeval TestTimeval
static int settimeofday(const timeval *value, void *) {
  if (fail_set_clock) return -1;
  applied_epoch = value->tv_sec; return 0;
}
#include "clock_manager_under_test.h"
int main() {
#ifdef _WIN32
  _putenv_s("TZ", "CET-1CEST,M3.5.0,M10.5.0/3"); _tzset();
#else
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1); tzset();
#endif
  time_t epoch;
  assert(manual_clock_parse("2026-10-17","14:25",epoch));
  struct tm check = {}; localtime_r(&epoch,&check);
  assert(check.tm_hour == 14 && check.tm_min == 25 && check.tm_mday == 17);
  assert(manual_clock_parse("2028-02-29","23:59:59",epoch));
  assert(!manual_clock_parse("2026-02-29","10:00",epoch));
  assert(!manual_clock_parse("2026-04-31","10:00",epoch));
  assert(!manual_clock_parse("2026-13-01","10:00",epoch));
  assert(!manual_clock_parse("2026-10-17","24:00",epoch));
  assert(!manual_clock_parse("2026-10-17","12:60",epoch));
  assert(!manual_clock_parse("2026-10-17","12:30:60",epoch));
  assert(!manual_clock_parse("2026-10-17","12:30xx",epoch));
  assert(!manual_clock_parse("2026-10-17","12:30:00junk",epoch));
  assert(!manual_clock_parse("2026-0a-17","12:30",epoch));
  assert(!ntp_manager_set_manual("2026-02-29","12:30") && !ntp_received.load());
  fail_set_clock = true;
  assert(!ntp_manager_set_manual("2026-10-17","12:30") && !ntp_received.load());
  fail_set_clock = false;
  assert(ntp_manager_set_manual("2026-10-17","12:30") && ntp_received.load() && manual_time_active.load());
  assert(applied_epoch > 0 && strstr(ntp_manager_status(),"manuelle"));
  ntp_time_received(nullptr);
  assert(ntp_received.load() && !manual_time_active.load() && strstr(ntp_manager_status(),"NTP synchronisee"));
  puts("Manual clock: calendar, parsing and local time round-trip passed");
}
