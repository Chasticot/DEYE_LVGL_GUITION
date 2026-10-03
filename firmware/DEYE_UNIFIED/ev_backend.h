#pragma once
#include <stdint.h>
#include <string.h>

enum class EvBackend : uint8_t { None = 0, DeyeLoRa = 1, VetronicWb01 = 2 };

static inline const char *ev_backend_key(EvBackend backend) {
  switch (backend) {
    case EvBackend::None: return "none";
    case EvBackend::DeyeLoRa: return "deye_lora";
    case EvBackend::VetronicWb01: return "vetronic_wb01";
  }
  return "invalid";
}
static inline const char *ev_backend_name(EvBackend backend) {
  switch (backend) {
    case EvBackend::None: return "Aucune borne";
    case EvBackend::DeyeLoRa: return "Deye LoRa";
    case EvBackend::VetronicWb01: return "VE TRONIC / WB01";
  }
  return "Choix invalide";
}
static inline bool ev_backend_parse(const char *key, EvBackend &out) {
  if (!key) return false;
  const EvBackend backends[] = {EvBackend::None, EvBackend::DeyeLoRa, EvBackend::VetronicWb01};
  for (auto backend : backends) {
    if (strcmp(key, ev_backend_key(backend)) == 0) { out = backend; return true; }
  }
  return false;
}
static inline bool ev_backend_supported(EvBackend backend, bool native_supported) {
  return backend == EvBackend::None || backend == EvBackend::VetronicWb01 ||
    (backend == EvBackend::DeyeLoRa && native_supported);
}
// Used at the final FC06 boundary as well as at the command queue.
static inline bool ev_native_write_allowed(EvBackend backend, bool native_supported,
    bool unlocked, uint16_t reg, uint16_t mode_reg, uint16_t power_reg) {
  return backend == EvBackend::DeyeLoRa && native_supported && unlocked &&
    mode_reg != power_reg && (reg == mode_reg || reg == power_reg);
}
