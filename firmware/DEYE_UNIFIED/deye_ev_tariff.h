#pragma once
#include <stdint.h>

// Only a Libre mode confirmed after a local command during this boot belongs
// to the screen. A reboot, failed write or detectable external change cancels it.
struct EvDeyeTariffState {
  bool manual_owned = false, paused = false;
  uint16_t power_raw = 0;
  void cancel() { manual_owned = paused = false; power_raw = 0; }
  void local_confirmed(uint8_t mode, uint16_t power) {
    cancel();
    if (mode == 2) { manual_owned = true; power_raw = power; }
  }
  uint8_t next(bool enabled, bool allowed, uint8_t mode, uint16_t power) {
    if (!enabled) { cancel(); return 0; }
    if ((manual_owned && (mode != 2 || power != power_raw)) ||
        (paused && (mode != 1 || power != power_raw))) { cancel(); return 0; }
    return manual_owned && !allowed ? 1 : paused && allowed ? 2 : 0;
  }
  bool expected(uint8_t target, uint8_t mode, uint16_t power) const {
    return power == power_raw && (target == 1 ? manual_owned && mode == 2 : target == 2 && paused && mode == 1);
  }
  void automatic_confirmed(uint8_t target) {
    manual_owned = target == 2; paused = target == 1;
  }
};
