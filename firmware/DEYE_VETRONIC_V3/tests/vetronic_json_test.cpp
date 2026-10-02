#define DEYE_V2_CONFIG_TEST
#include <string>
#include <assert.h>
#include <stdio.h>
#include "../vetronic_data.h"
using String = std::string;
static bool cfg_ev_charger_enabled = true;
static VtSnapshot sample;
static VtSnapshot vt_snapshot() { return sample; }
#include "../vetronic_json.h"

int main() {
  DynamicJsonDocument doc(4096);
  sample.online = true; sample.measured = true; sample.mode = VT_LEGACY;
  sample.target = -1; sample.limit = 32; sample.amps = 10.5;
  assert(!deserializeJson(doc, vt_status_json()));
  assert(doc["enabled"].as<bool>());
  assert(doc["mode"].as<std::string>() == "legacy");
  assert(doc["mode_label"].as<std::string>() == "Main rendue a la borne");
  assert(doc["target_a"].as<int>() == -1 && doc["power_w"].as<int>() == 2415);
  assert(doc["power_estimated"].as<bool>());
  sample.measured = false;
  assert(!deserializeJson(doc, vt_status_json()));
  assert(doc["amps"].isNull() && doc["power_w"].isNull());
  sample.online = false; sample.confirmed = true;
  assert(!deserializeJson(doc, vt_status_json()));
  assert(doc["target_a"].isNull() && doc["manual_limit_a"].isNull());
  assert(!doc["confirmed"].as<bool>());
  assert(doc["mode_label"].as<std::string>() == "Indisponible");
  sample.tariff_pause = true; sample.manual_allowed = false;
  assert(!deserializeJson(doc, vt_status_json()));
  assert(doc["tariff_pause"].as<bool>() && !doc["manual_allowed"].as<bool>());
  puts("PASS: WB01 JSON, return-control label, measured/estimated watts, offline nulls and tariff status");
}
