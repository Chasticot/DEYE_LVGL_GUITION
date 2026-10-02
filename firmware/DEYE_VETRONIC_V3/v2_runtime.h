#pragma once
#include "settings.h"
#include "app_data.h"
#include "ntp_manager.h"
#include "tempo_api.h"

static bool v2_gen_included() { return inverter_profile().gen_supported && cfg_v2.add_gen && !cfg_gen_smartload; }
static uint32_t v2_pv_power(const DashboardData &data) {
  uint32_t total = (cfg_v2.pv_visible[0] ? data.pv1_w : 0) +
    (cfg_v2.pv_visible[1] ? data.pv2_w : 0) + (cfg_v2.pv_visible[2] ? data.pv3_w : 0) + (cfg_v2.pv4_visible ? data.pv4_w : 0);
  if (v2_gen_included() && data.gen_power > 0) total += data.gen_power;
  return total;
}
static int v2_day_key(const struct tm &local) { return (local.tm_year + 1900) * 1000 + local.tm_yday; }
static int v2_tempo_day(time_t timestamp) {
  struct tm local = {};
  localtime_r(&timestamp, &local);
  if (local.tm_hour < 6) --local.tm_mday;
  local.tm_hour = 12; local.tm_isdst = -1;
  mktime(&local);
  return v2_day_key(local);
}

struct V2Tariff {
  bool valid;
  bool hc;
  bool grid_allowed;
  bool red;
};
static V2Tariff v2_tariff() {
  struct tm local = {};
  const bool clock_ok = ntp_received.load() && getLocalTime(&local, 0);
  const unsigned minute = local.tm_hour * 60 + local.tm_min;
  const TempoNow tempo = tempo_api_get_now();
  bool tempo_ok = tempo.valid && clock_ok && tempo.observed_at > 0;
  if (tempo_ok) {
    // A Tempo day changes at 06:00. Never reuse yesterday's colour.
    tempo_ok = v2_tempo_day(tempo.observed_at) == v2_tempo_day(time(nullptr));
  }
  const bool hc = cfg_v2.tariff_mode == 1 ? v2_in_window(minute, 1320, 360) :
    v2_in_window(minute, cfg_v2.hc_start[0], cfg_v2.hc_end[0]) ||
    v2_in_window(minute, cfg_v2.hc_start[1], cfg_v2.hc_end[1]);
  const bool valid = clock_ok && (cfg_v2.tariff_mode != 1 || tempo_ok);
  const bool allowed = v2_grid_allowed(cfg_v2.ev_tariff_enabled, clock_ok, cfg_v2.ev_hc_only,
    hc, cfg_v2.tariff_mode == 1 && cfg_v2.ev_block_red_hp, tempo_ok, tempo.color_code == 3);
  return {valid, hc, allowed, tempo_ok && tempo.color_code == 3};
}

struct V2Measurements {
  bool gen_valid = false;
  int32_t gen_power = 0;
  float gen_kwh = 0;
  bool energy_valid = false;
  int energy_day = 0;
  uint32_t gen_ms = 0;
  V2RelaySample relay;
};
static portMUX_TYPE v2_measure_lock = portMUX_INITIALIZER_UNLOCKED;
static V2Measurements v2_measurements;
static V2Measurements v2_measure_snapshot() {
  portENTER_CRITICAL(&v2_measure_lock);
  V2Measurements result = v2_measurements;
  portEXIT_CRITICAL(&v2_measure_lock);
  const uint32_t now = millis();
  result.gen_valid = result.gen_valid && uint32_t(now - result.gen_ms) < 45000;
  return result;
}

static bool v2_daily_kwh(uint16_t pv_raw, bool pv_valid, float &result) {
  const bool pv = cfg_v2.pv_visible[0] || cfg_v2.pv_visible[1] ||
    (inverter_profile().pv_count >= 3 && cfg_v2.pv_visible[2]) || (inverter_profile().pv_count >= 4 && cfg_v2.pv4_visible);
  result = pv ? pv_raw * 0.1f : 0;
  if (pv && !pv_valid) return false;
  if (v2_gen_included()) {
    const V2Measurements m = v2_measure_snapshot();
    struct tm local = {};
    if (!ntp_received.load() || !getLocalTime(&local, 0) || !m.energy_valid ||
        m.energy_day != v2_day_key(local) || !m.gen_valid) return false;
    result += m.gen_kwh;
  }
  return true;
}

static constexpr uint8_t V2_RELAY_GPIO = 40;
static V2RelayState v2_relay;
static void v2_relay_begin() {
  digitalWrite(V2_RELAY_GPIO, LOW);
  pinMode(V2_RELAY_GPIO, OUTPUT);
}
static void v2_relay_process() {
  const V2Measurements m = v2_measure_snapshot();
  const bool valid = cfg_v2.relay_enabled && m.relay.valid(millis());
  const bool wanted = valid && v2_compare(m.relay.value, cfg_v2.relay_comparator, cfg_v2.relay_threshold);
  digitalWrite(V2_RELAY_GPIO, v2_relay.update(valid, wanted, millis(), uint32_t(cfg_v2.relay_delay_s) * 1000) ? HIGH : LOW);
}
