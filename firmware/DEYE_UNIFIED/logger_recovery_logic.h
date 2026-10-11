#pragma once
#include <stdint.h>

static constexpr uint32_t LOGGER_RECOVERY_OFFLINE_MS = 60000;
static constexpr uint32_t LOGGER_RECOVERY_RETRY_MS = 300000;
static constexpr uint8_t LOGGER_RECOVERY_FAILURES = 3;
static inline bool logger_recovery_serial_valid(uint32_t serial) {
  return serial != 0 && serial != 123456789UL;
}

// Only the Solarman task owns this policy. Durations use unsigned subtraction
// so that recovery keeps working across millis() wraparound.
struct LoggerRecoveryPolicy {
  uint32_t last_response = 0, last_attempt_finished = 0;
  uint8_t failures = 0;
  bool attempted = false;
  void reset(uint32_t now) { *this = {}; last_response = now; }
  void observe(uint32_t now, bool logger_replied) {
    if (logger_replied) { last_response = now; failures = 0; }
    else if (failures < UINT8_MAX) ++failures;
  }
  void wifi_unavailable(uint32_t now) { last_response = now; failures = 0; }
  bool due(uint32_t now, uint32_t serial, bool wifi, bool busy, bool paused) const {
    return logger_recovery_serial_valid(serial) && wifi && !busy && !paused &&
      failures >= LOGGER_RECOVERY_FAILURES && uint32_t(now - last_response) >= LOGGER_RECOVERY_OFFLINE_MS &&
      (!attempted || uint32_t(now - last_attempt_finished) >= LOGGER_RECOVERY_RETRY_MS);
  }
  void attempt_finished(uint32_t now) { last_attempt_finished = now; attempted = true; }
};
