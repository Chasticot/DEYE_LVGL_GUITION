#define DEYE_V2_CONFIG_TEST
#include <string>
#include <assert.h>
#include <stdio.h>
#include "../v2_logic.h"
using String = std::string;
static V2Config cfg_v2;
#include "../web_v2.h"

int main() {
  DynamicJsonDocument charging(512);
  EvBackend backend = EvBackend::None;
  bool ambiguous = false;
  for (const char *key : {"none", "deye_lora", "vetronic_wb01"}) {
    charging["ev_backend"] = key;
    charging["ev_enabled"] = strcmp(key, "none") != 0;
    assert(web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
    assert(strcmp(ev_backend_key(backend), key) == 0 && !ambiguous);
  }
  assert(web_ev_backend_parse(charging.as<JsonObjectConst>(), false, backend, ambiguous));
  charging["ev_backend"] = "deye_lora";
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), false, backend, ambiguous));
  charging["ev_backend"] = "none";
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  charging["ev_enabled"] = false;
  charging["ev_backend"] = "vetronic_wb01";
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  charging["ev_backend"] = "unknown";
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  charging["ev_backend"] = 2;
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  charging["ev_backend"] = nullptr;
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  charging.remove("ev_backend");
  assert(web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous) && backend == EvBackend::None);
  charging["ev_enabled"] = true;
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous) && ambiguous);
  charging["ev_enabled"] = "false";
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous) && !ambiguous);
  charging.remove("ev_enabled");
  assert(!web_ev_backend_parse(charging.as<JsonObjectConst>(), true, backend, ambiguous));
  cfg_v2.pv_visible[1] = false;
  cfg_v2.pv4_visible = false;
  cfg_v2.show_gen_daily = true;
  cfg_v2.tariff_mode = 1;
  cfg_v2.sleep_enabled = true;
  cfg_v2.relay_coefficient = -0.125f;
  cfg_v2.relay_threshold = -1200.5f;
  const String json = web_v2_json();
  DynamicJsonDocument doc(4096);
  assert(!deserializeJson(doc, json));
  V2Config restored;
  assert(web_v2_parse(doc.as<JsonObjectConst>(), restored));
  assert(!restored.pv_visible[1] && restored.pv_visible[0]);
  assert(!restored.pv4_visible);
  assert(restored.show_gen_daily);
  doc["show_gen_daily"] = "true";
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc.remove("show_gen_daily");
  restored.show_gen_daily = true;
  assert(web_v2_parse(doc.as<JsonObjectConst>(), restored) && !restored.show_gen_daily);
  doc.remove("pv4_visible");
  restored.pv4_visible = true;
  assert(web_v2_parse(doc.as<JsonObjectConst>(), restored) && restored.pv4_visible);
  assert(restored.sleep_enabled && restored.tariff_mode == 1);
  assert(restored.relay_threshold == -1200.5f && restored.relay_coefficient == -0.125f);
  assert(!doc.containsKey("gen_daily_register_enabled"));
  doc["gen_daily_register"] = 536;
  doc["gen_daily_scale"] = 0.01;
  doc["gen_daily_register_enabled"] = false; // Old exports must not override the address.
  assert(web_v2_parse(doc.as<JsonObjectConst>(), restored));
  assert(v2_gen_daily_uses_register(restored) && restored.gen_daily_register == 536);
  assert(fabsf(restored.gen_daily_scale - 0.01f) < 0.00001f);
  doc["gen_daily_register"] = 0;
  doc["gen_daily_register_enabled"] = true;
  assert(web_v2_parse(doc.as<JsonObjectConst>(), restored));
  assert(!v2_gen_daily_uses_register(restored));
  doc["gen_daily_register"] = 65536;
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc["gen_daily_register"] = 0;
  doc["gen_daily_scale"] = 0;
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc["gen_daily_scale"] = 0.1;
  doc["wake_seconds"] = 0;
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc["wake_seconds"] = 60.5;
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc["wake_seconds"] = 60;
  doc["relay_enabled"] = "false";
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  doc["relay_enabled"] = false;
  doc.remove("sleep_start");
  assert(!web_v2_parse(doc.as<JsonObjectConst>(), restored));
  assert(deserializeJson(doc, "{broken"));
  puts("PASS: configuration JSON round-trip, strict charging backend types/compatibility, legacy ambiguity, missing fields, bounds, malformed input");
}
