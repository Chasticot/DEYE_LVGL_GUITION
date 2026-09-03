#include <array>
#include <cstdint>

#include "modbus_tcp_codec.h"

static bool accepts_valid_read_response() {
  const std::array<uint8_t, 13> adu = {
    0x12, 0x34, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x00, 0x0A, 0x00, 0x0B
  };
  std::array<uint8_t, 9> rtu = {};
  if (!modbus_tcp_read_response_to_rtu(adu.data(), adu.size(), 0x1234, 0x01, 2, rtu.data())) return false;
  return rtu[0] == 0x01 && rtu[1] == 0x03 && rtu[2] == 0x04 &&
         rtu[3] == 0x00 && rtu[4] == 0x0A && rtu[5] == 0x00 && rtu[6] == 0x0B;
}

static bool rejects_invalid_frames() {
  std::array<uint8_t, 13> adu = {
    0x12, 0x34, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x00, 0x0A, 0x00, 0x0B
  };
  std::array<uint8_t, 9> rtu = {};
  adu[1] = 0x35;
  if (modbus_tcp_read_response_to_rtu(adu.data(), adu.size(), 0x1234, 0x01, 2, rtu.data())) return false;
  adu[1] = 0x34;
  adu[8] = 0x03;
  if (modbus_tcp_read_response_to_rtu(adu.data(), adu.size(), 0x1234, 0x01, 2, rtu.data())) return false;
  adu[8] = 0x04;
  adu[7] = 0x83;
  return !modbus_tcp_read_response_to_rtu(adu.data(), adu.size(), 0x1234, 0x01, 2, rtu.data());
}

int main() {
  return accepts_valid_read_response() && rejects_invalid_frames() ? 0 : 1;
}
