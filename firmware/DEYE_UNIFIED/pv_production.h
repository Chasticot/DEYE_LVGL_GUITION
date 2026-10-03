#pragma once
#include "app_data.h"
#include "v2_logic.h"

// In GEN MO mode the port is a production source. Its configured sign is
// retained in raw diagnostics, but its magnitude is used like another PV.
static uint32_t v2_gen_production_w(int32_t signed_power) {
  return signed_power < 0 ? uint32_t(-int64_t(signed_power)) : uint32_t(signed_power);
}

static bool v2_gen_as_pv(const V2Config &config, bool supported, bool smartload) {
  return supported && config.add_gen && !smartload;
}

static uint32_t v2_production_power(const DashboardData &data, const V2Config &config,
                                    bool include_gen) {
  uint32_t total = (config.pv_visible[0] ? data.pv1_w : 0) +
    (config.pv_visible[1] ? data.pv2_w : 0) +
    (config.pv_visible[2] ? data.pv3_w : 0) +
    (config.pv4_visible ? data.pv4_w : 0);
  if (include_gen) total += v2_gen_production_w(data.gen_power);
  return total;
}

static bool v2_production_daily_kwh(const V2Config &config, uint8_t pv_count,
    uint16_t pv_raw, bool pv_valid, bool include_gen, bool gen_valid,
    float gen_kwh, float &result) {
  const bool pv = config.pv_visible[0] || config.pv_visible[1] ||
    (pv_count >= 3 && config.pv_visible[2]) || (pv_count >= 4 && config.pv4_visible);
  result = pv ? pv_raw * 0.1f : 0;
  if (pv && !pv_valid) return false;
  if (include_gen) {
    if (!gen_valid) return false;
    result += gen_kwh;
  }
  return true;
}
