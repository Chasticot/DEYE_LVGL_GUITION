#pragma once
#include "vetronic_protocol.h"

enum VtTariffAction : uint8_t { VT_TARIFF_NONE, VT_TARIFF_PAUSE, VT_TARIFF_RESUME };

// Only manual charges confirmed after a local action belong to this screen.
// Returning control, another local mode, an external mode/current change,
// a reboot or an uncertain POST abandons automatic resumption.
struct VtTariffState {
  bool manual_owned = false, paused = false;
  int amps = 0;
  void cancel() { manual_owned = false; paused = false; amps = 0; }
  void manual_confirmed(int current) { cancel(); manual_owned = true; amps = current; }
  VtTariffAction next(bool enabled, bool allowed, bool online, VtMode mode, int target) {
    if (!enabled) { cancel(); return VT_TARIFF_NONE; }
    if (!online) return VT_TARIFF_NONE;
    if ((paused && mode != VT_STOP) || (manual_owned && (mode != VT_MANUAL || target != amps))) {
      cancel(); return VT_TARIFF_NONE;
    }
    if (manual_owned && !allowed) return VT_TARIFF_PAUSE;
    if (paused && allowed) return VT_TARIFF_RESUME;
    return VT_TARIFF_NONE;
  }
  bool expected(VtTariffAction action, VtMode mode, int target) const {
    return action == VT_TARIFF_PAUSE ? manual_owned && mode == VT_MANUAL && target == amps :
      action == VT_TARIFF_RESUME && paused && mode == VT_STOP;
  }
  void completed(VtTariffAction action, bool confirmed) {
    if (!confirmed) { cancel(); return; }
    paused = action == VT_TARIFF_PAUSE;
    manual_owned = action == VT_TARIFF_RESUME;
  }
};
