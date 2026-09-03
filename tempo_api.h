#pragma once

// Lecture asynchrone du tarif Tempo applicable maintenant. La tâche réseau ne
// manipule jamais LVGL : l'interface lit seulement une copie de cet état.

struct TempoNow {
  bool valid;
  uint8_t color_code;   // 1 = bleu, 2 = blanc, 3 = rouge
  uint8_t hour_code;    // 0 = HC, 1 = HP
  uint8_t tomorrow_color_code;  // 0 = non communiqué, 1 = bleu, 2 = blanc, 3 = rouge
};

#ifdef ARDUINO

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

static constexpr uint32_t TEMPO_REFRESH_INTERVAL_MS = 5UL * 60UL * 1000UL;
static const char *const TEMPO_NOW_URL = "https://www.api-couleur-tempo.fr/api/now";

static TempoNow tempo_now = {false, 0, 0, 0};
static portMUX_TYPE tempo_now_mutex = portMUX_INITIALIZER_UNLOCKED;
static volatile bool tempo_fetch_running = false;
static uint32_t tempo_last_request_ms = 0;

static int tempo_json_int(const String &json, const char *key) {
  const String needle = String('"') + key + '"';
  const int key_pos = json.indexOf(needle);
  if (key_pos < 0) return -1;

  const int colon_pos = json.indexOf(':', key_pos + needle.length());
  if (colon_pos < 0) return -1;
  return json.substring(colon_pos + 1).toInt();
}

static String tempo_http_get(const char *url) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setConnectTimeout(4000);
  http.setTimeout(5000);

  String payload;
  if (http.begin(client, url)) {
    if (http.GET() == HTTP_CODE_OK) payload = http.getString();
    http.end();
  }
  return payload;
}

static void tempo_fetch_task(void *parameter) {
  (void)parameter;
  TempoNow next = {false, 0, 0, 0};

  if (WiFi.status() == WL_CONNECTED) {
    const String now_payload = tempo_http_get(TEMPO_NOW_URL);
    const int color_code = tempo_json_int(now_payload, "codeCouleur");
    const int hour_code = tempo_json_int(now_payload, "codeHoraire");
    if (color_code >= 1 && color_code <= 3 && (hour_code == 0 || hour_code == 1)) {
      next.valid = true;
      next.color_code = (uint8_t)color_code;
      next.hour_code = (uint8_t)hour_code;
    }

    const String tomorrow_payload = tempo_http_get(
      "https://www.api-couleur-tempo.fr/api/jourTempo/tomorrow"
    );
    const int tomorrow_color = tempo_json_int(tomorrow_payload, "codeJour");
    if (tomorrow_color >= 1 && tomorrow_color <= 3) {
      next.tomorrow_color_code = (uint8_t)tomorrow_color;
    }
  }

  portENTER_CRITICAL(&tempo_now_mutex);
  tempo_now = next;
  tempo_fetch_running = false;
  portEXIT_CRITICAL(&tempo_now_mutex);
  vTaskDelete(nullptr);
}

static void tempo_api_process(bool enabled) {
  if (!enabled) {
    portENTER_CRITICAL(&tempo_now_mutex);
    tempo_now.valid = false;
    // Une réactivation déclenche donc immédiatement une lecture fraîche.
    tempo_last_request_ms = 0;
    portEXIT_CRITICAL(&tempo_now_mutex);
    return;
  }
  if (WiFi.status() != WL_CONNECTED) return;

  const uint32_t now = millis();
  bool start_fetch = false;
  portENTER_CRITICAL(&tempo_now_mutex);
  if (!tempo_fetch_running &&
      (tempo_last_request_ms == 0 || now - tempo_last_request_ms >= TEMPO_REFRESH_INTERVAL_MS)) {
    tempo_fetch_running = true;
    tempo_last_request_ms = now;
    start_fetch = true;
  }
  portEXIT_CRITICAL(&tempo_now_mutex);

  if (start_fetch && xTaskCreatePinnedToCore(
      tempo_fetch_task, "TempoFetch", 6144, nullptr, 1, nullptr, 0
    ) != pdPASS) {
    portENTER_CRITICAL(&tempo_now_mutex);
    tempo_fetch_running = false;
    portEXIT_CRITICAL(&tempo_now_mutex);
  }
}

static TempoNow tempo_api_get_now() {
  TempoNow copy;
  portENTER_CRITICAL(&tempo_now_mutex);
  copy = tempo_now;
  portEXIT_CRITICAL(&tempo_now_mutex);
  return copy;
}

#else

// Aperçu cohérent dans le simulateur PC, sans accès réseau.
static void tempo_api_process(bool enabled) {
  (void)enabled;
}

static TempoNow tempo_api_get_now() {
  // Fixture visuelle optionnelle pour le simulateur : valeur transmise au
  // lancement, sans effet sur le firmware ESP32 ni sur le comportement usuel.
  const char *fixture = std::getenv("DEYE_SIM_TEMPO_FIXTURE");
  if (fixture != nullptr && std::strcmp(fixture, "blue") == 0) {
    return {true, 1, 1, 1};
  }
  return {true, 2, 1, 0};
}

#endif
