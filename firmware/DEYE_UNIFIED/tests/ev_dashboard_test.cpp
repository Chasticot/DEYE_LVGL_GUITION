#include <assert.h>
#include <stdio.h>
#include <initializer_list>
#include "../ev_dashboard_status.h"
int main() {
  VtSnapshot wb;
  EvDeyeData native_ev = {};
  auto status = [&]() { return ev_dashboard_indicators(EvBackend::VetronicWb01, wb, native_ev); };
  assert(status().show_cable && !status().solar && status().cable == EvCableState::Unknown);
  wb.online = wb.measured = true; wb.mode = VT_SOLAR;
  for (int state = 0; state <= 2; ++state) {
    wb.state = state;
    assert(status().solar);
    assert(status().cable == (state == 0 ? EvCableState::Disconnected : EvCableState::Connected));
  }
  wb.amps = 0; wb.state = 1; wb.busy = true; wb.confirmed = false;
  assert(status().cable == EvCableState::Connected && status().solar);
  for (auto mode : {VT_MANUAL, VT_STOP, VT_LEGACY, VT_UNKNOWN}) {
    wb.mode = mode; assert(!status().solar);
  }
  wb.mode = VT_SOLAR; wb.measured = false;
  assert(status().solar && status().cable == EvCableState::Unknown);
  wb.measured = true; wb.state = 3;
  assert(status().cable == EvCableState::Unknown);
  wb.state = 2; wb.online = false;
  assert(!status().solar && status().cable == EvCableState::Unknown);
  native_ev.valid = true; native_ev.mode_raw = 0x5F01;
  auto native_status = [&]() { return ev_dashboard_indicators(EvBackend::DeyeLoRa, wb, native_ev); };
  assert(native_status().solar && !native_status().show_cable);
  for (auto raw : {0x5F02, 0x5A00, 0x0001, 0x5F00, 0x5F03}) {
    native_ev.mode_raw = raw; assert(!native_status().solar);
  }
  native_ev.mode_raw = 0x5F01; native_ev.valid = false;
  assert(!native_status().solar);
  wb.online = true; native_ev.valid = true;
  const auto none = ev_dashboard_indicators(EvBackend::None, wb, native_ev);
  assert(!none.solar && !none.show_cable);
  puts("EV dashboard indicators: OK (cable, solar, stale data, both backends)");
}
