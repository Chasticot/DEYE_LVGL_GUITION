#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings.h"
#include "vetronic_protocol.h"
#include "vetronic_tariff.h"
#include "v2_runtime.h"

#include "vetronic_data.h"

enum VtCommandType : uint8_t { VT_COMMAND_MODE, VT_COMMAND_SOC_GUARD };
struct VtCommand {
  VtCommandType type;
  VtMode mode;
  int amps, soc_stop, soc_resume;
  bool soc_guard;
  uint32_t at;
  VtTariffAction automatic = VT_TARIFF_NONE;
};
static VtSnapshot vt_data;
static SemaphoreHandle_t vt_mutex = nullptr;
static QueueHandle_t vt_queue = nullptr;
static bool vt_enabled = false;
static bool vt_tariffs_enabled = false, vt_manual_allowed_now = true;
static VtTariffState vt_tariffs;
static uint32_t vt_host_generation = 0;

static VtSnapshot vt_snapshot() {
  VtSnapshot s;
  if (!vt_mutex) return s;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  s = vt_data;
  s.tariff_pause = vt_tariffs.paused;
  s.manual_allowed = vt_manual_allowed_now;
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
  if (vt_data.busy) { xSemaphoreGive(vt_mutex); return false; }
  const bool saved = settings_save_vetronic_host(String(host));
  if (saved) {
    ++vt_host_generation;
    vt_tariffs.cancel();
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
  const bool allowed = v2_tariff().grid_allowed;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  vt_enabled = cfg_ev_charger_enabled;
  vt_tariffs_enabled = cfg_v2.ev_tariff_enabled;
  vt_manual_allowed_now = allowed;
  if (!vt_enabled || !vt_tariffs_enabled) vt_tariffs.cancel();
  xSemaphoreGive(vt_mutex);
}
static bool vt_submit(VtMode mode, int amps) {
  if (!vt_mutex || !vt_queue || !cfg_ev_charger_enabled || WiFi.status() != WL_CONNECTED || mode >= VT_UNKNOWN) return false;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  bool ready = !vt_data.busy;
  if (mode == VT_MANUAL) ready = ready && vt_manual_allowed_now && vt_data.online && vt_fresh(millis(), vt_data.at) && vt_manual_allowed(amps, vt_data.limit);
  if (ready) {
    VtCommand c = { VT_COMMAND_MODE, mode, amps, 0, 0, false, millis() };
    vt_data.busy = true;
    strlcpy(vt_data.result, "Commande en attente...", sizeof(vt_data.result));
    ready = xQueueSend(vt_queue, &c, 0) == pdTRUE;
    if (!ready) vt_data.busy = false;
    else vt_tariffs.cancel(); // Includes returning control to the autonomous WB01.
  }
  xSemaphoreGive(vt_mutex);
  return ready;
}
static bool vt_submit_soc_guard(bool enabled, int stop, int resume) {
  if (!vt_mutex || !vt_queue || !cfg_ev_charger_enabled || WiFi.status() != WL_CONNECTED || !vt_soc_guard_valid(stop, resume)) return false;
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  bool ready = !vt_data.busy && vt_data.online && vt_fresh(millis(), vt_data.at) && vt_data.soc_control_api;
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
  for (const char *key : {"token", "mode", "message", "wbValid", "amps", "state", "targetA", "limit", "nativeLimitA", "manualLimitA", "currentConfirmed",
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
  s.limit = vt_gateway_manual_limit(doc["limit"].as<int>(), doc["manualLimitA"] | 0, doc.containsKey("manualLimitA"));
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
static uint32_t vt_generation() {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  const uint32_t generation = vt_host_generation;
  xSemaphoreGive(vt_mutex);
  return generation;
}
static bool vt_publish(VtSnapshot &s, bool completed, uint32_t generation) {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  if (generation != vt_host_generation) { xSemaphoreGive(vt_mutex); return false; }
  // A command can be queued while a periodic GET is in progress.
  s.busy = completed ? false : vt_data.busy;
  if (!completed) strlcpy(s.result, vt_data.result, sizeof(s.result));
  vt_data = s;
  xSemaphoreGive(vt_mutex);
  return true;
}
static bool vt_command_policy(const VtCommand &c, const VtSnapshot &s) {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  const bool manual_ok = c.mode != VT_MANUAL || vt_manual_allowed_now;
  const bool auto_ok = c.automatic == VT_TARIFF_NONE || (vt_tariffs_enabled &&
    vt_tariffs.expected(c.automatic, s.mode, s.target) &&
    (c.automatic == VT_TARIFF_PAUSE ? !vt_manual_allowed_now : vt_manual_allowed_now));
  xSemaphoreGive(vt_mutex);
  return manual_ok && auto_ok;
}
static void vt_enforce_tariff(const VtSnapshot &s, uint32_t generation) {
  xSemaphoreTake(vt_mutex, portMAX_DELAY);
  if (generation == vt_host_generation && vt_enabled && !vt_data.busy) {
    const VtTariffAction action = vt_tariffs.next(vt_tariffs_enabled, vt_manual_allowed_now,
      s.online && vt_fresh(millis(), s.at), s.mode, s.target);
    if (action != VT_TARIFF_NONE) {
      VtCommand c = {VT_COMMAND_MODE, action == VT_TARIFF_PAUSE ? VT_STOP : VT_MANUAL,
        vt_tariffs.amps, 0, 0, false, millis(), action};
      if (xQueueSend(vt_queue, &c, 0) == pdTRUE) {
        vt_data.busy = true;
        strlcpy(vt_data.result, action == VT_TARIFF_PAUSE ? "Pause tarifaire en attente..." :
          "Reprise manuelle en attente...", sizeof(vt_data.result));
      }
    }
  }
  xSemaphoreGive(vt_mutex);
}
static void vt_task(void *) {
  uint32_t lastPoll = millis() - 3000;
  VtSnapshot s;
  for (;;) {
    VtCommand c;
    if (xQueueReceive(vt_queue, &c, pdMS_TO_TICKS(100)) == pdTRUE) {
      const uint32_t generation = vt_generation();
      bool mode_confirmed = false;
      String token;
      bool ready = vt_is_enabled() && uint32_t(millis() - c.at) < 15000 && vt_read_status(s, token);
      ready = ready && vt_is_enabled() && uint32_t(millis() - c.at) < 15000;
      if (!ready) strlcpy(s.result, "Commande annulee : liaison indisponible ou attente expiree", sizeof(s.result));
      else if (c.type == VT_COMMAND_MODE && !vt_command_policy(c, s))
        strlcpy(s.result, "Commande annulee : tarifs ou mode de la borne modifies.", sizeof(s.result));
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
        mode_confirmed = code == 200 && readback && s.mode == c.mode && (c.mode != VT_MANUAL || s.target == c.amps);
        if (mode_confirmed)
          strlcpy(s.result, c.mode == VT_LEGACY ? "Main rendue a la borne. Vous pouvez reprendre le pilotage ici." :
            "Mode relu sur la passerelle. Voir mesure et consigne WB01.", sizeof(s.result));
        else if (code > 0 && code != 200)
          snprintf(s.result, sizeof(s.result), "Refus HTTP %d : %s", code, response.c_str());
        else strlcpy(s.result, "Resultat incertain : verifier l'etat. Aucun renvoi automatique.", sizeof(s.result));
      }
      xSemaphoreTake(vt_mutex, portMAX_DELAY);
      if (c.type == VT_COMMAND_MODE) {
        if (c.automatic != VT_TARIFF_NONE) vt_tariffs.completed(c.automatic, mode_confirmed);
        else if (mode_confirmed && c.mode == VT_MANUAL && vt_enabled && vt_tariffs_enabled) vt_tariffs.manual_confirmed(c.amps);
      }
      xSemaphoreGive(vt_mutex);
      vt_publish(s, true, generation);
      lastPoll = millis();
    } else if (uint32_t(millis() - lastPoll) >= 3000) {
      const uint32_t generation = vt_generation();
      if (vt_is_enabled()) {
        String token;
        vt_read_status(s, token);
      } else { s.online = false; s.measured = false; }
      if (vt_publish(s, false, generation)) vt_enforce_tariff(s, generation);
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
