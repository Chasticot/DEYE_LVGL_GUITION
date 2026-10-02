#pragma once
#include <ArduinoJson.h>
#ifndef DEYE_V2_CONFIG_TEST
#include "vetronic.h"
#endif

static String vt_status_json() {
  const VtSnapshot s = vt_snapshot();
  StaticJsonDocument<2048> doc;
  doc["enabled"] = cfg_ev_charger_enabled;
  doc["online"] = s.online;
  doc["measured"] = s.measured;
  doc["busy"] = s.busy;
  doc["mode_label"] = vt_mode_label(s.online ? s.mode : VT_UNKNOWN);
  doc["mode"] = vt_mode_key(s.online ? s.mode : VT_UNKNOWN);
  doc["confirmed"] = s.online && s.confirmed;
  if (s.measured) { doc["amps"] = s.amps; doc["power_w"] = vt_power_w(s.amps); }
  else { doc["amps"] = nullptr; doc["power_w"] = nullptr; }
  doc["power_estimated"] = true;
  if (s.online) { doc["target_a"] = s.target; doc["manual_limit_a"] = s.limit; doc["soc"] = s.soc; }
  else { doc["target_a"] = nullptr; doc["manual_limit_a"] = nullptr; doc["soc"] = nullptr; }
  doc["soc_api"] = s.online && s.soc_control_api;
  doc["soc_guard"] = s.soc_guard;
  doc["soc_stop"] = s.soc_stop;
  doc["soc_resume"] = s.soc_resume;
  doc["soc_blocked"] = s.online && s.soc_blocked;
  doc["tariff_pause"] = s.tariff_pause;
  doc["manual_allowed"] = s.manual_allowed;
  doc["message"] = s.message;
  doc["result"] = s.result;
  String result;
  serializeJson(doc, result);
  return result;
}
