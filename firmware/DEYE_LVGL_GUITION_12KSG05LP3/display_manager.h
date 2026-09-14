#pragma once

#include <Arduino.h>
#include <time.h>
#include <math.h>
#include "config.h"
#include "settings.h"

static constexpr uint8_t DISPLAY_PWM_CHANNEL = 7;
// Frequence validee sur ce firmware pour le retroeclairage GPIO38.
static constexpr uint32_t DISPLAY_PWM_FREQUENCY = 5000;
// Sous ce seuil, le driver du panneau GUITION peut couper visuellement le
// retroeclairage. Il protege aussi une ancienne preference trop faible.
static constexpr uint8_t DISPLAY_BACKLIGHT_MIN_DUTY = DISPLAY_BRIGHTNESS_MIN;
// -1 est un etat invalide : la premiere ecriture PWM doit toujours avoir
// lieu, y compris lorsque la luminosite demandee est deja 255.
static int16_t display_applied_brightness = -1;
static constexpr float DISPLAY_PI = 3.14159265358979323846f;
static constexpr int DISPLAY_SOLAR_TRANSITION_MINUTES = 60;

static uint8_t display_interpolate(uint8_t start, uint8_t end, float amount) {
  amount = constrain(amount, 0.0f, 1.0f);
  return uint8_t(lroundf(float(start) + (float(end) - start) * amount));
}

// Approximation NOAA : suffisamment precise pour piloter le confort visuel,
// sans dependance reseau autre que l'heure NTP deja utilisee par le projet.
static bool display_sunrise_sunset_minutes(const struct tm &local, int &sunrise, int &sunset) {
  const float gamma = 2.0f * DISPLAY_PI / 365.0f * (float(local.tm_yday) + (float(local.tm_hour) - 12.0f) / 24.0f);
  const float equation = 229.18f * (0.000075f + 0.001868f * cosf(gamma) - 0.032077f * sinf(gamma)
    - 0.014615f * cosf(2.0f * gamma) - 0.040849f * sinf(2.0f * gamma));
  const float declination = 0.006918f - 0.399912f * cosf(gamma) + 0.070257f * sinf(gamma)
    - 0.006758f * cosf(2.0f * gamma) + 0.000907f * sinf(2.0f * gamma)
    - 0.002697f * cosf(3.0f * gamma) + 0.00148f * sinf(3.0f * gamma);
  const float latitude = cfg_display.latitude * DISPLAY_PI / 180.0f;
  const float cos_hour_angle = (cosf(90.833f * DISPLAY_PI / 180.0f) / (cosf(latitude) * cosf(declination)))
    - tanf(latitude) * tanf(declination);
  if (cos_hour_angle < -1.0f || cos_hour_angle > 1.0f) return false;
  const float hour_angle = acosf(cos_hour_angle) * 180.0f / DISPLAY_PI;
  time_t timestamp = time(nullptr);
  struct tm utc = {};
  gmtime_r(&timestamp, &utc);
  const int timezone_minutes = int(difftime(timestamp, mktime(&utc)) / 60);
  const float noon = 720.0f - 4.0f * cfg_display.longitude - equation + timezone_minutes;
  sunrise = int(lroundf(noon - 4.0f * hour_angle));
  sunset = int(lroundf(noon + 4.0f * hour_angle));
  return sunrise >= 0 && sunset <= 1440 && sunrise < sunset;
}

static uint8_t display_effective_brightness() {
  if (!cfg_display.night_enabled) return cfg_display.day_brightness;
  struct tm now = {};
  if (!getLocalTime(&now, 0)) return cfg_display.day_brightness;
  if (cfg_display.sunset_mode) {
    int sunrise = 0, sunset = 0;
    if (display_sunrise_sunset_minutes(now, sunrise, sunset)) {
      const int minute = now.tm_hour * 60 + now.tm_min;
      const int half = DISPLAY_SOLAR_TRANSITION_MINUTES / 2;
      if (minute >= sunrise - half && minute <= sunrise + half) {
        return display_interpolate(cfg_display.night_brightness, cfg_display.day_brightness,
          float(minute - (sunrise - half)) / DISPLAY_SOLAR_TRANSITION_MINUTES);
      }
      if (minute >= sunset - half && minute <= sunset + half) {
        return display_interpolate(cfg_display.day_brightness, cfg_display.night_brightness,
          float(minute - (sunset - half)) / DISPLAY_SOLAR_TRANSITION_MINUTES);
      }
      return minute > sunrise + half && minute < sunset - half ? cfg_display.day_brightness : cfg_display.night_brightness;
    }
  }
  const uint8_t hour = uint8_t(now.tm_hour);
  const uint8_t start = cfg_display.night_start_hour;
  const uint8_t end = cfg_display.night_end_hour;
  const bool night = start == end || (start < end ? hour >= start && hour < end : hour >= start || hour < end);
  return night ? cfg_display.night_brightness : cfg_display.day_brightness;
}

static void display_manager_apply() {
  const uint8_t brightness = display_effective_brightness();
  if (brightness == display_applied_brightness) return;
  display_applied_brightness = brightness;
  ledcWrite(DISPLAY_PWM_CHANNEL, max(brightness, DISPLAY_BACKLIGHT_MIN_DUTY));
}

static void display_manager_begin() {
  ledcSetup(DISPLAY_PWM_CHANNEL, DISPLAY_PWM_FREQUENCY, 8);
  ledcAttachPin(GFX_BL, DISPLAY_PWM_CHANNEL);
  // Le pilote RGB peut reinitialiser la broche pendant son demarrage. Cette
  // fonction est appelee juste apres gfx->begin(), puis force le premier PWM.
  display_applied_brightness = -1;
  display_manager_apply();
}
