#include <assert.h>
#include <time.h>
#include <Preferences.h>
#include "../v2_logic.h"

// Exercise the production reader with deterministic Modbus, clock and flash adapters.
static V2Config cfg_v2;
static bool cfg_gen_smartload = true;
static struct { bool gen_supported = true; const char *storage = "gen-test"; } profile;
static const auto &inverter_profile() { return profile; }
static constexpr uint16_t DEYE_NO_REGISTER = 65535;
static uint16_t REG_GEN_POWER = 166;
static float COEFF_GEN_POWER = 1;
static bool clock_valid = true, energy_ok = true, power_ok = true;
static uint16_t raw_energy = 2780;
static unsigned energy_reads = 0;
static uint32_t now_ms = 1000;
static int day = 100;
static uint32_t millis() { return now_ms; }
static struct { bool load() const { return clock_valid; } } ntp_received;
static bool clock_get_local_time(tm *local) { local->tm_year = 126; local->tm_yday = day; return clock_valid; }
static int v2_day_key(const tm &local) { return (local.tm_year + 1900) * 1000 + local.tm_yday; }
static bool solarman_read_block(uint16_t address, unsigned, uint8_t *rtu, uint8_t *) {
  if (address == REG_GEN_POWER) { rtu[0] = 0x0a; rtu[1] = 0x5a; return power_ok; }
  assert(address == cfg_v2.gen_daily_register);
  ++energy_reads; rtu[0] = raw_energy >> 8; rtu[1] = raw_energy & 255; return energy_ok;
}
static uint16_t modbus_get_u16_be(const uint8_t *rtu, unsigned) { return (uint16_t(rtu[0]) << 8) | rtu[1]; }
static int32_t scaled_power(int16_t value, float scale, float) { return int32_t(value * scale); }
static uint32_t v2_gen_production_w(int32_t power) { return power > 0 ? uint32_t(power) : 0; }
static struct {
  bool gen_valid = false; uint32_t gen_ms = 0; int32_t gen_power = 0;
  float gen_kwh = 0; int energy_day = 0; bool energy_valid = false; V2RelaySample relay;
} v2_measurements;
static int v2_measure_lock, data_mutex;
static void portENTER_CRITICAL(int *) {} static void portEXIT_CRITICAL(int *) {}
static constexpr int portMAX_DELAY = 0;
static void xSemaphoreTake(int, int) {} static void xSemaphoreGive(int) {}
static struct { int16_t gen_power = 0; } main_data;
static int dashboard_data;
static void update_dashboard_from_data() {} static void history_add_snapshot(int) {}
static bool v2_gen_included() { return !cfg_gen_smartload && cfg_v2.add_gen; }
// Tariff collaborators are inert: this test calls only the auxiliary reader.
static bool ev_deye_enabled() { return false; }
static struct { bool write_enabled = false; } cfg_ev_registers;
static struct { void cancel() {} uint8_t next(bool, bool, uint8_t, uint16_t) { return 0; } } deye_tariffs;
struct EvDeyeData { bool valid = false; int command_state = 0; uint16_t mode_raw = 0, max_charge_power_raw = 0; };
struct EvDeyeCommand { bool set_mode = false; uint8_t mode = 0; bool tariff_automatic = false; };
static bool deye_copy_ev_snapshot(EvDeyeData *) { return false; }
static bool deye_ev_command_busy(int) { return false; }
static bool deye_ev_mode_write_supported(uint16_t) { return false; }
struct Tariff { bool grid_allowed = false; };
static Tariff v2_tariff() { return {}; }
static bool deye_submit_ev_command(EvDeyeCommand) { return false; }
#include "../v2_reader.h"

int main() {
  v2_read_auxiliary(false);
  assert(energy_reads == 0 && !v2_measurements.energy_valid);
  cfg_v2.show_gen_daily = true;
  v2_read_auxiliary(false);
  assert(energy_reads == 1 && v2_measurements.energy_valid && v2_measurements.gen_kwh == 278);
  // SmartLoad energy does not depend on NTP or the instantaneous power register.
  clock_valid = false; power_ok = false; raw_energy = 123;
  v2_read_auxiliary(false);
  assert(!v2_measurements.gen_valid && v2_measurements.energy_valid);
  assert(fabsf(v2_measurements.gen_kwh - 12.3f) < 0.001f && v2_measurements.energy_day == 0);
  energy_ok = false; v2_read_auxiliary(false);
  assert(!v2_measurements.energy_valid);
  energy_ok = true; clock_valid = power_ok = true;
  cfg_gen_smartload = false; cfg_v2.show_gen_daily = false;
  cfg_v2.gen_daily_register = 536; cfg_v2.gen_daily_scale = 0.01f; raw_energy = 27800;
  v2_read_auxiliary(false);
  assert(v2_measurements.energy_valid && v2_measurements.gen_kwh == 278);
  ++day; raw_energy = 0; v2_read_auxiliary(false);
  assert(v2_measurements.energy_valid && v2_measurements.gen_kwh == 0);
  assert(v2_measurements.energy_day == 2026000 + day);
  cfg_gen_smartload = true; cfg_v2.show_gen_daily = true; cfg_v2.gen_daily_register = 0;
  const unsigned before = energy_reads;
  v2_read_auxiliary(false);
  assert(energy_reads == before && !v2_measurements.energy_valid);
  cfg_v2.gen_daily_register = 62; profile.gen_supported = false;
  v2_read_auxiliary(false);
  assert(energy_reads == before && !v2_measurements.energy_valid);
  puts("PASS: GEN Daily reader in SmartLoad/GEN MO, optional reads, coefficient, no NTP, failed reads and day rollover");
}
