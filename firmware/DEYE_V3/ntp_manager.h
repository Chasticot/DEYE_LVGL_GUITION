#pragma once

#include <Arduino.h>
#include <time.h>
#include <atomic>
#include <esp_sntp.h>
#include "settings.h"

static std::atomic<bool> ntp_received{false};
static void ntp_time_received(struct timeval *) { ntp_received.store(true); }

static void ntp_manager_begin() {
  sntp_set_time_sync_notification_cb(ntp_time_received);
  configTzTime(
    cfg_tz_rule.c_str(),
    cfg_ntp_primary.c_str(),
    cfg_ntp_secondary.c_str()
  );
}

static bool ntp_manager_get_local_time(struct tm *timeinfo) {
  return getLocalTime(timeinfo, 10);
}

