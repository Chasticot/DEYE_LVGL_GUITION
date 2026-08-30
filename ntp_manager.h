#pragma once

#include <Arduino.h>
#include <time.h>
#include "settings.h"

static void ntp_manager_begin() {
  configTzTime(
    cfg_tz_rule.c_str(),
    cfg_ntp_primary.c_str(),
    cfg_ntp_secondary.c_str()
  );
}

static bool ntp_manager_get_local_time(struct tm *timeinfo) {
  return getLocalTime(timeinfo, 10);
}

