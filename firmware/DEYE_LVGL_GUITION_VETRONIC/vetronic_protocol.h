#pragma once
#include <stdint.h>
#include <math.h>
#include <string.h>

enum VtMode : uint8_t { VT_STOP, VT_MANUAL, VT_SOLAR, VT_LEGACY, VT_UNKNOWN };
static constexpr const char *vt_mode_key(VtMode mode) {
  return mode == VT_STOP ? "stop" : mode == VT_MANUAL ? "manual" :
    mode == VT_SOLAR ? "solar" : mode == VT_LEGACY ? "legacy" : "unknown";
}
static constexpr bool vt_equal(const char *a, const char *b) {
  return a && b && (*a == *b) && (!*a || vt_equal(a + 1, b + 1));
}
static constexpr VtMode vt_parse_mode(const char *text) {
  return vt_equal(text, "stop") ? VT_STOP : vt_equal(text, "manual") ? VT_MANUAL :
    vt_equal(text, "solar") ? VT_SOLAR : vt_equal(text, "legacy") ? VT_LEGACY : VT_UNKNOWN;
}
// Installation 32 A confirmed by the owner. The API's heuristic native limit
// must not shrink the manual control. The gateway still validates each command.
static constexpr int vt_current_limit(int installation) {
  return installation >= 6 && installation <= 32 ? installation : 0;
}
static constexpr bool vt_measurement_valid(bool fresh, float amps, int state) {
  return fresh && isfinite(amps) && amps >= 0 && amps <= 63 && state >= 0 && state <= 2;
}
static constexpr bool vt_manual_allowed(int amps, int limit) { return amps >= 6 && amps <= limit; }
// The gateway uses a 5 % hysteresis: solar charging stops at the lower
// threshold and is permitted again only after the upper threshold is reached.
static constexpr bool vt_soc_guard_valid(int stop, int resume) {
  return stop >= 0 && stop <= 95 && resume >= 5 && resume <= 100 && resume >= stop + 5;
}
// Adjust each threshold independently, keeping at least five percentage points.
static constexpr int vt_soc_stop_adjust(int stop, int resume, int delta) {
  return stop + delta < 0 ? 0 : stop + delta > resume - 5 ? resume - 5 : stop + delta;
}
static constexpr int vt_soc_resume_adjust(int stop, int resume, int delta) {
  return resume + delta > 100 ? 100 : resume + delta < stop + 5 ? stop + 5 : resume + delta;
}
static constexpr bool vt_fresh(uint32_t now, uint32_t at) { return uint32_t(now - at) < 12000; }
static constexpr unsigned vt_power_w(float measuredAmps) { return unsigned(measuredAmps * 230.0f + 0.5f); }
