#include <assert.h>
#include <stdio.h>
#include <limits>
#include <initializer_list>
#include "../vetronic_tariff.h"

int main() {
  assert(vt_parse_mode("legacy") == VT_LEGACY);
  assert(vt_equal(vt_mode_key(VT_LEGACY), "legacy"));
  assert(vt_equal(vt_mode_label(VT_LEGACY), "Main rendue a la borne"));
  assert(vt_parse_mode("invalid") == VT_UNKNOWN);
  assert(vt_manual_allowed(6, 32) && vt_manual_allowed(32, 32));
  assert(!vt_manual_allowed(5, 32) && !vt_manual_allowed(33, 32));
  assert(vt_gateway_manual_limit(32, 32, true) == 32);
  assert(vt_gateway_manual_limit(32, 16, true) == 16);
  assert(vt_gateway_manual_limit(32, 0, false) == 32);
  assert(vt_gateway_manual_limit(32, 0, true) == 0);
  assert(vt_gateway_manual_limit(63, 63, true) == 32);
  assert(vt_power_w(10.5f) == 2415);
  assert(vt_measurement_valid(true, 0.1f, 0));
  assert(!vt_measurement_valid(false, 0.1f, 0));
  assert(!vt_measurement_valid(true, std::numeric_limits<float>::quiet_NaN(), 2));
  assert(!vt_measurement_valid(true, std::numeric_limits<float>::infinity(), 2));
  assert(!vt_measurement_valid(true, -1, 2));
  assert(!vt_measurement_valid(true, 64, 2));
  assert(!vt_measurement_valid(true, 10, 3));
  assert(vt_fresh(100, UINT32_MAX - 100));
  assert(!vt_fresh(12100, 100));
  assert(vt_soc_guard_valid(0, 5) && vt_soc_guard_valid(95, 100));
  assert(!vt_soc_guard_valid(30, 34) && !vt_soc_guard_valid(-1, 35));
  assert(vt_soc_stop_adjust(30, 35, 1) == 30);
  assert(vt_soc_stop_adjust(30, 35, -1) == 29);
  assert(vt_soc_resume_adjust(30, 35, -1) == 35);
  assert(vt_soc_resume_adjust(95, 100, 1) == 100);
  int value = -1;
  assert(vt_parse_unsigned("32", 32, value) && value == 32);
  for (const char *bad : {"", "-6", "+6", "6.0", "6junk", "33", "999999999999999999", " 6", "6 "})
    assert(!vt_parse_unsigned(bad, 32, value));

  VtTariffState t;
  // Restart and external manual charge: no automatic command or resumption.
  assert(t.next(true, false, true, VT_MANUAL, 20) == VT_TARIFF_NONE);
  t.manual_confirmed(20);
  assert(t.next(true, true, true, VT_MANUAL, 20) == VT_TARIFF_NONE);
  assert(t.next(true, false, true, VT_MANUAL, 20) == VT_TARIFF_PAUSE);
  assert(t.expected(VT_TARIFF_PAUSE, VT_MANUAL, 20));
  assert(!t.expected(VT_TARIFF_PAUSE, VT_LEGACY, -1));
  t.completed(VT_TARIFF_PAUSE, true);
  assert(t.paused && t.amps == 20);
  assert(t.next(true, true, false, VT_STOP, 0) == VT_TARIFF_NONE);
  assert(t.next(true, false, true, VT_STOP, 0) == VT_TARIFF_NONE);
  assert(t.next(true, true, true, VT_STOP, 0) == VT_TARIFF_RESUME);
  t.completed(VT_TARIFF_RESUME, true);
  assert(t.manual_owned && !t.paused);
  assert(t.next(true, false, true, VT_MANUAL, 20) == VT_TARIFF_PAUSE);
  t.completed(VT_TARIFF_PAUSE, true);
  // Returning control cancels a pending tariff restart.
  t.cancel();
  assert(t.next(true, true, true, VT_LEGACY, -1) == VT_TARIFF_NONE);
  assert(t.next(true, true, true, VT_STOP, 0) == VT_TARIFF_NONE);
  // A gateway-side mode/current change hands control back to its operator.
  t.manual_confirmed(16);
  assert(t.next(true, false, true, VT_MANUAL, 10) == VT_TARIFF_NONE);
  assert(!t.manual_owned);
  t.manual_confirmed(16); t.completed(VT_TARIFF_PAUSE, true);
  assert(t.next(true, true, true, VT_SOLAR, 6) == VT_TARIFF_NONE);
  assert(!t.paused);
  // A lost or rejected POST is never repeated automatically.
  t.manual_confirmed(16); t.completed(VT_TARIFF_PAUSE, false);
  assert(t.next(true, false, true, VT_MANUAL, 16) == VT_TARIFF_NONE);
  t.manual_confirmed(16); t.completed(VT_TARIFF_PAUSE, true);
  assert(t.next(false, true, true, VT_STOP, 0) == VT_TARIFF_NONE);
  assert(!t.paused);
  puts("PASS: WB01 limits/measurements/SOC, input validation, tariffs, return control, reboot and uncertain POST");
}
