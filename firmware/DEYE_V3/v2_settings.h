#pragma once
#include <Preferences.h>
#include "v2_logic.h"
#include "inverter_profiles.h"

static V2Config cfg_v2;
static const char *v2_storage_key() { return deye_is_aiw51() ? "config_p1" : "config"; }
static bool v2_correct_gen_daily(V2Config &value) {
  if (!inverter_profile().available || inverter_profile().gen_daily != 62 ||
      value.gen_daily_register != 536) return false;
  value.gen_daily_register = 62; // Only LP1: triphase R536 remains valid.
  return true;
}
static bool v2_save(V2Config value) {
  if (!v2_config_valid(value)) return false;
  v2_correct_gen_daily(value);
  Preferences nvs;
  if (!nvs.begin(inverter_profile().options_storage, false)) return false;
  V2Config check;
  const bool ok = nvs.putBytes(v2_storage_key(), &value, sizeof(value)) == sizeof(value) &&
    nvs.getBytes(v2_storage_key(), &check, sizeof(check)) == sizeof(check) &&
    memcmp(&value, &check, sizeof(check)) == 0;
  nvs.end();
  // The UI restarts after saving; the reader uses immutable settings until then.
  return ok;
}
static void v2_load() {
  cfg_v2 = V2Config{};
  cfg_v2.gen_daily_register = inverter_profile().gen_daily;
  cfg_v2.relay_register = inverter_profile().registers.battery_soc;
  Preferences nvs;
  if (nvs.begin(inverter_profile().options_storage, true)) {
    V2Config saved;
    const bool valid = nvs.getBytesLength(v2_storage_key()) == sizeof(saved) &&
      nvs.getBytes(v2_storage_key(), &saved, sizeof(saved)) == sizeof(saved) && v2_config_valid(saved);
    const bool legacy_ai = !valid && deye_is_aiw51() &&
      nvs.getBytesLength("config") == sizeof(saved) &&
      nvs.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) && v2_config_valid(saved);
    nvs.end();
    if (valid) {
      cfg_v2 = saved;
      if (v2_correct_gen_daily(cfg_v2)) v2_save(cfg_v2);
      return;
    }
    if (legacy_ai) {
      saved.relay_register = deye_aiw51_p1_address(saved.relay_register);
      cfg_v2 = saved;
      v2_save(cfg_v2);
      return;
    }
  }
  if (cfg_inverter_model != 0 || !nvs.begin("deye-v2", true)) return;
  // pv4_visible is appended; copy only the old struct's bytes, preserving its default.
  V2Config saved;
  const size_t old_size = (offsetof(V2Config, pv4_visible) + 3) & ~size_t(3);
  if (nvs.getBytesLength("config") == old_size &&
      nvs.getBytes("config", &saved, old_size) == old_size && v2_config_valid(saved)) {
    saved.pv4_visible = true;
    cfg_v2 = saved;
  }
  nvs.end();
  if (v2_correct_gen_daily(cfg_v2)) v2_save(cfg_v2);
}
