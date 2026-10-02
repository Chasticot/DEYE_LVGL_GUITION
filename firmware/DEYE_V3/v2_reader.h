#pragma once

// Called only by SolarmanReader, without the dashboard mutex held.
static void v2_read_auxiliary(bool core_sample_ok) {
  uint8_t rtu[7] = {};
  uint8_t exception = 0;
  const bool gen_ok = inverter_profile().gen_supported && REG_GEN_POWER != DEYE_NO_REGISTER && solarman_read_block(REG_GEN_POWER, 1, rtu, &exception);
  const int16_t raw = gen_ok ? int16_t(modbus_get_u16_be(rtu, 0)) : 0;
  const int32_t power = scaled_power(raw, COEFF_GEN_POWER, 1.0f);
  const uint32_t sampled_ms = millis();
  static V2EnergyIntegrator meter;
  static uint32_t last_save_ms = 0;
  struct tm local = {};
  const bool clock_ok = ntp_received.load() && getLocalTime(&local, 0);
  bool energy_ok = false;
  if (clock_ok && inverter_profile().gen_supported && !cfg_gen_smartload) {
    const int today = v2_day_key(local);
    if (meter.day != today) {
      meter.kwh = 0;
      meter.previous_valid = false;
      Preferences nvs;
      if (nvs.begin(inverter_profile().storage, true)) {
        struct Checkpoint { int32_t day; float kwh; } saved = {};
        if (nvs.getBytes("gen_meter", &saved, sizeof(saved)) == sizeof(saved) && saved.day == today)
          meter.kwh = saved.kwh;
        nvs.end();
      }
      if (!isfinite(meter.kwh) || meter.kwh < 0) meter.kwh = 0;
      meter.day = today;
    }
    if (v2_gen_daily_uses_register(cfg_v2)) {
      energy_ok = solarman_read_block(cfg_v2.gen_daily_register, 1, rtu, &exception);
      if (energy_ok) meter.kwh = modbus_get_u16_be(rtu, 0) * cfg_v2.gen_daily_scale;
      meter.previous_valid = false;
    } else {
      meter.sample(today, gen_ok, power, sampled_ms);
      energy_ok = true;
      // Bounded flash wear: at most one checkpoint every five minutes.
      if (uint32_t(sampled_ms - last_save_ms) >= 300000) {
        Preferences nvs;
        if (nvs.begin(inverter_profile().storage, false)) {
          const struct Checkpoint { int32_t day; float kwh; } saved = {meter.day, meter.kwh};
          nvs.putBytes("gen_meter", &saved, sizeof(saved)); nvs.end();
        }
        last_save_ms = sampled_ms;
      }
    }
  } else meter.previous_valid = false;
  bool relay_ok = false;
  float relay_value = 0;
  if (cfg_v2.relay_enabled) {
    relay_ok = solarman_read_block(cfg_v2.relay_register, 1, rtu, &exception);
    if (relay_ok) {
      const uint16_t value = modbus_get_u16_be(rtu, 0);
      relay_value = (cfg_v2.relay_signed ? float(int16_t(value)) : float(value)) * cfg_v2.relay_coefficient;
    }
  }
  portENTER_CRITICAL(&v2_measure_lock);
  v2_measurements.gen_valid = gen_ok;
  v2_measurements.gen_ms = sampled_ms;
  v2_measurements.gen_power = power;
  v2_measurements.gen_kwh = meter.kwh;
  v2_measurements.energy_day = meter.day;
  v2_measurements.energy_valid = energy_ok;
  v2_measurements.relay.accept(relay_ok, relay_value, millis());
  portEXIT_CRITICAL(&v2_measure_lock);
  xSemaphoreTake(data_mutex, portMAX_DELAY);
  main_data.gen_power = raw;
  update_dashboard_from_data();
  if (core_sample_ok && (!v2_gen_included() || gen_ok)) history_add_snapshot(dashboard_data);
  xSemaphoreGive(data_mutex);
}

static uint32_t v2_tariff_last_attempt = 0;
static uint8_t v2_tariff_last_target = 0;
static void v2_enforce_tariff() {
  if (!cfg_v2.ev_tariff_enabled || !cfg_ev_charger_enabled || !cfg_ev_registers.write_enabled) return;
  EvDeyeData data = {};
  if (!deye_copy_ev_snapshot(&data) || !data.valid || deye_ev_command_busy(data.command_state)) return;
  if (!deye_ev_mode_write_supported(data.mode_raw)) { v2_tariff_owned_mode = false; return; }
  const bool allowed = v2_tariff().grid_allowed;
  const uint8_t mode = data.mode_raw & 3;
  const bool restrict = !allowed && mode == 2;
  const bool restore = allowed && mode == 1 && v2_tariff_owned_mode;
  if (!restrict && !restore) return;
  const uint8_t target = restrict ? 1 : 2;
  if (v2_tariff_last_attempt && v2_tariff_last_target == target &&
      uint32_t(millis() - v2_tariff_last_attempt) < 60000) return;
  EvDeyeCommand command = {};
  command.set_mode = true;
  command.mode = target;
  command.tariff_automatic = true;
  if (deye_submit_ev_command(command)) { v2_tariff_last_attempt = millis(); v2_tariff_last_target = target; }
}
