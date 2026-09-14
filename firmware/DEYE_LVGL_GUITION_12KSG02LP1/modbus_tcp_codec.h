#pragma once

#include <stddef.h>
#include <stdint.h>

// Validate a complete Modbus TCP ADU and convert its read-register response
// into the RTU-shaped buffer used by the existing decoder.
// `rtu_out` must hold at least 5 + 2 * register_count bytes.
static inline bool modbus_tcp_read_response_to_rtu(
  const uint8_t *adu,
  size_t adu_len,
  uint16_t expected_transaction_id,
  uint8_t expected_unit_id,
  uint16_t register_count,
  uint8_t *rtu_out
) {
  if (adu == nullptr || rtu_out == nullptr || register_count == 0 || register_count > 125) {
    return false;
  }
  if (adu_len < 9) return false;

  const uint16_t transaction_id = ((uint16_t)adu[0] << 8) | adu[1];
  const uint16_t protocol_id = ((uint16_t)adu[2] << 8) | adu[3];
  const uint16_t mbap_length = ((uint16_t)adu[4] << 8) | adu[5];

  // An ADU contains six bytes before the MBAP length payload.
  if (transaction_id != expected_transaction_id || protocol_id != 0 ||
      mbap_length < 3 || adu_len != (size_t)mbap_length + 6 ||
      adu[6] != expected_unit_id) {
    return false;
  }

  const uint8_t function_code = adu[7];
  if (function_code != 0x03) return false;  // Reject Modbus exceptions too.

  const uint8_t byte_count = adu[8];
  const size_t expected_data_bytes = (size_t)register_count * 2;
  if (byte_count != expected_data_bytes || mbap_length != 3 + expected_data_bytes) {
    return false;
  }

  rtu_out[0] = expected_unit_id;
  rtu_out[1] = 0x03;
  rtu_out[2] = byte_count;
  for (size_t i = 0; i < expected_data_bytes; ++i) {
    rtu_out[3 + i] = adu[9 + i];
  }
  // The existing decoder does not consume the RTU CRC, but preserve its shape.
  rtu_out[3 + expected_data_bytes] = 0;
  rtu_out[4 + expected_data_bytes] = 0;
  return true;
}
