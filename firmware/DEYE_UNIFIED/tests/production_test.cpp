#include "../pv_production.h"
#include "../inverter_profiles.h"
#include <assert.h>
#include <stdio.h>
#include <limits>

int main() {
  V2Config config;
  config.add_gen = true;
  DashboardData data = {};
  data.pv1_w = 1000; data.pv2_w = 2000; data.pv3_w = 3000; data.pv4_w = 4000;
  // Same production with either calibrated sign: GEN is the fifth source.
  const int32_t gen_signs[] = {750, -750};
  for (int32_t gen : gen_signs) {
    data.gen_power = gen;
    assert(v2_production_power(data, config, v2_gen_as_pv(config, true, false)) == 10750);
    assert(v2_production_power(data, config, v2_gen_as_pv(config, true, true)) == 10000);
    assert(v2_production_power(data, config, v2_gen_as_pv(config, false, false)) == 10000);
    config.add_gen = false;
    assert(v2_production_power(data, config, v2_gen_as_pv(config, true, false)) == 10000);
    config.add_gen = true;
  }
  for (const auto &profile : deye_profiles) {
    data.gen_power = deye_scale_power(750, profile.registers.coeff_gen_power, 1.0f);
    const bool included = v2_gen_as_pv(config, profile.gen_supported, false);
    assert(v2_production_power(data, config, included) == 10000U + (included ? 750U : 0U));
  }
  data.gen_power = deye_scale_power(500, -0.1f, 1.0f);
  assert(v2_production_power(data, config, true) == 10050); // Preserve calibration.
  data.gen_power = -750;
  config.pv_visible[1] = false; config.pv_visible[2] = false; config.pv4_visible = false;
  assert(v2_production_power(data, config, true) == 1750);
  config.pv_visible[0] = false;
  assert(v2_production_power(data, config, true) == 750);
  assert(v2_production_power(data, config, false) == 0);
  assert(v2_gen_production_w(0) == 0);
  assert(v2_gen_production_w(std::numeric_limits<int32_t>::min()) == 2147483648U);

  config = V2Config{}; config.add_gen = true;
  float day = 0;
  assert(v2_production_daily_kwh(config, 4, 123, true, true, true, 4.5f, day));
  assert(fabsf(day - 16.8f) < 0.00001f);
  assert(v2_production_daily_kwh(config, 4, 123, true, false, false, 4.5f, day));
  assert(fabsf(day - 12.3f) < 0.00001f);
  assert(!v2_production_daily_kwh(config, 4, 123, true, true, false, 4.5f, day));
  assert(!v2_production_daily_kwh(config, 4, 123, false, true, true, 4.5f, day));
  for (auto &visible : config.pv_visible) visible = false;
  config.pv4_visible = false;
  assert(v2_production_daily_kwh(config, 4, 123, false, true, true, 4.5f, day));
  assert(day == 4.5f);
  assert(v2_production_daily_kwh(config, 4, 123, false, false, false, 4.5f, day) && day == 0);
  config.pv_visible[2] = true; config.pv4_visible = true;
  assert(v2_production_daily_kwh(config, 2, 123, false, true, true, 4.5f, day) && day == 4.5f);

  // Register-free fallback uses the same magnitude as the instantaneous total.
  V2EnergyIntegrator meter;
  for (uint32_t ms = 0; ms <= 3600000; ms += 10000)
    meter.sample(1, true, float(v2_gen_production_w(-1000)), ms);
  assert(fabsf(meter.kwh - 1.0f) < 0.0001f);
  meter.sample(1, false, 1000, 3610000);
  meter.sample(1, true, 1000, 3620000);
  assert(fabsf(meter.kwh - 1.0f) < 0.0001f);
  meter.sample(2, true, 1000, 3630000);
  assert(meter.kwh == 0);
  puts("PASS: GEN MO as PV5, both signs, PV visibility, SmartLoad/disabled/unsupported, daily totals and energy fallback");
}
