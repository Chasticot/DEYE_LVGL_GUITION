#pragma once
#include <Preferences.h>
#include <Arduino.h>
#include <stddef.h>
#include "inverter_profiles.h"
static bool settings_registers_valid(const CustomRegisters &regs) {
  DeyeReadBlock one, two;
  const bool blocks = deye_register_blocks(regs, one, two);
  const uint16_t required[] = {regs.pv1_power,regs.pv2_power,regs.pv_daily,regs.battery_soc,
    regs.battery_voltage,regs.battery_power,regs.battery_temp,regs.grid_power,regs.grid_status,
    regs.grid_buy_daily,regs.grid_sell_daily,regs.load_power,regs.load_daily,regs.dc_temp,regs.ac_temp};
  for (auto address : required) if (address == DEYE_NO_REGISTER) return false;
  const bool timeouts = regs.connect_timeout >= 50 && regs.connect_timeout <= 60000 &&
    regs.response_window >= 50 && regs.response_window <= 60000 && regs.frame_timeout >= 50 &&
    regs.frame_timeout <= 60000 && regs.block_interval >= 50 && regs.block_interval <= 60000;
  const bool coefficients = isfinite(regs.coeff_grid_power) && isfinite(regs.coeff_load_power) &&
    isfinite(regs.coeff_gen_power) && isfinite(regs.coeff_battery_power) &&
    fabsf(regs.coeff_battery_power) <= 100 && regs.smartload_bit <= 15 &&
    regs.coeff_grid_power >= -100.0f && regs.coeff_grid_power <= 100.0f &&
    regs.coeff_load_power >= -100.0f && regs.coeff_load_power <= 100.0f &&
    regs.coeff_gen_power >= -100.0f && regs.coeff_gen_power <= 100.0f;
  return timeouts && coefficients && blocks;
}

// ==================== FONCTIONS DE SAUVEGARDE DES REGISTRES ====================
static const char *settings_registers_key() { return deye_is_aiw51() ? "registers_p1" : "registers"; }

static bool settings_save_registers(const CustomRegisters &regs) {
  if (!settings_registers_valid(regs)) return false;
  Preferences storage;
  if (!storage.begin(inverter_profile().storage, false)) return false;
  CustomRegisters verify;
  const bool ok = storage.putBytes(settings_registers_key(), &regs, sizeof(regs)) == sizeof(regs) &&
    storage.getBytes(settings_registers_key(), &verify, sizeof(verify)) == sizeof(verify) &&
    memcmp(&regs, &verify, sizeof(regs)) == 0;
  storage.end();
  return ok;
}

static CustomRegisters settings_load_registers() {
  CustomRegisters regs = inverter_profile().registers;
  Preferences storage;
  if (storage.begin(inverter_profile().storage, true)) {
    CustomRegisters saved;
    const bool valid = storage.getBytesLength(settings_registers_key()) == sizeof(saved) &&
      storage.getBytes(settings_registers_key(), &saved, sizeof(saved)) == sizeof(saved) && settings_registers_valid(saved);
    const bool legacy_ai = !valid && deye_is_aiw51() &&
      storage.getBytesLength("registers") == sizeof(saved) &&
      storage.getBytes("registers", &saved, sizeof(saved)) == sizeof(saved);
    storage.end();
    if (valid) return saved;
    if (legacy_ai) {
      // Replace the old map once, retaining valid timings and coefficients.
      memcpy(&saved, &regs, offsetof(CustomRegisters, connect_timeout));
      saved.pv4_power = DEYE_NO_REGISTER;
      if (!settings_registers_valid(saved)) saved = regs;
      settings_save_registers(saved); // Failed persistence is retried next boot.
      return saved;
    }
  }
  // Only the original SG02 profile may inherit the v2 register configuration.
  if (cfg_inverter_model != 0 || !preferences.begin("deye-ui", true)) return regs;
  const size_t legacy_size = offsetof(CustomRegisters, pv4_power);
  if (preferences.getBytesLength("regs_v3") == legacy_size &&
      preferences.getBytes("regs_v3", &regs, legacy_size) == legacy_size && settings_registers_valid(regs)) {
    preferences.end(); return regs;
  }
  regs.pv1_power = preferences.getUShort("reg_pv1", 186);
  regs.pv2_power = preferences.getUShort("reg_pv2", 187);
  regs.pv3_power = preferences.getUShort("reg_pv3", 188);
  regs.pv_daily = preferences.getUShort("reg_pvd", 108);
  regs.battery_soc = preferences.getUShort("reg_bat_soc", 184);
  regs.battery_voltage = preferences.getUShort("reg_bat_v", 183);
  regs.battery_power = preferences.getUShort("reg_bat_p", 190);
  regs.battery_temp = preferences.getUShort("reg_bat_t", 182);
  regs.grid_power = preferences.getUShort("reg_grid_p", 169);
  regs.grid_status = preferences.getUShort("reg_grid_s", 194);
  regs.grid_buy_daily = preferences.getUShort("reg_grid_buy", 76);
  regs.grid_sell_daily = preferences.getUShort("reg_grid_sell", 77);
  regs.load_power = preferences.getUShort("reg_load_p", 178);
  regs.gen_power = preferences.getUShort("reg_gen_p", 166);
  regs.load_daily = preferences.getUShort("reg_load_d", 84);
  regs.dc_temp = preferences.getUShort("reg_dc_t", 90);
  regs.ac_temp = preferences.getUShort("reg_ac_t", 91);
  regs.smartload = preferences.getUShort("reg_smart", 195);
  regs.smartload_bit = preferences.getUShort("smart_bit", 0);
  regs.connect_timeout = preferences.getUInt("reg_conn_t", 10000);
  regs.response_window = preferences.getUInt("reg_resp_t", 10000);
  regs.frame_timeout = preferences.getUInt("reg_frame_t", 7000);
  regs.block_interval = preferences.getUInt("reg_block_i", 3000);
  // Coefficients
  regs.coeff_grid_power = preferences.getFloat("coeff_grid", 1.0f);
  regs.coeff_load_power = preferences.getFloat("coeff_load", 1.0f);
  regs.coeff_gen_power = preferences.getFloat("coeff_gen", -1.0f);
  regs.coeff_battery_power = preferences.getFloat("coeff_bat", -1.0f);
  

  preferences.end();
  return settings_registers_valid(regs) ? regs : inverter_profile().registers;
}

static void settings_load_model() {
  Preferences storage;
  cfg_inverter_model = 0;
  if (!storage.begin("deye-v3", true)) return;
  const String id = storage.getString("model", deye_profiles[0].id);
  storage.end();
  const int index = deye_profile_index(id.c_str());
  if (index >= 0 && deye_profiles[index].available) cfg_inverter_model = uint8_t(index);
}
static bool settings_save_model(uint16_t index) {
  if (index >= DEYE_PROFILE_COUNT || !deye_profiles[index].available) return false;
  Preferences storage;
  if (!storage.begin("deye-v3", false)) return false;
  const char *id = deye_profiles[index].id;
  const bool ok = storage.putString("model", id) == strlen(id) && storage.getString("model", "") == id;
  storage.end();
  // Do not mutate the live decoder while a Solarman request is in flight.
  return ok;
}

