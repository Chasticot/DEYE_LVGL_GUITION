#pragma once

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include "app_data.h"

// 24 h a raison d'un echantillon toutes les cinq minutes. Le tampon circulaire
// reste volontairement en RAM : aucun echantillon ne provoque d'ecriture flash.
static constexpr uint16_t HISTORY_CAPACITY = 288;
static constexpr uint32_t HISTORY_SAMPLE_MS = 5UL * 60UL * 1000UL;

struct HistoryPoint {
  uint32_t captured_ms;
  uint16_t pv_w;
  int16_t grid_w;
  int16_t load_w;
  int16_t battery_w;
  uint8_t battery_soc;
};

static HistoryPoint *history_points = nullptr;
static uint16_t history_first = 0;
static uint16_t history_count = 0;
static uint32_t history_last_sample_ms = 0;
static portMUX_TYPE history_lock = portMUX_INITIALIZER_UNLOCKED;

static void history_begin() {
  history_points = static_cast<HistoryPoint *>(heap_caps_calloc(
    HISTORY_CAPACITY, sizeof(HistoryPoint), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
  ));
  if (history_points == nullptr) {
    history_points = static_cast<HistoryPoint *>(heap_caps_calloc(
      HISTORY_CAPACITY, sizeof(HistoryPoint), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
    ));
  }
  if (history_points == nullptr) DBG.println("Historique indisponible : memoire insuffisante.");
}

static void history_add_snapshot(const DashboardData &data) {
  if (history_points == nullptr || !data.valid) return;
  const uint32_t now = millis();
  if (history_last_sample_ms && uint32_t(now - history_last_sample_ms) < HISTORY_SAMPLE_MS) return;
  HistoryPoint point = {
    now,
    uint16_t(data.pv1_w + data.pv2_w + data.pv3_w),
    data.grid_power, data.load_power, data.battery_power,
    uint8_t(constrain(data.battery_soc, 0, 100))
  };
  portENTER_CRITICAL(&history_lock);
  const uint16_t index = (history_first + history_count) % HISTORY_CAPACITY;
  history_points[index] = point;
  if (history_count < HISTORY_CAPACITY) ++history_count;
  else history_first = (history_first + 1) % HISTORY_CAPACITY;  // Ecrase le plus ancien.
  history_last_sample_ms = now;
  portEXIT_CRITICAL(&history_lock);
}

static uint16_t history_copy(HistoryPoint *out, uint16_t capacity) {
  if (out == nullptr || history_points == nullptr || capacity == 0) return 0;
  portENTER_CRITICAL(&history_lock);
  const uint16_t copied = min(capacity, history_count);
  const uint16_t skip = history_count - copied;
  for (uint16_t i = 0; i < copied; ++i) {
    out[i] = history_points[(history_first + skip + i) % HISTORY_CAPACITY];
  }
  portEXIT_CRITICAL(&history_lock);
  return copied;
}
