#pragma once
#include <stdint.h>
#include <math.h>

// Half-open local-time intervals. Equal endpoints disable an interval.
static constexpr bool v2_in_window(unsigned minute, unsigned start, unsigned end) {
  return start != end && (start < end ? minute >= start && minute < end : minute >= start || minute < end);
}
static constexpr bool v2_compare(float value, unsigned op, float threshold) {
  return op == 0 ? value < threshold : op == 1 ? value == threshold : value > threshold;
}
static constexpr bool v2_grid_allowed(bool enabled, bool clock_valid, bool hc_only,
                                      bool hc, bool red_block, bool tempo_valid, bool red) {
  return !enabled || (clock_valid && (!hc_only || hc) && (!red_block || (tempo_valid && (!red || hc))));
}

struct V2Config {
  uint32_t version = 1;
  bool pv_visible[3] = {true, true, true};
  bool add_gen = false;
  bool gen_daily_register_enabled = false; // Legacy NVS layout only; address now selects the source.
  uint16_t gen_daily_register = 62;
  float gen_daily_scale = 0.1f;
  uint8_t tariff_mode = 0; // 0: personal windows, 1: Tempo
  uint16_t hc_start[2] = {1320, 0};
  uint16_t hc_end[2] = {360, 0};
  bool ev_tariff_enabled = false;
  bool ev_hc_only = false;
  bool ev_block_red_hp = true;
  bool relay_enabled = false;
  uint16_t relay_register = 184;
  bool relay_signed = false;
  float relay_coefficient = 1.0f;
  uint8_t relay_comparator = 2;
  float relay_threshold = 80;
  uint16_t relay_delay_s = 5;
  bool sleep_enabled = false;
  uint16_t sleep_start = 1380;
  uint16_t sleep_end = 420;
  uint16_t wake_seconds = 60;
};
static inline bool v2_gen_daily_uses_register(const V2Config &c) {
  return c.gen_daily_register != 0;
}
static inline bool v2_config_valid(const V2Config &c) {
  return c.version == 1 && c.tariff_mode <= 1 && c.relay_comparator <= 2 &&
    c.hc_start[0] < 1440 && c.hc_start[1] < 1440 && c.hc_end[0] < 1440 && c.hc_end[1] < 1440 &&
    c.sleep_start < 1440 && c.sleep_end < 1440 && (!c.sleep_enabled || c.sleep_start != c.sleep_end) &&
    c.wake_seconds >= 1 && c.wake_seconds <= 3600 && c.relay_delay_s <= 3600 &&
    isfinite(c.relay_threshold) && fabsf(c.relay_threshold) <= 6553500 &&
    isfinite(c.relay_coefficient) && fabsf(c.relay_coefficient) <= 100 &&
    isfinite(c.gen_daily_scale) && c.gen_daily_scale > 0 && c.gen_daily_scale <= 100;
}

struct V2RelaySample {
  bool received = false;
  float value = 0;
  uint32_t at = 0;
  void accept(bool ok, float reading, uint32_t now) {
    if (!ok || !isfinite(reading)) return;
    received = true; value = reading; at = now;
  }
  bool valid(uint32_t now) const { return received && uint32_t(now - at) < 300000; }
};

static inline bool v2_response_recent(bool received, uint32_t last_success, uint32_t now, uint32_t timeout) {
  return received && uint32_t(now - last_success) <= timeout;
}

struct V2RelayState {
  bool output = false;
  bool pending = false;
  uint32_t since = 0;
  bool update(bool valid, bool requested, uint32_t now, uint32_t delay_ms) {
    if (!valid) { output = false; pending = false; since = now; return false; }
    if (requested != pending) { pending = requested; since = now; }
    if (pending != output && uint32_t(now - since) >= delay_ms) output = pending;
    return output;
  }
};

struct V2EnergyIntegrator {
  int day = 0;
  float kwh = 0;
  bool previous_valid = false;
  float previous_power = 0;
  uint32_t previous_ms = 0;
  void sample(int today, bool valid, float power, uint32_t now) {
    if (today != day) { day = today; kwh = 0; previous_valid = false; }
    const float watts = power > 0 ? power : 0;
    const uint32_t elapsed = now - previous_ms;
    if (valid && previous_valid && elapsed <= 45000)
      kwh += (watts + previous_power) * 0.5f * float(elapsed) / 3600000000.0f;
    previous_valid = valid;
    previous_ms = now;
    previous_power = watts;
  }
};

struct V2SleepState {
  bool sleeping = false;
  bool awake = false;
  bool was_due = false;
  uint32_t woke_at = 0;
  bool update(bool due, uint32_t now, uint32_t wake_ms) {
    // Entering the window (including first NTP sync) grants a full wake period.
    if (due && !was_due) { awake = true; woke_at = now; }
    was_due = due;
    if (!due || (awake && uint32_t(now - woke_at) >= wake_ms)) awake = false;
    sleeping = due && !awake;
    return sleeping;
  }
  void touch(bool due, uint32_t now) {
    if (due) { awake = true; woke_at = now; sleeping = false; }
  }
};
