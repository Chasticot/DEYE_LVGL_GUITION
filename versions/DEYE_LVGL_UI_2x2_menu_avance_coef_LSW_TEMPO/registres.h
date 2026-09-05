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
// MODÈLE : DEYE HYBRID 3 MPPT
// =============================================
// ✅ Validé avec SUN-12K-SG02LP1-EU-AM2
// ✅ Compatible avec SUN-(5/6/8/10/12)K-SG(01/02/03/04)LP1
const InverterRegisters deye_hybrid = {
  // PV
  .pv1_power = 186,
  .pv2_power = 187,
  .pv3_power = 188,
  .pv_daily = 108,
  
  // Batterie
  .battery_soc = 184,
  .battery_voltage = 183,
  .battery_power = 190,
  .battery_temp = 182,
  
  // Réseau
  .grid_power = 169,
  .grid_status = 194,
  .grid_buy_daily = 76,
  .grid_sell_daily = 77,
  
  // Consommation
  .load_power = 178,
  .ups_power = 172,
  .load_daily = 84,
  
  // Températures
  .dc_temp = 90,
  .ac_temp = 91,
  
  // SmartLoad
  .smartload = 195,
  
  // Plages de lecture
  .block1_start = 76,
  .block1_count = 37,   // 76 à 112
  .block2_start = 169,
  .block2_count = 27    // 169 à 195
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