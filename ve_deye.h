#pragma once
#include <Arduino.h>
#include "ve_modbus_codec.h"

// Cartographie relevee sur SUN-xxK-SG02LP1 avec une SUN-EVSE22K01-EU en LoRa.
// R489: les bits 1:0 sont le mode (1 solaire, 2 libre). R490 est en watts.
// La partie haute de R489 et tous ses autres bits sont reserves et preserves.
static constexpr uint16_t DEYE_REG_EV_CHARGE_MODE = 489;
static constexpr uint16_t DEYE_REG_EV_MAX_CHARGE_POWER = 490;
// Unite de R490. Ne pas deduire l'unite de la valeur lue.
static constexpr uint16_t DEYE_EV_POWER_REGISTER_SCALE_W = 1;
static constexpr uint32_t DEYE_EV_SINGLE_PHASE_MAX_POWER_W = 7400;
static constexpr uint32_t DEYE_EV_THREE_PHASE_MAX_POWER_W = 22000;
// Plafond independant de R490, pour l'installation monophasee actuelle.
static constexpr uint32_t DEYE_EV_INSTALLATION_MAX_POWER_W = 7400;
static constexpr uint16_t DEYE_EV_MODE_ACTIVE_PREFIX = 0x5F00;
static constexpr uint16_t DEYE_EV_MODE_DISABLED_RAW = 0x5A00;
// Valide par comparaison LCD/DeyeCloud: R490 3050 -> 4000 W et R489
// 0x5F01 <-> 0x5F02 lors du basculement solaire/libre.
static constexpr bool DEYE_EV_INVERTER_PROFILE_VERIFIED = true;
static constexpr uint16_t DEYE_EV_BLOCK3_START = DEYE_REG_EV_CHARGE_MODE;
static constexpr uint16_t DEYE_EV_BLOCK3_COUNT = 2;
static constexpr uint32_t DEYE_EV_FRESH_MS = 45000;
static constexpr uint32_t DEYE_EV_COMMAND_MAX_AGE_MS = 15000;
static_assert(DEYE_EV_BLOCK3_COUNT > 0 && DEYE_EV_BLOCK3_COUNT <= 125, "Taille bloc3 invalide");
static_assert(DEYE_EV_BLOCK3_START <= DEYE_REG_EV_CHARGE_MODE &&
  uint32_t(DEYE_EV_BLOCK3_START) + DEYE_EV_BLOCK3_COUNT > DEYE_REG_EV_MAX_CHARGE_POWER &&
  uint32_t(DEYE_EV_BLOCK3_START) + DEYE_EV_BLOCK3_COUNT <= 65536, "Bloc3 doit contenir R489/R490");

enum EvDeyeCommandState : uint8_t {
  EV_COMMAND_IDLE, EV_COMMAND_QUEUED, EV_COMMAND_RUNNING, EV_COMMAND_CONFIRMED,
  EV_COMMAND_FAILED, EV_COMMAND_PARTIAL, EV_COMMAND_CANCELLED
};
struct EvDeyeData {
  bool valid;                    // R489/R490 recents.
  uint16_t max_charge_power_raw;
  uint16_t mode_raw;
  uint32_t settings_updated_ms;
  EvDeyeCommandState command_state;
  uint8_t modbus_exception;
  char command_message[96];
};
struct EvDeyeCommand {
  bool set_power;
  bool set_mode;
  uint16_t power_raw;
  uint8_t mode;                  // 0 = desactive, 1 = solaire, 2 = libre.
  uint32_t queued_ms;
};
static inline uint32_t deye_ev_max_power_w(uint16_t raw) {
  return uint32_t(raw) * DEYE_EV_POWER_REGISTER_SCALE_W;
}
static inline bool deye_ev_mode_write_supported(uint16_t raw) {
  const uint8_t mode = raw & 3;
  return (raw & 0xFF00) == DEYE_EV_MODE_ACTIVE_PREFIX && (mode == 1 || mode == 2);
}
static inline bool deye_ev_mode_transition_supported(uint16_t raw) {
  return deye_ev_mode_write_supported(raw) || raw == DEYE_EV_MODE_DISABLED_RAW;
}
static inline bool deye_ev_mode_target(uint16_t current, uint8_t requested, uint16_t *target) {
  if (target == nullptr || requested > 2 || !deye_ev_mode_transition_supported(current)) return false;
  if (requested == 0) {
    *target = DEYE_EV_MODE_DISABLED_RAW;
    return true;
  }
  *target = deye_ev_mode_write_supported(current)
    ? deye_ev_replace_mode(current, requested)
    : uint16_t(DEYE_EV_MODE_ACTIVE_PREFIX | requested);
  return true;
}
static inline const char *deye_ev_mode_name(uint16_t raw) {
  if (raw == DEYE_EV_MODE_DISABLED_RAW) return "Desactive";
  if (!deye_ev_mode_write_supported(raw)) return "Non reconnu";
  switch (raw & 3) {
    case 1: return "Solaire uniquement";
    case 2: return "Libre";
    default: return "Non reconnu"; // Inatteignable, garde le compilateur satisfait.
  }
}
static inline bool deye_ev_command_busy(EvDeyeCommandState state) {
  return state == EV_COMMAND_QUEUED || state == EV_COMMAND_RUNNING;
}
