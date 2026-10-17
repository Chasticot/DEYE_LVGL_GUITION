#pragma once
#include <time.h>
#include <string.h>

static inline int manual_clock_digits(const char *text, size_t start, size_t count) {
  int value = 0;
  for (size_t i = start; i < start + count; ++i) {
    if (text[i] < '0' || text[i] > '9') return -1;
    value = value * 10 + text[i] - '0';
  }
  return value;
}
// Validate the calendar before mktime normalizes it. Round-trip also rejects
// nonexistent local times during the spring DST transition.
static inline bool manual_clock_parse(const char *date, const char *clock, time_t &epoch) {
  if (!date || !clock || strlen(date) != 10 || (strlen(clock) != 5 && strlen(clock) != 8) ||
      date[4] != '-' || date[7] != '-' || clock[2] != ':' || (strlen(clock) == 8 && clock[5] != ':')) return false;
  const int year = manual_clock_digits(date, 0, 4), month = manual_clock_digits(date, 5, 2),
    day = manual_clock_digits(date, 8, 2), hour = manual_clock_digits(clock, 0, 2),
    minute = manual_clock_digits(clock, 3, 2), second = strlen(clock) == 8 ? manual_clock_digits(clock, 6, 2) : 0;
  const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  if (year < 2020 || year > 2099 || month < 1 || month > 12 || day < 1 ||
      day > days[month-1] + (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) return false;
  struct tm local = {};
  local.tm_year = year - 1900; local.tm_mon = month - 1; local.tm_mday = day;
  local.tm_hour = hour; local.tm_min = minute; local.tm_sec = second; local.tm_isdst = -1;
  const time_t candidate = mktime(&local);
  struct tm check = {};
  if (candidate == time_t(-1) || !localtime_r(&candidate, &check) || check.tm_year != year-1900 ||
      check.tm_mon != month-1 || check.tm_mday != day || check.tm_hour != hour ||
      check.tm_min != minute || check.tm_sec != second) return false;
  epoch = candidate;
  return true;
}
