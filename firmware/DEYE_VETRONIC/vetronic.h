#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings.h"
#include "vetronic_protocol.h"

struct VtSnapshot {
  bool online = false, measured = false, busy = false, confirmed = false;
  bool config_ready = false, soc_control_api = false, soc_guard = false, soc_blocked = false;
  uint32_t at = 0;
  VtMode mode = VT_UNKNOWN;
  float amps = 0;
  int target = -1, limit = 0, state = -1, soc = -1, soc_stop = 30, soc_resume = 35;
  int config_limit = 0;
  uint16_t reg_grid = 0, reg_load = 0, reg_soc = 0, reg_pv1 = 0, reg_pv2 = 0, reg_pv3 = 0, reg_bat = 0;
  float pv1_scale = 1, pv2_scale = 1, pv3_scale = 1, load_scale = 1, grid_scale = 1, bat_scale = 1, soc_scale = 1;
  bool includes_ev = false, meter_confirmed = false, third_mppt = false;
  char deye_host[16] = "", deye_serial[12] = "";
  char message[240] = "En attente de la passerelle VE TRONIC";
  char result[240] = "";
};
enum VtCommandType : uint8_t { VT_COMMAND_MODE, VT_COMMAND_SOC_GUARD };
struct VtCommand {
  VtCommandType type;
  VtMode mode;
  int amps, soc_stop, soc_resume;
  bool soc_guard;
  uint32_t at;
};
static VtSnapshot vt_data;
static SemaphoreHandle_t vt_mutex = nullptr;
static QueueHandle_t vt_queue = nullptr;
static bool vt_enabled = false;

static VtSnapshot vt_snapshot() {
  VtSnapshot s;
  if (!vt_mutex) return s;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  s = vt_data;
  xSemaphoreGive(vt_mutex);
  s.online = s.online && cfg_ev_charger_enabled && WiFi.status() == WL_CONNECTED && vt_fresh(millis(), s.at);
  s.measured = s.measured && s.online;
  return s;
}
static bool vt_is_enabled() {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  bool enabled = vt_enabled;
  xSemaphoreGive(vt_mutex);
  return enabled;
}
static String vt_host_snapshot() {
  if (!vt_mutex) return cfg_vetronic_host;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  String host = cfg_vetronic_host;
  xSemaphoreGive(vt_mutex);
  return host;
}
static bool vt_save_host(const char *host) {
  if (!host || !vt_mutex) return false;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  const bool saved = settings_save_vetronic_host(String(host));
  if (saved) {
    vt_data.online = false;
    vt_data.measured = false;
    vt_data.at = 0;
    strlcpy(vt_data.message, "Nouvelle adresse enregistree, connexion en cours", sizeof(vt_data.message));
  }
  xSemaphoreGive(vt_mutex);
  return saved;
}
// Called on the UI thread. Network I/O only runs in vt_task.
static void vt_sync_enabled() {
  if (!vt_mutex) return;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  vt_enabled = cfg_ev_charger_enabled;
  xSemaphoreGive(vt_mutex);
}
static bool vt_submit(VtMode mode, int amps) {
  if (!vt_mutex || !vt_queue || !cfg_ev_charger_enabled || WiFi.status() != WL_CONNECTED || mode >= VT_UNKNOWN) return false;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  bool ready = !vt_data.busy;
  if (mode == VT_MANUAL) ready = ready && vt_data.online && vt_fresh(millis(), vt_data.at) && vt_manual_allowed(amps, vt_data.limit);
  if (ready) {
    VtCommand c = { VT_COMMAND_MODE, mode, amps, 0, 0, false, millis() };
    vt_data.busy = true;
    strlcpy(vt_data.result, "Commande en attente...", sizeof(vt_data.result));
    ready = xQueueSend(vt_queue, &c, 0) == pdTRUE;
    if (!ready) vt_data.busy = false;
  }
  xSemaphoreGive(vt_mutex);
  return ready;
}
static bool vt_submit_soc_guard(bool enabled, int stop, int resume) {
  if (!vt_mutex || !vt_queue || !cfg_ev_charger_enabled || WiFi.status() != WL_CONNECTED || !vt_soc_guard_valid(stop, resume)) return false;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  bool ready = !vt_data.busy && vt_data.online && vt_data.soc_control_api;
  if (ready) {
    VtCommand c = { VT_COMMAND_SOC_GUARD, VT_UNKNOWN, 0, stop, resume, enabled, millis() };
    vt_data.busy = true;
    strlcpy(vt_data.result, "Mise a jour de la protection SOC...", sizeof(vt_data.result));
    if (xQueueSend(vt_queue, &c, 0) != pdTRUE) {
      vt_data.busy = false;
      ready = false;
    }
  }
  xSemaphoreGive(vt_mutex);
  return ready;
}
static void vt_http_begin(HTTPClient &http, WiFiClient &client, const char *path) {
  const String host = vt_host_snapshot();
  http.begin(client, String("http://") + host + ":" + String(VETRONIC_HTTP_PORT) + path);
  http.useHTTP10(true); // JSON stream without chunk framing.
  http.setConnectTimeout(2000);
  http.setTimeout(12000);
  client.setTimeout(12000);
}
static bool vt_read_status(VtSnapshot &s, String &token) {
  s.online = false;
  s.measured = false;
  token = "";
  if (WiFi.status() != WL_CONNECTED) {
    strlcpy(s.message, "Wi-Fi de l'ecran deconnecte", sizeof(s.message));
    return false;
  }
  WiFiClient client;
  HTTPClient http;
  vt_http_begin(http, client, "/api/status");
  int code = http.GET();
  if (code != 200) {
    snprintf(s.message, sizeof(s.message), "Passerelle indisponible (HTTP %d)", code);
    http.end();
    return false;
  }
  StaticJsonDocument<1024> filter;
  for (const char *key : {"token", "mode", "message", "wbValid", "amps", "state", "targetA", "limit", "nativeLimitA", "currentConfirmed",
                           "soc", "socGuard", "socStop", "socResume", "socBlocked", "host", "serial", "regGrid", "regLoad", "regSoc",
                           "regPv1", "regPv2", "regPv3", "regBat", "pv1Scale", "pv2Scale", "pv3Scale", "loadScale", "gridScale", "batScale",
                           "socScale", "includes", "meter", "pv3", "socGuardApi"}) filter[key] = true;
  DynamicJsonDocument doc(4096);
  auto error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
  http.end();
  VtMode mode = vt_parse_mode(doc["mode"] | "");
  const char *csrf = doc["token"] | "";
  if (error || mode == VT_UNKNOWN || !*csrf || strlen(csrf) > 96 || !doc["limit"].is<int>() || !doc["nativeLimitA"].is<int>()) {
    strlcpy(s.message, "API VE TRONIC incompatible ou reponse incomplete", sizeof(s.message));
    return false;
  }
  token = csrf;
  s.online = true;
  s.at = millis();
  s.mode = mode;
  s.limit = vt_current_limit(VETRONIC_MAX_CURRENT_A);
  s.amps = doc["amps"] | -1.0f;
  s.state = doc["state"] | -1;
  s.target = doc["targetA"] | -1;
  s.confirmed = doc["currentConfirmed"] | false;
  s.soc = doc["soc"] | -1;
  s.soc_guard = doc["socGuard"] | false;
  s.soc_stop = doc["socStop"] | 30;
  s.soc_resume = doc["socResume"] | 35;
  s.soc_blocked = doc["socBlocked"] | false;
  s.soc_control_api = doc["socGuardApi"] | false;
  s.config_limit = doc["limit"] | 0;
  strlcpy(s.deye_host, doc["host"] | "", sizeof(s.deye_host));
  strlcpy(s.deye_serial, doc["serial"] | "", sizeof(s.deye_serial));
  s.reg_grid = doc["regGrid"] | 0; s.reg_load = doc["regLoad"] | 0; s.reg_soc = doc["regSoc"] | 0;
  s.reg_pv1 = doc["regPv1"] | 0; s.reg_pv2 = doc["regPv2"] | 0; s.reg_pv3 = doc["regPv3"] | 0; s.reg_bat = doc["regBat"] | 0;
  s.pv1_scale = doc["pv1Scale"] | 0.0f; s.pv2_scale = doc["pv2Scale"] | 0.0f; s.pv3_scale = doc["pv3Scale"] | 0.0f;
  s.load_scale = doc["loadScale"] | 0.0f; s.grid_scale = doc["gridScale"] | 0.0f; s.bat_scale = doc["batScale"] | 0.0f; s.soc_scale = doc["socScale"] | 0.0f;
  s.includes_ev = doc["includes"] | false; s.meter_confirmed = doc["meter"] | false; s.third_mppt = doc["pv3"] | false;
  s.config_ready = s.config_limit >= 6 && *s.deye_host && *s.deye_serial &&
    doc["regGrid"].is<int>() && doc["regLoad"].is<int>() && doc["regSoc"].is<int>() &&
    doc["regPv1"].is<int>() && doc["regPv2"].is<int>() && doc["regPv3"].is<int>() && doc["regBat"].is<int>() &&
    s.pv1_scale > 0 && s.pv2_scale > 0 && s.pv3_scale > 0 && s.load_scale > 0 && s.grid_scale > 0 && s.bat_scale > 0 && s.soc_scale > 0 &&
    vt_soc_guard_valid(s.soc_stop, s.soc_resume);
  s.measured = doc["amps"].is<float>() && doc["state"].is<int>() && doc["wbValid"].is<bool>() &&
    vt_measurement_valid(doc["wbValid"], s.amps, s.state);
  strlcpy(s.message, doc["message"] | "", sizeof(s.message));
  return true;
}
static void vt_form_value(String &body, const char *name, const String &value) {
  if (body.length()) body += '&';
  body += name;
  body += '=';
  body += value;
}
static bool vt_save_soc_guard(const VtSnapshot &s, const String &token, bool enabled, int stop, int resume, String &response) {
  if (!s.soc_control_api || !vt_soc_guard_valid(stop, resume)) return false;
  String body;
  body.reserve(48);
  vt_form_value(body, "socGuard", enabled ? "1" : "0");
  vt_form_value(body, "socStop", String(stop)); vt_form_value(body, "socResume", String(resume));
  WiFiClient client;
  HTTPClient http;
  vt_http_begin(http, client, "/api/soc-guard");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("X-CSRF-Token", token);
  const int code = http.POST(body);
  response = code > 0 ? http.getString() : "";
  http.end();
  return code == 200;
}
static void vt_publish(VtSnapshot &s, bool completed) {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  // A command can be queued while a periodic GET is in progress.
  s.busy = completed ? false : vt_data.busy;
  if (!completed) strlcpy(s.result, vt_data.result, sizeof(s.result));
  vt_data = s;
  xSemaphoreGive(vt_mutex);
}
static void vt_task(void *) {
  uint32_t lastPoll = millis() - 3000;
  VtSnapshot s;
  for (;;) {
    VtCommand c;
    if (xQueueReceive(vt_queue, &c, pdMS_TO_TICKS(100)) == pdTRUE) {
      String token;
      bool ready = vt_is_enabled() && uint32_t(millis() - c.at) < 15000 && vt_read_status(s, token);
      ready = ready && vt_is_enabled() && uint32_t(millis() - c.at) < 15000;
      if (!ready) strlcpy(s.result, "Commande annulee : liaison indisponible ou attente expiree", sizeof(s.result));
      else if (c.type == VT_COMMAND_SOC_GUARD && (!s.soc_control_api || !vt_soc_guard_valid(c.soc_stop, c.soc_resume)))
        strlcpy(s.result, "Protection SOC indisponible : mettre a jour le firmware de la passerelle.", sizeof(s.result));
      else if (c.type == VT_COMMAND_MODE && c.mode == VT_MANUAL && !vt_manual_allowed(c.amps, s.limit))
        strlcpy(s.result, "Intensite refusee : verifier les limites de la WB01", sizeof(s.result));
      else if (c.type == VT_COMMAND_SOC_GUARD) {
        String response;
        const bool saved = vt_save_soc_guard(s, token, c.soc_guard, c.soc_stop, c.soc_resume, response);
        const bool readback = vt_read_status(s, token);
        if (saved && readback && s.soc_guard == c.soc_guard && s.soc_stop == c.soc_stop && s.soc_resume == c.soc_resume)
          strlcpy(s.result, "Protection SOC enregistree sur la passerelle.", sizeof(s.result));
        else if (!saved && response.length())
          snprintf(s.result, sizeof(s.result), "Protection SOC refusee : %s", response.c_str());
        else strlcpy(s.result, "Resultat SOC incertain : verifier les seuils sur la passerelle.", sizeof(s.result));
      }
      else {
        WiFiClient client;
        HTTPClient http;
        vt_http_begin(http, client, "/api/mode");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("X-CSRF-Token", token);
        int code = http.POST(String("mode=") + vt_mode_key(c.mode) + "&amps=" + String(c.amps));
        String response = code > 0 ? http.getString() : "";
        http.end();
        // Never retry a POST: a lost reply can follow an applied command.
        bool readback = vt_read_status(s, token);
        if (code == 200 && readback && s.mode == c.mode && (c.mode != VT_MANUAL || s.target == c.amps))
          strlcpy(s.result, "Mode relu sur la passerelle. Voir mesure et consigne WB01.", sizeof(s.result));
        else if (code > 0 && code != 200)
          snprintf(s.result, sizeof(s.result), "Refus HTTP %d : %s", code, response.c_str());
        else strlcpy(s.result, "Resultat incertain : verifier l'etat. Aucun renvoi automatique.", sizeof(s.result));
      }
      vt_publish(s, true);
      lastPoll = millis();
    } else if (uint32_t(millis() - lastPoll) >= 3000) {
      if (vt_is_enabled()) {
        String token;
        vt_read_status(s, token);
      } else { s.online = false; s.measured = false; }
      vt_publish(s, false);
      lastPoll = millis();
    }
  }
}
static void vt_begin() {
  vt_mutex = xSemaphoreCreateMutex();
  vt_queue = xQueueCreate(1, sizeof(VtCommand));
  if (!vt_mutex || !vt_queue) return;
  vt_sync_enabled();
  if (xTaskCreate(vt_task, "VETronic", 8192, nullptr, 1, nullptr) != pdPASS) {
    vQueueDelete(vt_queue);
    vt_queue = nullptr;
    strlcpy(vt_data.message, "Creation tache VE TRONIC impossible", sizeof(vt_data.message));
  }
}
