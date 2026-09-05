#pragma once

#include <Arduino.h>

// Le rendu historique reste le choix de démarrage, afin de ne pas modifier
// l'apparence des installations existantes. Les deux autres identifiants sont
// réservés à une évolution ultérieure de la palette.
enum UiThemeId : uint8_t {
  UI_THEME_DARK = 0,
  UI_THEME_LIGHT
};

static constexpr UiThemeId UI_THEME_DEFAULT = UI_THEME_DARK;

// Une palette est volontairement réduite à des rôles visuels. Cela évite les
// images, les allocations dynamiques et tout travail supplémentaire dans la
// boucle principale de l'ESP32.
struct UiThemePalette {
  uint32_t dashboard_bg;
  uint32_t screen_bg;
  uint32_t card_bg;
  uint32_t border;
  uint32_t text;
  uint32_t muted_text;
  uint32_t detail_text;
  uint32_t accent;
  uint32_t accent_text;
  uint32_t accent_dark;
  uint32_t control_bg;
  uint32_t control_border;
  uint32_t switch_off;
  uint32_t switch_on;
  uint32_t highlight;
  uint32_t arc_bg;
};

static constexpr UiThemePalette UI_THEME_PALETTES[] = {
  // Sombre : thème par défaut, neutre et contrasté.
  {0x020617, 0x020617, 0x0F172A, 0x334155, 0xF8FAFC, 0x94A3B8,
   0xCBD5E1, 0x38BDF8, 0x020617, 0x0369A1, 0x0F172A, 0x334155,
   0x475569, 0x0284C7, 0x22D3EE, 0x233044},
  // Clair : lisible dans un environnement lumineux.
  {0xF1F5F9, 0xF8FAFC, 0xFFFFFF, 0x000000, 0x0F172A, 0x475569,
   0x334155, 0x0284C7, 0xFFFFFF, 0x0369A1, 0xFFFFFF, 0x94A3B8,
   0xCBD5E1, 0x0284C7, 0x0E7490, 0xCBD5E1}
};

static UiThemeId ui_theme_from_value(uint8_t value) {
  return value <= UI_THEME_LIGHT
    ? static_cast<UiThemeId>(value)
    : UI_THEME_DEFAULT;
}

static const UiThemePalette &ui_theme_palette(UiThemeId theme) {
  return UI_THEME_PALETTES[static_cast<uint8_t>(
    ui_theme_from_value(static_cast<uint8_t>(theme))
  )];
}
