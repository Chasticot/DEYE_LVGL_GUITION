#pragma once
#include "ve_modbus_codec.h"

// The application keeps RTU-shaped buffers internally; only MBAP + PDU goes
// onto the wire. No logger serial and no RTU CRC are transmitted in TCP mode.
static inline size_t deye_tcp_request(const uint8_t *rtu, size_t length,
                                     uint16_t transaction, uint8_t *out, size_t capacity) {
  if (!rtu || !out || length < 8 || length > 256 || capacity < length + 4 ||
      (rtu[1] != 3 && rtu[1] != 0x10)) return 0;
  const size_t payload = length - 2;
  out[0] = transaction >> 8; out[1] = transaction;
  out[2] = out[3] = 0;
  out[4] = payload >> 8; out[5] = payload;
  memcpy(out + 6, rtu, payload);
  return payload + 6;
}

static inline DeyeRtuResult deye_tcp_response(const uint8_t *adu, size_t length,
  uint16_t transaction, uint8_t unit, uint8_t function, uint16_t reg, uint16_t count,
  uint8_t *out, size_t capacity, uint8_t *exception) {
  if (!adu || length < 9 || length > 260 ||
      ((uint16_t(adu[0]) << 8) | adu[1]) != transaction || adu[2] || adu[3] ||
      size_t((uint16_t(adu[4]) << 8) | adu[5]) + 6 != length || adu[6] != unit) return DEYE_RTU_IGNORE;
  const bool error = adu[7] == (function | 0x80);
  if (error ? length != 9 : (adu[7] != function ||
      (function == 3 ? (length != 9 + size_t(count) * 2 || adu[8] != count * 2) :
       (function != 0x10 || length != 12)))) return DEYE_RTU_IGNORE;
  uint8_t rtu[256];
  const size_t payload = length - 6;
  memcpy(rtu, adu + 6, payload);
  const uint16_t crc = modbus_crc16(rtu, payload);
  rtu[payload] = crc; rtu[payload + 1] = crc >> 8;
  return deye_modbus_response(rtu, payload + 2, unit, function, reg, count, out, capacity, exception);
}
