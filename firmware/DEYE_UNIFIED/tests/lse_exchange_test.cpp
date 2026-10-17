#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "../logger_connection.h"
#include "../modbus_tcp_protocol.h"
static uint32_t tick;
static uint32_t millis() { return tick; }
static void delay(uint32_t n) { tick += n; }
static std::string cfg_deye_host = "192.168.1.50";
static uint16_t cfg_deye_port = 502;
static uint8_t cfg_deye_unit = 7;
static uint32_t TCP_CONNECT_TIMEOUT_MS = 100, RESPONSE_WINDOW_MS = 3000;
static bool solarman_last_exchange_replied;
static struct { template<class... T> void printf(const char *, T...) {} } DBG;
enum Scenario { Normal, Exception, StaleThenValid, WrongUnit, Truncated, Oversize, WrongEcho, WriteFailure, NoConnection };
static Scenario scenario;
static bool closed;
class WiFiClient {
  std::vector<uint8_t> data;
  size_t position = 0;
public:
  bool connect(const char *host, int port, int timeout) {
    assert(std::string(host) == cfg_deye_host && port == 502 && timeout == 100);
    return scenario != NoConnection;
  }
  size_t write(const uint8_t *request, size_t length) {
    assert(length == 12 || length == 15); // FC03 or FC10, without RTU CRC/V5.
    assert(request[2] == 0 && request[3] == 0 && request[6] == 7);
    assert(size_t(request[5]) + 6 == length);
    if (scenario == WriteFailure) return length - 1;
    const int count = (request[10] << 8) | request[11];
    data.assign(request, request + 8);
    if (scenario == Exception) { data[5] = 3; data[7] |= 0x80; data.push_back(2); }
    else if (request[7] == 3) {
      const int payload = 3 + 2 * count;
      data[4] = payload >> 8; data[5] = payload;
      data.push_back(2 * count);
      for (int i=0; i<count; ++i) { data.push_back(i >> 8); data.push_back(i); }
    } else {
      data[5] = 6;
      data.insert(data.end(), request + 8, request + 12);
      if (scenario == WrongEcho) data[9] ^= 1;
    }
    if (scenario == WrongUnit) data[6] ^= 1;
    if (scenario == Oversize) { data[4] = 0x7f; data[5] = 0xff; }
    if (scenario == Truncated) data.resize(8);
    if (scenario == StaleThenValid) {
      auto valid = data; data[1] ^= 1;
      data.insert(data.end(), valid.begin(), valid.end());
    }
    return length;
  }
  int available() {
    // Release bytes gradually, including splits inside MBAP and register data.
    return int(std::min(data.size(), size_t(tick / 2)) - position);
  }
  int read() { return data[position++]; }
  void stop() { closed = true; }
};
#include "lse_exchange_under_test.h"
static bool exchange(Scenario value, int count = 2, bool write = false) {
  scenario = value; tick = 0; closed = false; solarman_last_exchange_replied = false;
  uint8_t request[11] = {7,3,0,184,0,uint8_t(count),0,0};
  if (write) deye_modbus_write_one(7, 489, 3200, request);
  else { const auto crc = modbus_crc16(request,6); request[6] = crc; request[7] = crc >> 8; }
  uint8_t response[256] = {}, error = 0;
  const bool ok = modbus_tcp_exchange(request, write ? 11 : 8, write ? 489 : 184,
    write ? 1 : count, response, sizeof(response), &error);
  if (value != NoConnection) assert(closed);
  if (ok) {
    const size_t size = write ? 8 : 5 + 2 * count;
    assert(modbus_crc16(response,size-2) == (response[size-2] | (response[size-1] << 8)));
    assert(solarman_last_exchange_replied);
  }
  if (value == Exception) assert(!ok && error == 2 && solarman_last_exchange_replied);
  if (value == WrongUnit || value == Truncated || value == Oversize || value == WrongEcho)
    assert(!solarman_last_exchange_replied);
  return ok;
}
int main() {
  assert(exchange(Normal)); assert(exchange(Normal,125));
  assert(exchange(Normal,1,true)); assert(exchange(StaleThenValid));
  for (auto s : {Exception,WrongUnit,Truncated,Oversize,WriteFailure,NoConnection}) assert(!exchange(s));
  assert(!exchange(WrongEcho,1,true)); assert(!exchange(Exception,1,true));
  assert(deye_connection_valid(DeyeTransport::ModbusTcp,0,502,7));
  assert(!deye_connection_valid(DeyeTransport::SolarmanV5,0,8899,1));
  assert(!deye_connection_valid(DeyeTransport::ModbusTcp,0,0,1));
  assert(!deye_connection_valid(DeyeTransport::ModbusTcp,0,502,248));
  puts("LSE: production exchange, fragmented reads, FC10, exceptions and identity checks passed");
}
