#include "network_config_testable.h"
constexpr bool parse_is(const char *text, uint32_t expected) {
  uint32_t result = 0;
  return network_parse_ipv4(text, result) && result == expected;
}
constexpr bool rejects(const char *text) {
  uint32_t result = 0;
  return !network_parse_ipv4(text, result);
}
constexpr NetworkConfig network(uint32_t ip, uint32_t mask, uint32_t gateway, uint32_t dns=0) {
  NetworkConfig cfg;
  cfg.use_static=1; cfg.ip=ip; cfg.mask=mask; cfg.gateway=gateway; cfg.dns=dns;
  return cfg;
}
static_assert(parse_is("192.168.1.42",0xc0a8012a), "IPv4 byte order");
static_assert(parse_is("255.255.255.255",0xffffffff), "IPv4 limit");
static_assert(parse_is("0.0.0.0",0), "IPv4 zero");
static_assert(rejects("256.1.1.1") && rejects("1.2.3") && rejects("1.2.3.4.5"), "Invalid IPv4");
static_assert(rejects("") && rejects("1..3.4") && rejects("-1.2.3.4") && rejects("1.2.3.4x"), "Malformed IPv4");
static_assert(network_config_valid(NetworkConfig{}), "Default DHCP");
static_assert(network_config_valid(network(0xc0a8012a,0xffffff00,0xc0a80101)), "Valid static subnet");
static_assert(network_config_valid(network(0x0a000102,0xffff0000,0x0a000001,0x08080808)), "Valid /16 with DNS");
static_assert(!network_config_valid(network(0xc0a8012a,0xff00ff00,0xc0a80101)), "Noncontiguous mask");
static_assert(!network_config_valid(network(0xc0a8012a,0,0xc0a80101)), "Empty mask");
static_assert(!network_config_valid(network(0xc0a8012a,0xffffffff,0xc0a80101)), "Host-only mask");
static_assert(!network_config_valid(network(0xc0a80100,0xffffff00,0xc0a80101)), "Network address");
static_assert(!network_config_valid(network(0xc0a801ff,0xffffff00,0xc0a80101)), "Broadcast address");
static_assert(!network_config_valid(network(0xc0a8012a,0xffffff00,0xc0a80201)), "Off-subnet gateway");
static_assert(!network_config_valid(network(0xc0a8012a,0xffffff00,0xc0a8012a)), "Gateway is device");
static_assert(!network_config_valid(network(0x7f000002,0xffffff00,0x7f000001)), "Loopback");
static_assert(!network_config_valid(network(0xe0000002,0xffffff00,0xe0000001)), "Multicast");
static_assert(!network_config_valid(network(0xc0a8012a,0xffffff00,0xc0a80101,0xe0000001)), "Multicast DNS");
static_assert(sizeof(NetworkConfig)==24, "NVS record layout");
