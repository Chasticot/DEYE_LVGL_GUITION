#pragma once

#include <Arduino.h>

// =============================================
// STRUCTURE POUR LES REGISTRES
// =============================================
struct InverterRegisters {
  // PV
  uint16_t pv1_power;
  uint16_t pv2_power;
  uint16_t pv3_power;
  uint16_t pv4_power;
  uint16_t pv_daily;
  
  // Batterie
  uint16_t battery_soc;
  uint16_t battery_voltage;
  uint16_t battery_power;
  uint16_t battery_temp;
  
  // Réseau
  uint16_t grid_power;
  uint16_t grid_status;
  uint16_t grid_buy_daily;
  uint16_t grid_sell_daily;
  
  // Consommation
  uint16_t load_power;
  uint16_t ups_power;
  uint16_t load_daily;
  
  // Températures
  uint16_t dc_temp;
  uint16_t ac_temp;
  
  // SmartLoad
  uint16_t smartload;
  
  // Plages de lecture
  uint16_t block1_start;
  uint16_t block1_count;
  uint16_t block2_start;
  uint16_t block2_count;
};

// =============================================
// MODÈLE : DEYE SUN-25K-SG01HP3-EU-AM2
// =============================================
// Cartographie triphase haute tension issue du protocole Deye V1.04.
// Ce modele a 2 MPPT et 4 entrees PV (2+2 strings).
const InverterRegisters deye_hybrid = {
  // PV
  .pv1_power = 672,
  .pv2_power = 673,
  .pv3_power = 674,
  .pv4_power = 675,
  .pv_daily = 529,
  
  // Batterie
  .battery_soc = 588,
  .battery_voltage = 587,
  .battery_power = 590,
  .battery_temp = 586,
  
  // Réseau
  .grid_power = 625,
  .grid_status = 552,
  .grid_buy_daily = 520,
  .grid_sell_daily = 521,
  
  // Consommation
  .load_power = 653,
  .ups_power = 643,
  .load_daily = 526,
  
  // Températures
  .dc_temp = 540,
  .ac_temp = 541,
  
  // SmartLoad
  .smartload = 552,
  
  // Plages de lecture
  .block1_start = 520,
  .block1_count = 22,   // 520 a 541
  .block2_start = 552,
  .block2_count = 124   // 552 a 675, limite FC03 respectee
};

// =============================================
// MODÈLE ACTIF
// =============================================
#define CURRENT_MODEL deye_hybrid

// =============================================
// ACCÈS AUX REGISTRES (macros)
// =============================================
#define REG_PV1_POWER      CURRENT_MODEL.pv1_power
#define REG_PV2_POWER      CURRENT_MODEL.pv2_power
#define REG_PV3_POWER      CURRENT_MODEL.pv3_power
#define REG_PV4_POWER      CURRENT_MODEL.pv4_power
#define REG_PV_DAILY       CURRENT_MODEL.pv_daily

#define REG_BATTERY_SOC    CURRENT_MODEL.battery_soc
#define REG_BATTERY_VOLTAGE CURRENT_MODEL.battery_voltage
#define REG_BATTERY_POWER  CURRENT_MODEL.battery_power
#define REG_BATTERY_TEMP   CURRENT_MODEL.battery_temp

#define REG_GRID_POWER     CURRENT_MODEL.grid_power
#define REG_GRID_STATUS    CURRENT_MODEL.grid_status
#define REG_GRID_BUY_DAY   CURRENT_MODEL.grid_buy_daily
#define REG_GRID_SELL_DAY  CURRENT_MODEL.grid_sell_daily

#define REG_LOAD_POWER     CURRENT_MODEL.load_power
#define REG_UPS_POWER      CURRENT_MODEL.ups_power
#define REG_LOAD_DAY       CURRENT_MODEL.load_daily

#define REG_DC_TEMP        CURRENT_MODEL.dc_temp
#define REG_AC_TEMP        CURRENT_MODEL.ac_temp

#define REG_SMARTLOAD      CURRENT_MODEL.smartload

// Plages de lecture
#define BLOCK1_START       CURRENT_MODEL.block1_start
#define BLOCK1_COUNT       CURRENT_MODEL.block1_count
#define BLOCK2_START       CURRENT_MODEL.block2_start
#define BLOCK2_COUNT       CURRENT_MODEL.block2_count
