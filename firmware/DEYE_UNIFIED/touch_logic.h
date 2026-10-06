#pragma once

#include <stdint.h>

// No fresh frame is different from an explicit finger release.
enum class TouchSample { Pending, Released, Pressed, Invalid };

static constexpr TouchSample gt911_status_sample(uint8_t status) {
  if (!(status & 0x80)) return TouchSample::Pending;
  const uint8_t contacts = status & 0x0f;
  return contacts == 0 ? TouchSample::Released
    : contacts <= 5 ? TouchSample::Pressed : TouchSample::Invalid;
}

static bool gt911_decode_point(const uint8_t *point, uint16_t width, uint16_t height,
                               int16_t &x, int16_t &y) {
  const uint16_t raw_x = uint16_t(point[1]) | (uint16_t(point[2]) << 8);
  const uint16_t raw_y = uint16_t(point[3]) | (uint16_t(point[4]) << 8);
  // Reject corrupt coordinates instead of turning them into an edge touch.
  if (raw_x >= width || raw_y >= height) return false;
  x = int16_t(raw_x);
  y = int16_t(raw_y);
  return true;
}

struct TouchWakeFilter {
  static constexpr uint32_t CONFIRM_MS = 80;
  static constexpr uint32_t MAX_SAMPLE_GAP_MS = 120;
  uint32_t started_at = 0;
  uint32_t last_sample_at = 0;
  uint8_t samples = 0;

  void reset() { samples = 0; }

  bool update(TouchSample sample, uint32_t now) {
    if (sample == TouchSample::Released || sample == TouchSample::Invalid) {
      reset();
      return false;
    }
    if (samples && uint32_t(now - last_sample_at) > MAX_SAMPLE_GAP_MS) reset();
    if (sample == TouchSample::Pending) return false;
    if (!samples) started_at = now;
    last_sample_at = now;
    if (samples < 3) ++samples;
    // Only fresh, acknowledged frames count. One phantom frame cannot wake up
    // the screen, even if LVGL keeps polling it for longer than CONFIRM_MS.
    return samples >= 3 && uint32_t(now - started_at) >= CONFIRM_MS;
  }
};
