#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

// 65535 means that an optional measurement is absent from this profile.
static constexpr uint16_t DEYE_NO_REGISTER = UINT16_MAX;
struct CustomRegisters {
  uint16_t pv1_power, pv2_power, pv3_power, pv_daily;
  uint16_t battery_soc, battery_voltage, battery_power, battery_temp;
  uint16_t grid_power, grid_status, grid_buy_daily, grid_sell_daily;
  uint16_t load_power, gen_power, load_daily, dc_temp, ac_temp, smartload, smartload_bit;
  uint32_t connect_timeout, response_window, frame_timeout, block_interval;
  float coeff_grid_power, coeff_load_power, coeff_gen_power, coeff_battery_power;
  uint16_t pv4_power = DEYE_NO_REGISTER;
};
struct DeyeReadBlock { uint16_t start = 0, count = 0; };
template<size_t N> static bool deye_make_block(const uint16_t (&addresses)[N], DeyeReadBlock &block) {
  uint16_t first = UINT16_MAX, last = 0;
  for (auto address : addresses) {
    if (address == DEYE_NO_REGISTER) continue;
    if (address < first) first = address;
    if (address > last) last = address;
  }
  if (first == UINT16_MAX || uint32_t(last) - first + 1 > 125) return false;
  block.start = first; block.count = last - first + 1;
  return true;
}
static bool deye_register_blocks(const CustomRegisters &r, DeyeReadBlock &one, DeyeReadBlock &two) {
  const uint16_t a[] = {r.grid_buy_daily,r.grid_sell_daily,r.load_daily,r.dc_temp,r.ac_temp,r.pv_daily};
  const uint16_t b[] = {r.grid_power,r.load_power,r.battery_temp,r.battery_voltage,r.battery_soc,
    r.pv1_power,r.pv2_power,r.pv3_power,r.pv4_power,r.battery_power,r.grid_status,r.smartload};
  return deye_make_block(a, one) && deye_make_block(b, two);
}
struct InverterProfile {
  const char *id;                 // Stable identity for NVS and JSON, never a list index.
  const char *name;
  const char *storage;
  const char *options_storage;
  CustomRegisters registers;
  float pv_scale, battery_voltage_scale, battery_power_scale;
  int8_t grid_status_bit;         // -1: raw == 1; otherwise bit mask.
  uint8_t pv_count;
  bool gen_supported, ev_supported, experimental;
  uint16_t gen_daily;
  bool available = true;
  const char *note = "";
};

static InverterProfile deye_variant(InverterProfile base, const char *id, const char *name,
    const char *storage, const char *options, bool low_voltage_single_phase = false) {
  base.id = id; base.name = name; base.storage = storage; base.options_storage = options;
  base.ev_supported = false;
  if (low_voltage_single_phase) {
    base.pv_count = 2;
    base.registers.pv3_power = DEYE_NO_REGISTER;
    // The historical renderer applies x10 before these user coefficients.
    base.registers.coeff_grid_power = base.registers.coeff_load_power = 0.1f;
    base.gen_daily = 62; // LP1 Modbus protocol: daily GEN energy, 0.1 kWh.
    base.note = "Etat SmartLoad a verifier sur l'appareil.";
  }
  if (strcmp(id, "sun-10k-sg04lp3") == 0) base.note = "Etat SmartLoad a verifier sur l'appareil.";
  return base;
}

// Ported from the four existing variants. See README_V3.md for provenance.
static const InverterProfile deye_profiles[] = {
  {"sun-12k-sg02lp1", "SUN-12K-SG02LP1-EU-AM2", "v3-sg02", "v3-sg02-opt",
   {186,187,188,108,184,183,190,182,169,194,76,77,178,166,84,90,91,195,0,
    10000,10000,7000,3000,1,1,-1,-1}, 1,0.01f,1,-1,3,true,true,false,62},
  {"sun-12k-sg05lp3", "SUN-12K-SG05LP3-EU-SM2", "v3-sg05", "v3-sg05-opt",
   {672,673,DEYE_NO_REGISTER,529,588,587,590,586,625,552,520,521,653,667,526,540,541,552,3,
    10000,10000,7000,3000,0.1f,0.1f,1,-1}, 1,0.01f,1,2,2,true,false,false,536},
  {"sun-25k-sg01hp3", "SUN-25K-SG01HP3-EU-AM2", "v3-hp3", "v3-hp3-opt",
   {672,673,674,529,588,587,590,586,625,552,520,521,653,667,526,540,541,552,3,
    10000,10000,7000,3000,0.1f,0.1f,1,-1,675}, 10,0.1f,10,2,4,true,false,false,536},
  {"ai-w5.1-ess", "AI-W5.1 ESS (P1)", "v3-aiw51", "v3-aiw51-opt",
   {186,187,DEYE_NO_REGISTER,108,184,183,190,182,169,194,76,77,178,DEYE_NO_REGISTER,84,90,91,DEYE_NO_REGISTER,0,
    10000,10000,7000,3000,0.1f,0.1f,1,-1}, 1,0.01f,1,-1,2,false,false,false,0,true,
    "Cartographie P1 : fonctionnement confirme par l'utilisateur le 02/10/2026."},
  deye_variant(deye_profiles[0], "sun-8k-sg01lp1", "SUN-8K-SG01LP1-EU", "v3-8sg01", "v3-8sg01-opt", true),
  deye_variant(deye_profiles[0], "sun-6k-sg03lp1", "SUN-6K-SG03LP1-EU", "v3-6sg03", "v3-6sg03-opt", true),
  deye_variant(deye_profiles[0], "sun-8k-sg05lp1", "SUN-8K-SG05LP1-EU", "v3-8sg05", "v3-8sg05-opt", true),
  deye_variant(deye_profiles[1], "sun-10k-sg04lp3", "SUN-10K-SG04LP3-EU", "v3-10sg04", "v3-10sg04-opt"),
  deye_variant(deye_profiles[2], "sun-20k-sg01hp3", "SUN-20K-SG01HP3-EU-AM2", "v3-20hp3", "v3-20hp3-opt"),
  // This reference was discussed only for Daily Load. No complete map was established.
  []() { auto p = deye_variant(deye_profiles[0], "sun-6k-sg06lp1", "SUN-6K-SG06LP1 (a confirmer)",
      "v3-6sg06", "v3-6sg06-opt", true);
    p.available = false; p.note = "Cartographie incomplete : selection indisponible."; return p; }(),
  // Additional references reported working by users (2026-10-01).
  deye_variant(deye_profiles[0], "sun-5k-sg05lp1-eu-am2-p", "SUN-5K-SG05LP1-EU-AM2-P",
    "v3-5sg05p", "v3-5sg05p-opt", true),
  deye_variant(deye_profiles[0], "sun-6k-sg05lp1", "SUN-6K-SG05LP1",
    "v3-6sg05", "v3-6sg05-opt", true),
  deye_variant(deye_profiles[0], "sun-12k-sg02lp1-eu-am3", "SUN-12K-SG02LP1-EU-AM3",
    "v3-sg02am3", "v3-sg02am3-opt")
};
static constexpr size_t DEYE_PROFILE_COUNT = sizeof(deye_profiles) / sizeof(deye_profiles[0]);
static uint8_t cfg_inverter_model = 0; // Immutable after startup; save selection then reboot.
static const InverterProfile &inverter_profile() { return deye_profiles[cfg_inverter_model]; }
static bool deye_is_aiw51() { return strcmp(inverter_profile().id, "ai-w5.1-ess") == 0; }
static uint16_t deye_aiw51_p1_address(uint16_t address) {
  const uint16_t old_regs[] = {672,673,529,588,587,590,586,607,552,520,521,637,526,540,541};
  const uint16_t new_regs[] = {186,187,108,184,183,190,182,169,194,76,77,178,84,90,91};
  for (size_t i = 0; i < sizeof(old_regs) / sizeof(old_regs[0]); ++i)
    if (address == old_regs[i]) return new_regs[i];
  return address;
}
static int deye_profile_index(const char *id) {
  if (id) for (size_t i = 0; i < DEYE_PROFILE_COUNT; ++i)
    if (strcmp(id, deye_profiles[i].id) == 0) return int(i);
  return -1;
}
static bool deye_grid_connected(uint16_t raw, const InverterProfile &p) {
  return p.grid_status_bit < 0 ? raw == 1 : (raw & (1U << p.grid_status_bit)) != 0;
}
static int32_t deye_scale_power(int16_t raw, float coefficient, float scale) {
  return int32_t(lroundf(float(raw) * coefficient * scale));
}
