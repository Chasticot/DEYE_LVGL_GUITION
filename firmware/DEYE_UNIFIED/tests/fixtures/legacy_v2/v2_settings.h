#pragma once
#include <Preferences.h>
#include "v2_logic.h"

static V2Config cfg_v2;
static bool v2_correct_gen_daily(V2Config &value) {
  if (value.gen_daily_register != 536) return false;
  value.gen_daily_register = 62; // LP1 protocol; preserve custom scale and other settings.
  return true;
}
static bool v2_save(V2Config value) {
  if (!v2_config_valid(value)) return false;
  v2_correct_gen_daily(value);
  Preferences nvs;
  if (!nvs.begin("deye-v2", false)) return false;
  V2Config check;
  const bool ok = nvs.putBytes("config", &value, sizeof(value)) == sizeof(value) &&
    nvs.getBytes("config", &check, sizeof(check)) == sizeof(check) &&
    memcmp(&value, &check, sizeof(check)) == 0;
  nvs.end();
  // The UI restarts after saving; the reader uses immutable settings until then.
  return ok;
}
static void v2_load() {
  cfg_v2 = V2Config{};
  Preferences nvs;
  if (!nvs.begin("deye-v2", true)) return;
  V2Config saved;
  if (nvs.getBytesLength("config") == sizeof(saved) &&
      nvs.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) && v2_config_valid(saved)) cfg_v2 = saved;
  nvs.end();
  if (v2_correct_gen_daily(cfg_v2)) v2_save(cfg_v2);
}
