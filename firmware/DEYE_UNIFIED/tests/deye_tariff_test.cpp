#include <assert.h>
#include <initializer_list>
#include <stdio.h>
#include "../deye_ev_tariff.h"

int main() {
  constexpr uint16_t power = 32;

  // A fresh boot never takes control of a charge already present on the inverter.
  EvDeyeTariffState fresh;
  assert(fresh.next(true, false, 2, power) == 0);
  assert(fresh.next(true, true, 1, power) == 0);
  assert(!fresh.manual_owned && !fresh.paused);
  assert(!fresh.expected(1, 2, power));
  assert(!fresh.expected(2, 1, power));

  // A confirmed local Libre command owns its exact power setting.
  fresh.local_confirmed(2, power);
  assert(fresh.manual_owned && !fresh.paused && fresh.power_raw == power);
  assert(fresh.next(true, true, 2, power) == 0);
  assert(fresh.next(true, false, 2, power) == 1);
  assert(fresh.expected(1, 2, power));
  assert(!fresh.expected(1, 1, power));
  assert(!fresh.expected(1, 2, power + 1));
  assert(!fresh.expected(2, 2, power));
  assert(!fresh.expected(0, 2, power));

  // Only a confirmed pause may resume, at the original power and expected mode.
  fresh.automatic_confirmed(1);
  assert(!fresh.manual_owned && fresh.paused);
  assert(fresh.next(true, false, 1, power) == 0);
  assert(fresh.next(true, true, 1, power) == 2);
  assert(fresh.expected(2, 1, power));
  assert(!fresh.expected(2, 1, power + 1));
  assert(!fresh.expected(2, 2, power));
  fresh.automatic_confirmed(2);
  assert(fresh.manual_owned && !fresh.paused);
  assert(fresh.next(true, false, 2, power) == 1);

  // A local non-Libre action relinquishes all automatic resumption.
  for (uint8_t mode : {uint8_t(0), uint8_t(1), uint8_t(3)}) {
    EvDeyeTariffState local;
    local.local_confirmed(2, power);
    local.automatic_confirmed(1);
    local.local_confirmed(mode, power);
    assert(!local.manual_owned && !local.paused && local.power_raw == 0);
    assert(local.next(true, true, 1, power) == 0);
  }

  // Detectable external mode/current changes cancel ownership during a charge.
  for (uint8_t mode : {uint8_t(0), uint8_t(1), uint8_t(3)}) {
    EvDeyeTariffState external;
    external.local_confirmed(2, power);
    assert(external.next(true, false, mode, power) == 0);
    assert(!external.manual_owned && !external.paused);
    assert(external.next(true, false, 2, power) == 0);
  }
  EvDeyeTariffState external_power;
  external_power.local_confirmed(2, power);
  assert(external_power.next(true, false, 2, power + 1) == 0);
  assert(external_power.next(true, false, 2, power) == 0);

  // The same protection applies while paused, before automatic resumption.
  for (uint8_t mode : {uint8_t(0), uint8_t(2), uint8_t(3)}) {
    EvDeyeTariffState external;
    external.local_confirmed(2, power);
    external.automatic_confirmed(1);
    assert(external.next(true, true, mode, power) == 0);
    assert(external.next(true, true, 1, power) == 0);
  }
  external_power.local_confirmed(2, power);
  external_power.automatic_confirmed(1);
  assert(external_power.next(true, true, 1, power + 1) == 0);
  assert(external_power.next(true, true, 1, power) == 0);

  // Disabling tariffs, a failed/uncertain write, or reboot cannot resume a charge.
  for (bool paused : {false, true}) {
    EvDeyeTariffState disabled;
    disabled.local_confirmed(2, power);
    if (paused) disabled.automatic_confirmed(1);
    assert(disabled.next(false, true, paused ? 1 : 2, power) == 0);
    assert(!disabled.manual_owned && !disabled.paused && disabled.power_raw == 0);
    assert(disabled.next(true, true, 1, power) == 0);
    assert(disabled.next(true, false, 2, power) == 0);

    EvDeyeTariffState failed;
    failed.local_confirmed(2, power);
    if (paused) failed.automatic_confirmed(1);
    failed.cancel(); // Called by the command processor on an uncertain result.
    assert(!failed.manual_owned && !failed.paused && failed.power_raw == 0);
    assert(failed.next(true, true, 1, power) == 0);
    assert(failed.next(true, false, 2, power) == 0);

    EvDeyeTariffState rebooted;
    assert(rebooted.next(true, true, 1, power) == 0);
    assert(rebooted.next(true, false, 2, power) == 0);
  }

  puts("PASS: Deye tariff ownership, confirmed pause/resume, external changes, disabling, failure and reboot");
}
