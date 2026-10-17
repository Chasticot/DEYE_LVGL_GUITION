#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "logger_connection.h"

enum DeyeLoggerDiscoveryState : uint8_t {
  DEYE_LOGGER_DISCOVERY_IDLE, DEYE_LOGGER_DISCOVERY_QUEUED,
  DEYE_LOGGER_DISCOVERY_RUNNING, DEYE_LOGGER_DISCOVERY_FOUND,
  DEYE_LOGGER_DISCOVERY_NOT_FOUND, DEYE_LOGGER_DISCOVERY_ERROR,
  DEYE_LOGGER_DISCOVERY_CANDIDATES
};
enum DeyeLoggerDiscoveryPhase : uint8_t {
  DEYE_LOGGER_PHASE_NONE, DEYE_LOGGER_PHASE_BROADCAST,
  DEYE_LOGGER_PHASE_TCP, DEYE_LOGGER_PHASE_UNICAST, DEYE_LOGGER_PHASE_MODBUS
};
static inline const char *logger_discovery_phase_name(DeyeLoggerDiscoveryPhase phase) {
  switch (phase) {
    case DEYE_LOGGER_PHASE_BROADCAST: return "UDP broadcast";
    case DEYE_LOGGER_PHASE_TCP: return "TCP/V5";
    case DEYE_LOGGER_PHASE_UNICAST: return "UDP unicast";
    case DEYE_LOGGER_PHASE_MODBUS: return "Modbus TCP";
    default: return "Demarrage";
  }
}
struct DeyeLoggerDiscoveryStatus {
  DeyeLoggerDiscoveryState state = DEYE_LOGGER_DISCOVERY_IDLE;
  DeyeLoggerDiscoveryPhase phase = DEYE_LOGGER_PHASE_NONE;
  bool automatic = false, restart_pending = false, save_failed = false;
  uint16_t scanned_hosts = 0, total_hosts = 0;
  uint32_t serial = 0, generation = 0;
  char host[16] = {}, mac[18] = {};
  LoggerModel model = LoggerModel::Lsw;
  uint16_t port = 8899;
  uint8_t unit = 1, candidate_count = 0;
  char identity[33] = {}, candidates[8][16] = {};
};
static inline bool logger_discovery_busy(DeyeLoggerDiscoveryState state) {
  return state == DEYE_LOGGER_DISCOVERY_QUEUED || state == DEYE_LOGGER_DISCOVERY_RUNNING;
}
static inline bool logger_discovery_serial(const char *text, uint32_t *out) {
  if (!text || !out) return false;
  while (*text == ' ' || *text == '\t') ++text;
  uint32_t value = 0;
  bool digits = false;
  while (*text >= '0' && *text <= '9') {
    const uint32_t digit = uint32_t(*text++ - '0');
    if (value > (UINT32_MAX - digit) / 10) return false;
    value = value * 10 + digit;
    digits = true;
  }
  while (*text == '\r' || *text == '\n' || *text == ' ' || *text == '\t') ++text;
  if (!digits || !value || *text) return false;
  *out = value;
  return true;
}
// Some generations return an alphanumeric hostname/serial in the same UDP reply.
static inline bool logger_discovery_parse_identity_reply(char *reply, const char *source,
  char *identity, size_t identity_capacity, char *mac, size_t capacity) {
  if (!reply || !source || !identity || !mac || capacity < 18 || identity_capacity < 33) return false;
  char *first = strchr(reply, ','), *second = first ? strchr(first+1, ',') : nullptr;
  if (!second) return false;
  *first = *second = 0;
  if (strcmp(reply, source)) return false;
  const size_t len = strlen(first+1);
  if (len != 12 && len != 17) return false;
  for (size_t i=0; i<len; ++i) {
    const char c=first[1+i];
    if (len==17 && i%3==2) { if (c != ':') return false; }
    else if (!((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))) return false;
  }
  char *sn=second+1;
  size_t n=strlen(sn);
  while (n && (sn[n-1]=='\r' || sn[n-1]=='\n' || sn[n-1]==' ')) sn[--n]=0;
  uint32_t unused;
  if (!logger_identity_parse(sn, LoggerModel::Dl1000B, identity, identity_capacity, unused)) return false;
  snprintf(mac,capacity,"%s",first+1);
  return true;
}
// IP,MAC,SN : retain only a reply whose IP matches the actual UDP sender.
static inline bool logger_discovery_parse_reply(char *reply, const char *source,
                                                uint32_t *serial, char *mac, size_t capacity) {
  if (!reply || !source || !serial || !mac || capacity < 18) return false;
  char *first = strchr(reply, ',');
  if (!first) return false;
  char *second = strchr(first + 1, ',');
  if (!second) return false;
  *first = *second = '\0';
  if (strcmp(reply, source)) return false;
  const size_t len = strlen(first + 1);
  // Solarman firmware uses either 12 hex digits or a colon-separated MAC.
  if (len != 12 && len != 17) return false;
  for (size_t i = 0; i < len; ++i) {
    const char c = first[1 + i];
    if (len == 17 && i % 3 == 2) { if (c != ':') return false; }
    else if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) return false;
  }
  uint32_t value;
  if (!logger_discovery_serial(second + 1, &value)) return false;
  snprintf(mac, capacity, "%s", first + 1);
  *serial = value;
  return true;
}
struct LoggerDiscoverySubnet {
  uint32_t network;
  uint16_t last_host, total_hosts;
};
static inline LoggerDiscoverySubnet logger_discovery_subnet(uint32_t local, uint32_t mask) {
  const uint32_t hosts = ~mask;
  const bool contiguous = (hosts & (hosts + 1)) == 0;
  const bool bounded = contiguous && hosts >= 3 && hosts <= 255;
  const uint32_t network = local & (bounded ? mask : 0xFFFFFF00UL);
  const uint16_t last = bounded ? uint16_t(hosts - 1) : 254;
  const uint32_t own = local - network;
  return {network, last, uint16_t(last - (own >= 1 && own <= last ? 1 : 0))};
}
