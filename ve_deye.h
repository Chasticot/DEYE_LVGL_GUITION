#pragma once
#include <Arduino.h>
#include "ve_modbus_codec.h"

// Profil Smart Devices V105.4 : a comparer au LCD du SUN-12KSG02LP1-EU-AM3.
// Source et protocole de verification dans VE_INTEGRATION.md.
static constexpr uint16_t DEYE_REG_EV_CHARGE_MODE = 259;
static constexpr uint16_t DEYE_REG_EV_MAX_CHARGE_POWER = 260;
static constexpr uint16_t DEYE_REG_EV_REQUESTED_POWER = 709;
// Unite de R260 dans ce profil. Ne pas deduire l'unite de la valeur lue.
static constexpr uint16_t DEYE_EV_POWER_REGISTER_SCALE_W = 1;
static constexpr uint32_t DEYE_EV_SINGLE_PHASE_MAX_POWER_W = 7400;
static constexpr uint32_t DEYE_EV_THREE_PHASE_MAX_POWER_W = 22000;
// Plafond independant de R260, pour l'installation monophasee actuelle.
static constexpr uint32_t DEYE_EV_INSTALLATION_MAX_POWER_W = 7400;
// Elargir ici si le firmware refuse la lecture d'un seul registre.
static constexpr uint16_t DEYE_EV_BLOCK3_START = 709;
static constexpr uint16_t DEYE_EV_BLOCK3_COUNT = 1;
static constexpr uint32_t DEYE_EV_FRESH_MS = 45000;
static constexpr uint32_t DEYE_EV_COMMAND_MAX_AGE_MS = 15000;
static_assert(DEYE_EV_BLOCK3_COUNT > 0 && DEYE_EV_BLOCK3_COUNT <= 125, "Taille bloc3 invalide");
static_assert(DEYE_EV_BLOCK3_START <= 709 && uint32_t(DEYE_EV_BLOCK3_START) + DEYE_EV_BLOCK3_COUNT > 709 &&
  uint32_t(DEYE_EV_BLOCK3_START) + DEYE_EV_BLOCK3_COUNT <= 65536, "Bloc3 doit contenir R709");

enum EvDeyeCommandState : uint8_t {
  EV_COMMAND_IDLE, EV_COMMAND_QUEUED, EV_COMMAND_RUNNING, EV_COMMAND_CONFIRMED,
  EV_COMMAND_FAILED, EV_COMMAND_PARTIAL, EV_COMMAND_CANCELLED
};
struct EvDeyeData {
  bool valid;                    // R259/260 recents, independamment de R709.
  bool requested_power_valid;
  uint16_t requested_power_w;    // R709 : consigne envoyee, PAS puissance mesuree.
  uint16_t max_charge_power_raw;
  uint16_t mode_raw;
  uint32_t settings_updated_ms;
  uint32_t requested_updated_ms;
  EvDeyeCommandState command_state;
  uint8_t modbus_exception;
  char command_message[96];
};
struct EvDeyeCommand {
  bool set_power;
  bool set_mode;
  uint16_t power_raw;
  uint8_t mode;                  // 1 = solaire, 2 = libre ; seulement bits 1:0.
  uint32_t queued_ms;
};
static inline uint32_t deye_ev_max_power_w(uint16_t raw) {
  return uint32_t(raw) * DEYE_EV_POWER_REGISTER_SCALE_W;
}
static inline const char *deye_ev_mode_name(uint16_t raw) {
  switch (raw & 3) {
    case 1: return "Solaire uniquement";
    case 2: return "Libre";
    default: return "Non reconnu";
  }
}
static inline bool deye_ev_command_busy(EvDeyeCommandState state) {
  return state == EV_COMMAND_QUEUED || state == EV_COMMAND_RUNNING;
}
