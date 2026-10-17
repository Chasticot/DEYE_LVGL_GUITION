#pragma once

#include <Arduino.h>
#include <time.h>
#include <atomic>
#include <esp_sntp.h>
#include "settings.h"
#include "clock_local_time.h"
#include "manual_clock_logic.h"
#include <sys/time.h>

// Historical name: this flag marks a trusted clock, whether set by NTP or manually.
static std::atomic<bool> ntp_received{false};
static std::atomic<bool> manual_time_active{false};
static void ntp_time_received(struct timeval *) { manual_time_active.store(false); ntp_received.store(true); }

static const char *ntp_manager_status() {
  return manual_time_active.load() ? "Heure manuelle (NTP automatique au retour du reseau)" :
    ntp_received.load() ? "Heure NTP synchronisee" : "Heure non reglee";
}
static bool ntp_manager_set_manual(const char *date, const char *clock) {
  time_t epoch;
  if (!manual_clock_parse(date, clock, epoch)) return false;
  struct timeval value = {epoch, 0};
  if (settimeofday(&value, nullptr) != 0) return false;
  manual_time_active.store(true);
  ntp_received.store(true);
  return true;
}

static void ntp_manager_begin() {
  sntp_set_time_sync_notification_cb(ntp_time_received);
  configTzTime(
    cfg_tz_rule.c_str(),
    cfg_ntp_primary.c_str(),
    cfg_ntp_secondary.c_str()
  );
}

static bool ntp_manager_get_local_time(struct tm *timeinfo) {
  return clock_get_local_time(timeinfo);
}
