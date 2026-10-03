#include <assert.h>
#include <stdio.h>
#include <initializer_list>
#include <Preferences.h>
static Preferences preferences;
#include "../inverter_settings.h"
#include "../charging_settings.h"
#include "../ev_register_settings.h"

static void stored_backend(const char *key) {
  Preferences storage;
  assert(storage.begin("deye-ui", false));
  assert(storage.putString(SETTINGS_KEY_EV_BACKEND, key) == strlen(key));
  storage.end();
}
static void legacy_registers(const EvRegisterConfig &cfg) {
  Preferences storage;
  assert(storage.begin("deye-ev", false));
  assert(storage.putBytes("config", &cfg, sizeof(cfg)) == sizeof(cfg));
  storage.end();
}
int main() {
  Preferences::records.clear();
  cfg_inverter_model = 0;
  settings_load_charging_backend(false);
  assert(cfg_ev_backend == EvBackend::None && !cfg_ev_charger_enabled && !cfg_ev_backend_needs_selection);
  settings_load_charging_backend(true);
  assert(cfg_ev_backend == EvBackend::None && cfg_ev_backend_needs_selection);
  assert(!ev_deye_enabled() && !ev_vetronic_enabled());
  for (const char *bad : {"", "unknown", "DEYE_LORA", "1", "vetronic_wb01 "}) {
    stored_backend(bad); settings_load_charging_backend(true);
    assert(cfg_ev_backend == EvBackend::None && !cfg_ev_charger_enabled && cfg_ev_backend_needs_selection);
  }
  stored_backend("none"); settings_load_charging_backend(true);
  assert(!cfg_ev_backend_needs_selection && !cfg_ev_charger_enabled);
  const EvBackend backends[] = {EvBackend::None, EvBackend::DeyeLoRa, EvBackend::VetronicWb01};
  for (unsigned i = 0; i < DEYE_PROFILE_COUNT; ++i) {
    cfg_inverter_model = i;
    for (auto backend : backends) {
      const bool supported = backend != EvBackend::DeyeLoRa || inverter_profile().ev_supported;
      assert(ev_backend_supported(backend, inverter_profile().ev_supported) == supported);
      EvBackend parsed = EvBackend::None;
      assert(ev_backend_parse(ev_backend_key(backend), parsed) && parsed == backend);
      stored_backend(ev_backend_key(backend)); settings_load_charging_backend(true);
      assert(cfg_ev_backend == (supported ? backend : EvBackend::None));
      assert(ev_deye_enabled() == (supported && backend == EvBackend::DeyeLoRa));
      assert(ev_vetronic_enabled() == (backend == EvBackend::VetronicWb01));
      for (bool unlocked : {false, true}) {
        assert(ev_native_write_allowed(backend, inverter_profile().ev_supported, unlocked, 489, 489, 490) ==
          (backend == EvBackend::DeyeLoRa && inverter_profile().ev_supported && unlocked));
        assert(!ev_native_write_allowed(backend, inverter_profile().ev_supported, unlocked, 488, 489, 490));
        assert(!ev_native_write_allowed(backend, inverter_profile().ev_supported, unlocked, 489, 489, 489));
      }
    }
  }
  EvBackend untouched = EvBackend::VetronicWb01;
  assert(!ev_backend_parse(nullptr, untouched) && untouched == EvBackend::VetronicWb01);
  assert(!ev_backend_supported(static_cast<EvBackend>(255), true));
  // Saving cannot change a live decoder or charge worker until reboot/load.
  cfg_inverter_model = 0;
  stored_backend("none"); settings_load_charging_backend(false);
  assert(settings_save_ev_backend(EvBackend::VetronicWb01));
  assert(cfg_ev_backend == EvBackend::None && !cfg_ev_charger_enabled);
  settings_load_charging_backend(false); assert(ev_vetronic_enabled());
  Preferences::fail_write = true;
  assert(!settings_save_ev_backend(EvBackend::DeyeLoRa));
  Preferences::fail_write = false;
  settings_load_charging_backend(false); assert(ev_vetronic_enabled());
  assert(settings_save_ev_backend(EvBackend::DeyeLoRa)); assert(ev_vetronic_enabled());
  assert(settings_save_model(1) && cfg_inverter_model == 0);
  settings_load_model(); assert(cfg_inverter_model == 1);
  settings_load_charging_backend(false);
  assert(!cfg_ev_charger_enabled && cfg_ev_backend_needs_selection);
  assert(settings_save_model(0)); settings_load_model(); settings_load_charging_backend(false);
  assert(ev_deye_enabled()); // Incompatible load preserved the stored backend.
  // Old addresses are retained, but pre-4.3.5 global unlock is never inherited.
  Preferences::records.erase("deye-ev");
  EvRegisterConfig old; old.mode_register = 600; old.max_power_register = 601; old.write_enabled = true;
  legacy_registers(old);
  Preferences::fail_write = true; settings_load_ev_registers();
  assert(cfg_ev_registers.mode_register == 600 && !cfg_ev_registers.write_enabled);
  assert(Preferences::records["deye-ev"][EV_REGISTER_STORAGE_KEY].empty());
  Preferences::fail_write = false; settings_load_ev_registers();
  assert(cfg_ev_registers.max_power_register == 601 && !cfg_ev_registers.write_enabled);
  assert(Preferences::records["deye-ev"][EV_REGISTER_STORAGE_KEY].size() == sizeof(old));
  EvRegisterConfig unlocked = cfg_ev_registers; unlocked.write_enabled = true;
  assert(settings_save_ev_registers(unlocked)); assert(!cfg_ev_registers.write_enabled);
  settings_load_ev_registers(); assert(cfg_ev_registers.write_enabled);
  stored_backend("vetronic_wb01"); settings_load_charging_backend(false); settings_load_ev_registers();
  assert(!cfg_ev_registers.write_enabled && !settings_save_ev_registers(unlocked));
  assert(!ev_native_write_allowed(cfg_ev_backend, true, true, 600, 600, 601));
  stored_backend("none"); settings_load_charging_backend(false); settings_load_ev_registers();
  assert(!cfg_ev_registers.write_enabled && !settings_save_ev_registers(unlocked));
  Preferences storage; assert(storage.begin("deye-ev", true));
  EvRegisterConfig check;
  assert(storage.getBytes("config", &check, sizeof(check)) == sizeof(check));
  assert(check.write_enabled && check.mode_register == 600); storage.end();
  puts("PASS: backend/profile matrix, exclusive native writes, NVS migration, write failures and immutable selection");
}
