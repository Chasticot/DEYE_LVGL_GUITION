#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>

enum class LoggerModel : uint8_t { Lsw = 0, Lse = 1, Dl1000B = 2 };
static inline bool logger_model_valid(LoggerModel model) { return uint8_t(model) <= 2; }
static inline const char *logger_model_key(LoggerModel model) {
  return model == LoggerModel::Lse ? "lse" : model == LoggerModel::Dl1000B ? "dl1000b" : "lsw";
}
static inline bool logger_model_parse(const char *key, LoggerModel &model) {
  if (!key) return false;
  for (uint8_t i = 0; i < 3; ++i) if (!strcmp(key, logger_model_key(LoggerModel(i)))) { model = LoggerModel(i); return true; }
  return false;
}
static inline uint16_t logger_default_port(LoggerModel model) { return model == LoggerModel::Dl1000B ? 502 : 8899; }
// Keep the full DL1000B identity (including letters); never truncate it to a V5 number.
static inline bool logger_identity_parse(const char *text, LoggerModel model, char *out,
  size_t capacity, uint32_t &numeric, bool allow_zero = false) {
  if (!text || !out || capacity < 33 || !logger_model_valid(model)) return false;
  const size_t length = strlen(text);
  if (!length || length > 32) return false;
  numeric = 0; bool digits = true, overflow = false;
  for (size_t i = 0; i < length; ++i) {
    const char c = text[i];
    if (!((c >= '0' && c <= '9') || (model == LoggerModel::Dl1000B &&
        ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-')))) return false;
    out[i] = c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c;
    if (c < '0' || c > '9') digits = false;
    if (digits && !overflow) {
      if (numeric > (UINT32_MAX - uint32_t(c-'0')) / 10) overflow = true;
      else numeric = numeric * 10 + uint32_t(c-'0');
    }
  }
  out[length] = 0;
  if (model != LoggerModel::Dl1000B) {
    if (overflow || (!numeric && !allow_zero)) return false;
    snprintf(out, capacity, "%lu", (unsigned long)numeric);
  } else {
    if (digits && !overflow && !numeric && !allow_zero) return false;
    if (!digits || overflow) numeric = 0;
  }
  return true;
}

enum class DeyeTransport : uint8_t { SolarmanV5 = 0, ModbusTcp = 1 };
static inline DeyeTransport logger_transport(LoggerModel model) { return model == LoggerModel::Lsw ? DeyeTransport::SolarmanV5 : DeyeTransport::ModbusTcp; }
static inline LoggerModel logger_infer_model(DeyeTransport transport, uint16_t port) {
  return transport == DeyeTransport::SolarmanV5 ? LoggerModel::Lsw : port == 502 ? LoggerModel::Dl1000B : LoggerModel::Lse;
}
static inline const char *deye_transport_key(DeyeTransport mode) {
  return mode == DeyeTransport::ModbusTcp ? "modbus_tcp" : "solarman_v5";
}
static inline bool deye_transport_parse(const char *key, DeyeTransport &mode) {
  if (!key) return false;
  if (!strcmp(key, "solarman_v5")) mode = DeyeTransport::SolarmanV5;
  else if (!strcmp(key, "modbus_tcp")) mode = DeyeTransport::ModbusTcp;
  else return false;
  return true;
}
static inline bool deye_connection_valid(DeyeTransport mode, uint32_t serial, uint32_t port, uint32_t unit) {
  return (mode == DeyeTransport::SolarmanV5 || mode == DeyeTransport::ModbusTcp) &&
    port > 0 && port <= 65535 && unit > 0 && unit <= 247 &&
    (mode == DeyeTransport::ModbusTcp || serial != 0);
}
