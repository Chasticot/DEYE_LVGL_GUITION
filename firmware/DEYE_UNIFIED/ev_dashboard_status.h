#pragma once
#include "ev_backend.h"
#include "vetronic_data.h"
#include "ve_deye.h"

enum class EvCableState : uint8_t { Unknown, Disconnected, Connected };
struct EvDashboardIndicators {
  bool show_cable = false;
  bool solar = false;
  EvCableState cable = EvCableState::Unknown;
};

// Use the fresh snapshots returned by vt_snapshot/deye_copy_ev_snapshot.
static inline EvDashboardIndicators ev_dashboard_indicators(EvBackend backend,
    const VtSnapshot &wb01, const EvDeyeData &deye) {
  EvDashboardIndicators result;
  if (backend == EvBackend::VetronicWb01) {
    result.show_cable = true;
    result.solar = wb01.online && wb01.mode == VT_SOLAR;
    if (wb01.online && wb01.measured) {
      if (wb01.state == 0) result.cable = EvCableState::Disconnected;
      else if (wb01.state == 1 || wb01.state == 2) result.cable = EvCableState::Connected;
    }
  } else if (backend == EvBackend::DeyeLoRa) {
    result.solar = deye.valid && deye_ev_mode_write_supported(deye.mode_raw) &&
      (deye.mode_raw & 3) == 1;
    // R489/R490 do not report whether the vehicle is plugged in.
  }
  return result;
}
