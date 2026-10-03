#pragma once
#include "vetronic_protocol.h"

struct VtSnapshot {
  bool tariff_pause = false, manual_allowed = true;
  bool online = false, measured = false, busy = false, confirmed = false;
  bool config_ready = false, soc_control_api = false, soc_guard = false, soc_blocked = false;
  uint32_t at = 0;
  VtMode mode = VT_UNKNOWN;
  float amps = 0;
  int target = -1, limit = 0, state = -1, soc = -1, soc_stop = 30, soc_resume = 35;
  int config_limit = 0;
  uint16_t reg_grid = 0, reg_load = 0, reg_soc = 0, reg_pv1 = 0, reg_pv2 = 0, reg_pv3 = 0, reg_bat = 0;
  float pv1_scale = 1, pv2_scale = 1, pv3_scale = 1, load_scale = 1, grid_scale = 1, bat_scale = 1, soc_scale = 1;
  bool includes_ev = false, meter_confirmed = false, third_mppt = false;
  char deye_host[16] = "", deye_serial[12] = "";
  char message[240] = "En attente de la passerelle VE TRONIC";
  char result[240] = "";
};
