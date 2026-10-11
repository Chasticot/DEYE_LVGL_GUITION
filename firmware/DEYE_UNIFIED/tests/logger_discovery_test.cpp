#include <assert.h>
#include <string.h>
#include <initializer_list>
#include "../logger_discovery_logic.h"

static bool parse(const char *text, const char *source = "192.168.1.12") {
  char reply[128], mac[18]; uint32_t serial = 0;
  snprintf(reply, sizeof(reply), "%s", text);
  return logger_discovery_parse_reply(reply, source, &serial, mac, sizeof(mac));
}
int main() {
  assert(parse("192.168.1.12,AABBCCDDEEFF,2712345678\r\n"));
  assert(parse("192.168.1.12,aa:bb:cc:dd:ee:ff,4294967295"));
  assert(!parse("192.168.1.13,AABBCCDDEEFF,2712345678"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,4294967296"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,-1"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,+1"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,0"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,123,garbage"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF,  \r\n"));
  assert(!parse("192.168.1.12,garbage,2712345678"));
  assert(!parse("WIFIKIT-214028-READ"));
  assert(!parse("192.168.1.12,AABBCCDDEEFF"));
  uint32_t serial = 45;
  assert(!logger_discovery_serial("999999999999999999999", &serial) && serial == 45);
  assert(logger_discovery_serial(" 2712345678\r\n", &serial) && serial == 2712345678UL);
  assert(!logger_discovery_serial(nullptr, &serial));
  const auto lan24 = logger_discovery_subnet(0xC0A80110, 0xFFFFFF00);
  assert(lan24.network == 0xC0A80100 && lan24.last_host == 254 && lan24.total_hosts == 253);
  const auto lan25 = logger_discovery_subnet(0xC0A80190, 0xFFFFFF80);
  assert(lan25.network == 0xC0A80180 && lan25.last_host == 126 && lan25.total_hosts == 125);
  const auto lan30 = logger_discovery_subnet(0xC0A80105, 0xFFFFFFFC);
  assert(lan30.network == 0xC0A80104 && lan30.last_host == 2 && lan30.total_hosts == 1);
  for (auto mask : {0xFFFF0000UL, 0xFFFFFFFEUL, 0xFFFFFFFFUL, 0UL, 0xFFFF00FFUL}) {
    const auto bounded = logger_discovery_subnet(0xC0A80110, mask);
    assert(bounded.network == 0xC0A80100 && bounded.last_host == 254 && bounded.total_hosts == 253);
  }
  assert(logger_discovery_busy(DEYE_LOGGER_DISCOVERY_QUEUED));
  assert(logger_discovery_busy(DEYE_LOGGER_DISCOVERY_RUNNING));
  assert(!logger_discovery_busy(DEYE_LOGGER_DISCOVERY_FOUND));
  puts("Logger reply parsing and subnet tests passed.");
}
