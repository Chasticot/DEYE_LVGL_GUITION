#include "../v2_logic.h"
#include <assert.h>
#include <stdio.h>
#include <limits>

int main() {
  assert(!v2_response_recent(false, 0, 0, 120000));
  assert(v2_response_recent(true, 0, 0, 120000));
  assert(v2_response_recent(true, 100, 120100, 120000));
  assert(!v2_response_recent(true, 100, 120101, 120000));
  assert(v2_response_recent(true, 0xfffffff0, 10, 120000));
  for (unsigned minute = 0; minute < 1440; ++minute) {
    assert(v2_in_window(minute, 1320, 360) == (minute >= 1320 || minute < 360));
    assert(v2_in_window(minute, 120, 180) == (minute >= 120 && minute < 180));
    assert(!v2_in_window(minute, 0, 0));
  }
  assert(v2_compare(-1200, 0, 0));
  assert(v2_compare(80, 1, 80));
  assert(!v2_compare(79, 1, 80));
  assert(v2_compare(81, 2, 80));
  for (unsigned mask = 0; mask < 128; ++mask) {
    bool enabled = mask & 1, clock = mask & 2, hc_only = mask & 4, hc = mask & 8;
    bool block_red = mask & 16, known = mask & 32, red = mask & 64;
    const bool actual = v2_grid_allowed(enabled, clock, hc_only, hc, block_red, known, red);
    if (!enabled) assert(actual);
    else if (!clock || (hc_only && !hc) || (block_red && (!known || (red && !hc)))) assert(!actual);
    else assert(actual);
  }
  V2RelayState relay;
  assert(!relay.update(true, true, 100, 5000));
  assert(!relay.update(true, true, 5099, 5000));
  assert(relay.update(true, true, 5100, 5000));
  assert(relay.update(true, false, 6000, 5000));
  assert(relay.update(true, true, 7000, 5000));
  assert(!relay.update(false, true, 7001, 5000));
  assert(!relay.update(true, true, 0xfffffff0, 100));
  assert(relay.update(true, true, 84, 100));

  V2RelaySample sample;
  assert(!sample.valid(0));
  sample.accept(true, 85, 100);
  sample.accept(false, 0, 10000);
  assert(sample.value == 85 && sample.at == 100);
  assert(sample.valid(300099));
  assert(!sample.valid(300100));
  sample.accept(true, 70, 300101);
  assert(sample.valid(300101) && sample.value == 70);
  sample.accept(true, std::numeric_limits<float>::quiet_NaN(), 300102);
  assert(sample.value == 70 && sample.at == 300101);
  sample.accept(true, 90, 0xfffffff0);
  assert(sample.valid(299983));
  assert(!sample.valid(299984));
  V2RelayState cached_relay;
  sample.accept(true, 85, 0);
  assert(cached_relay.update(sample.valid(0), v2_compare(sample.value, 2, 80), 0, 0));
  sample.accept(false, 0, 45000);
  assert(cached_relay.update(sample.valid(299999), v2_compare(sample.value, 2, 80), 299999, 0));
  assert(!cached_relay.update(sample.valid(300000), true, 300000, 0));

  V2SleepState sleep;
  assert(!sleep.update(false, 0, 60000)); // disabled / awaiting NTP / outside interval
  assert(!sleep.update(true, 100, 60000)); // first NTP sync in the sleep window
  sleep.touch(true, 200);
  assert(!sleep.update(true, 60199, 60000));
  assert(sleep.update(true, 60200, 60000));
  sleep.touch(true, 0xfffffff0);
  assert(!sleep.update(true, 5, 60000));
  assert(sleep.update(true, 59984, 60000));
  assert(!sleep.update(false, 59985, 60000));
  assert(!sleep.update(true, 59986, 60000)); // next window also gets a grace period
  assert(!sleep.update(true, 119985, 60000));
  assert(sleep.update(true, 119986, 60000));
  V2SleepState first_sync;
  assert(!first_sync.update(false, 1000, 60000));
  assert(!first_sync.update(true, 2000, 60000));
  assert(!first_sync.update(true, 61999, 60000));
  assert(first_sync.update(true, 62000, 60000));
  assert(first_sync.update(true, 63000, 60000)); // later NTP syncs do not restart grace
  assert(!first_sync.update(false, 63001, 60000)); // disabled/outside window

  V2EnergyIntegrator meter;
  for (uint32_t ms = 0; ms <= 3600000; ms += 10000) meter.sample(1, true, 1000, ms);
  assert(fabsf(meter.kwh - 1) < 0.0001f);
  meter.sample(1, false, 1000, 3610000);
  meter.sample(1, true, 1000, 3620000);
  assert(fabsf(meter.kwh - 1) < 0.0001f); // do not bridge missing samples
  meter.sample(1, true, 1000, 3700000);
  assert(fabsf(meter.kwh - 1) < 0.0001f); // stale gap
  meter.sample(2, true, 2000, 3710000);
  assert(meter.kwh == 0); // no previous-day energy
  meter.sample(2, true, 2000, 3720000);
  assert(fabsf(meter.kwh - 0.00555556f) < 0.000001f);
  meter.sample(3, true, -1000, 0xfffffff0);
  meter.sample(3, true, -1000, 10000);
  assert(meter.kwh == 0);

  V2Config config;
  assert(v2_config_valid(config));
  assert(config.gen_daily_register == 62 && config.gen_daily_scale == 0.1f);
  assert(v2_gen_daily_uses_register(config));
  config.gen_daily_register = 65535;
  assert(v2_config_valid(config) && v2_gen_daily_uses_register(config));
  config.gen_daily_register = 0;
  config.gen_daily_register_enabled = true;
  assert(v2_config_valid(config) && !v2_gen_daily_uses_register(config));
  assert(!config.sleep_enabled && config.wake_seconds == 60 && !config.relay_enabled);
  config.sleep_enabled = true; config.sleep_end = config.sleep_start;
  assert(!v2_config_valid(config));
  config = V2Config{}; config.relay_threshold = std::numeric_limits<float>::quiet_NaN();
  assert(!v2_config_valid(config));
  config = V2Config{}; config.hc_end[1] = 1440;
  assert(!v2_config_valid(config));
  puts("PASS: schedules (1440 minutes), tariff matrix (128 cases), relay, sleep, GEN energy, validation");
}
