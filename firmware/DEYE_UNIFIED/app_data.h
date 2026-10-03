#pragma once

#include <Arduino.h>

struct DashboardData {
  bool valid;

  uint32_t pv1_w;
  uint32_t pv2_w;
  uint32_t pv3_w;

  uint16_t battery_soc;
  float battery_voltage;
  int32_t battery_power;
  float battery_temperature;

  int32_t grid_power;
  int32_t load_power;
  int32_t gen_power;
  float ups_voltage;

  float dc_temperature;
  float ac_temperature;

  bool smartload_on;
  uint32_t pv4_w;
};

// Initialisation correcte - tous les champs à 0
static DashboardData dashboard_data = {
  false,   // valid
  0,       // pv1_w
  0,       // pv2_w
  0,       // pv3_w
  0,       // battery_soc
  0.0f,    // battery_voltage
  0,       // battery_power
  0.0f,    // battery_temperature
  0,       // grid_power
  0,       // load_power
  0,       // gen_power
  0.0f,    // ups_voltage
  0.0f,    // dc_temperature
  0.0f,    // ac_temperature
  false    // smartload_on
};
