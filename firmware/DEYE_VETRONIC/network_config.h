#pragma once
#include <stdint.h>

struct NetworkConfig {
  uint32_t version = 1;
  uint32_t use_static = 0;
  uint32_t ip = 0;
  uint32_t mask = 0;
  uint32_t gateway = 0;
  uint32_t dns = 0;
};

static bool network_parse_ipv4(const char *text, uint32_t &result) {
  if (!text) return false;
  uint32_t value = 0;
  for (int part = 0; part < 4; ++part) {
    unsigned octet = 0, digits = 0;
    while (*text >= '0' && *text <= '9') {
      octet = octet * 10 + (*text++ - '0');
      if (++digits > 3 || octet > 255) return false;
    }
    if (!digits) return false;
    value = (value << 8) | octet;
    if (part < 3) { if (*text++ != '.') return false; }
    else if (*text != '\0') return false;
  }
  result = value;
  return true;
}

static bool network_unicast(uint32_t ip) {
  return (ip >> 24) != 0 && (ip >> 24) != 127 && (ip >> 24) < 224;
}

static bool network_config_valid(const NetworkConfig &cfg) {
  if (cfg.version != 1 || cfg.use_static > 1) return false;
  if (!cfg.use_static) return true;
  const uint32_t host_bits = ~cfg.mask;
  // Contiguous /1 through /30 mask, with usable host addresses.
  if (!cfg.mask || host_bits < 3 || (host_bits & (host_bits + 1))) return false;
  const uint32_t ip_host = cfg.ip & host_bits, gateway_host = cfg.gateway & host_bits;
  return network_unicast(cfg.ip) && network_unicast(cfg.gateway) &&
    ip_host && ip_host != host_bits && gateway_host && gateway_host != host_bits &&
    cfg.ip != cfg.gateway && (cfg.ip & cfg.mask) == (cfg.gateway & cfg.mask) &&
    (!cfg.dns || network_unicast(cfg.dns));
}
