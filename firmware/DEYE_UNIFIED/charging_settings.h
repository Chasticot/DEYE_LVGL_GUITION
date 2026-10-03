#pragma once
#include <Preferences.h>
#include "ev_backend.h"
#include "inverter_profiles.h"

// Immutable after settings_load; changes only take effect after reboot.
static EvBackend cfg_ev_backend = EvBackend::None;
static bool cfg_ev_charger_enabled = false;
static bool cfg_ev_backend_needs_selection = false;
static constexpr char SETTINGS_KEY_EV_BACKEND[] = "ev_backend";
static_assert(sizeof(SETTINGS_KEY_EV_BACKEND) <= 16, "Cle NVS trop longue");

static bool ev_deye_enabled() {
  return cfg_ev_charger_enabled && cfg_ev_backend == EvBackend::DeyeLoRa && inverter_profile().ev_supported;
}
static bool ev_vetronic_enabled() {
  return cfg_ev_charger_enabled && cfg_ev_backend == EvBackend::VetronicWb01;
}
static bool settings_save_ev_backend(EvBackend backend) {
  if (!ev_backend_supported(backend, inverter_profile().ev_supported)) return false;
  Preferences storage;
  if (!storage.begin("deye-ui", false)) return false;
  const char *key = ev_backend_key(backend);
  const bool ok = storage.putString(SETTINGS_KEY_EV_BACKEND, key) == strlen(key) &&
    storage.getString(SETTINGS_KEY_EV_BACKEND, "") == key;
  storage.end();
  return ok;
}
static void settings_load_charging_backend(bool legacy_enabled) {
  cfg_ev_backend = EvBackend::None;
  cfg_ev_charger_enabled = false;
  cfg_ev_backend_needs_selection = legacy_enabled;
  Preferences storage;
  if (!storage.begin("deye-ui", true)) return;
  const String key = storage.getString(SETTINGS_KEY_EV_BACKEND, "");
  storage.end();
  EvBackend selected;
  if (!ev_backend_parse(key.c_str(), selected)) {
    cfg_ev_backend_needs_selection = legacy_enabled || key.length() != 0;
    return;
  }
  if (!ev_backend_supported(selected, inverter_profile().ev_supported)) {
    cfg_ev_backend_needs_selection = true;
    return; // Keep the stored choice so returning to a compatible model restores it.
  }
  cfg_ev_backend = selected;
  cfg_ev_charger_enabled = selected != EvBackend::None;
  cfg_ev_backend_needs_selection = false;
}
