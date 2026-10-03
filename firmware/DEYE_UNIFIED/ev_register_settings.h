#pragma once
#include <Preferences.h>

struct EvRegisterConfig {
  uint32_t version = 1;
  uint16_t mode_register = 489;
  uint16_t max_power_register = 490;
  bool write_enabled = false;
};
static EvRegisterConfig cfg_ev_registers;
static constexpr char EV_REGISTER_STORAGE_KEY[] = "config_435";

static bool settings_ev_registers_valid(const EvRegisterConfig &cfg) {
  const uint16_t first = cfg.mode_register < cfg.max_power_register ? cfg.mode_register : cfg.max_power_register;
  const uint16_t last = cfg.mode_register < cfg.max_power_register ? cfg.max_power_register : cfg.mode_register;
  return cfg.version == 1 && cfg.mode_register != cfg.max_power_register && uint32_t(last) - first + 1 <= 125;
}
static bool settings_save_ev_registers(const EvRegisterConfig &cfg) {
  if (!settings_ev_registers_valid(cfg) || (cfg.write_enabled && !ev_deye_enabled())) return false;
  Preferences storage;
  if (!storage.begin("deye-ev", false)) return false;
  EvRegisterConfig check;
  const bool ok = storage.putBytes(EV_REGISTER_STORAGE_KEY, &cfg, sizeof(cfg)) == sizeof(cfg) &&
    storage.getBytes(EV_REGISTER_STORAGE_KEY, &check, sizeof(check)) == sizeof(check) &&
    memcmp(&cfg, &check, sizeof(cfg)) == 0;
  storage.end();
  // A saved permission is applied only on reboot, like the selected backend.
  return ok;
}
static void settings_load_ev_registers() {
  cfg_ev_registers = EvRegisterConfig{};
  if (!inverter_profile().ev_supported) return;
  Preferences storage;
  if (!storage.begin("deye-ev", true)) return;
  EvRegisterConfig saved;
  const bool valid = storage.getBytesLength(EV_REGISTER_STORAGE_KEY) == sizeof(saved) &&
    storage.getBytes(EV_REGISTER_STORAGE_KEY, &saved, sizeof(saved)) == sizeof(saved) && settings_ev_registers_valid(saved);
  const bool legacy = !valid && storage.getBytesLength("config") == sizeof(saved) &&
    storage.getBytes("config", &saved, sizeof(saved)) == sizeof(saved) && settings_ev_registers_valid(saved);
  storage.end();
  if (valid || legacy) cfg_ev_registers = saved;
  if (legacy) {
    cfg_ev_registers.write_enabled = false; // Never inherit an ambiguous pre-4.3.5 write permission.
    settings_save_ev_registers(cfg_ev_registers);
  }
  if (!ev_deye_enabled()) cfg_ev_registers.write_enabled = false;
}
