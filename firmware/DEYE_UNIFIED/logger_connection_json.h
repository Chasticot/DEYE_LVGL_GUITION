#pragma once
#include <ArduinoJson.h>
#include <math.h>
#include "logger_connection.h"

struct DeyeConnectionImport {
  DeyeTransport transport = DeyeTransport::SolarmanV5;
  uint16_t port = 8899;
  uint8_t unit = 1;
  uint32_t serial = 0;
  LoggerModel model = LoggerModel::Lsw;
  char identity[33] = {};
};
static inline bool deye_json_uint(JsonObjectConst json, const char *key, uint32_t max, uint32_t &out) {
  if (!json[key].is<double>()) return false;
  const double value = json[key].as<double>();
  if (!isfinite(value) || value < 0 || value > max || floor(value) != value) return false;
  out = uint32_t(value); return true;
}
static inline bool deye_connection_from_json(JsonObjectConst json, DeyeConnectionImport &out) {
  DeyeConnectionImport candidate;
  uint32_t port = 8899, unit = 1;
  if (json.containsKey("logger_transport")) {
    if (!json["logger_transport"].is<const char *>() ||
        !deye_transport_parse(json["logger_transport"].as<const char *>(), candidate.transport) ||
        !deye_json_uint(json,"logger_port",65535,port) || !deye_json_uint(json,"logger_unit",247,unit)) return false;
  } else if (json.containsKey("logger_port") || json.containsKey("logger_unit")) return false;
  if (!deye_json_uint(json,"logger_serial",UINT32_MAX,candidate.serial) ||
      !deye_connection_valid(candidate.transport,candidate.serial,port,unit)) return false;
  candidate.model = logger_infer_model(candidate.transport, uint16_t(port));
  if (json.containsKey("logger_model")) {
    if (!json["logger_model"].is<const char *>() || !json["logger_serial_text"].is<const char *>() ||
        !logger_model_parse(json["logger_model"].as<const char *>(), candidate.model) ||
        logger_transport(candidate.model) != candidate.transport) return false;
    uint32_t numeric = 0;
    if (!logger_identity_parse(json["logger_serial_text"].as<const char *>(), candidate.model,
        candidate.identity, sizeof(candidate.identity), numeric, true) || numeric != candidate.serial) return false;
  } else {
    if (json.containsKey("logger_serial_text")) return false;
    snprintf(candidate.identity, sizeof(candidate.identity), "%lu", (unsigned long)candidate.serial);
  }
  candidate.port = uint16_t(port); candidate.unit = uint8_t(unit);
  out = candidate; return true;
}
