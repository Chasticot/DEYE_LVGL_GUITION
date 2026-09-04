// ve_deye.h - Donnees et registres du chargeur VE pilote par le DEYE

#pragma once

#include <Arduino.h>

// Ces adresses sont propres a la fonction "Smart devices / EV charger" du
// firmware DEYE. La puissance maximale est exprimee par pas de 10 W.
static constexpr uint16_t DEYE_REG_EV_CHARGE_MODE = 259;
static constexpr uint16_t DEYE_REG_EV_MAX_CHARGE_POWER = 260;
static constexpr uint16_t DEYE_REG_EV_CHARGE_POWER = 709;
static constexpr uint16_t DEYE_EV_POWER_REGISTER_SCALE_W = 10;
static constexpr uint32_t DEYE_EV_SINGLE_PHASE_MAX_POWER_W = 7400;
static constexpr uint32_t DEYE_EV_THREE_PHASE_MAX_POWER_W = 22000;

struct EvDeyeData {
  bool valid;
  uint16_t charge_power_w;        // Registre 709, en W.
  uint16_t max_charge_power_raw;  // Registre 260, en pas de 10 W.
  uint16_t connection_state_raw;  // Registre 259, masque/mode brut.
};

static inline uint32_t deye_ev_max_power_w(uint16_t raw_value) {
  return static_cast<uint32_t>(raw_value) * DEYE_EV_POWER_REGISTER_SCALE_W;
}
