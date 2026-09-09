#include "../ve_modbus_codec.h"
#include <assert.h>
#include <stdio.h>

static void crc_append(uint8_t *p, size_t n) {
  uint16_t crc = modbus_crc16(p, n);
  p[n] = crc & 255; p[n+1] = crc >> 8;
}
static size_t read_reply(uint8_t *p, uint16_t count) {
  p[0] = 1; p[1] = 3; p[2] = count * 2;
  for (uint16_t i = 0; i < count; ++i) { p[3 + i*2] = i >> 8; p[4 + i*2] = i & 255; }
  crc_append(p, 3 + count*2);
  return 5 + count*2;
}
static void checksum(uint8_t *p, size_t n) {
  uint8_t sum = 0;
  for (size_t i = 1; i < n-2; ++i) sum += p[i];
  p[n-2] = sum;
}
int main(void) {
  assert(deye_modbus_range_valid(169, 92));
  assert(deye_modbus_contains(169, 92, 259));
  assert(deye_modbus_contains(169, 92, 260));
  assert(!deye_modbus_contains(169, 27, 259));
  assert(deye_modbus_range_valid(0, 125));
  assert(!deye_modbus_range_valid(0, 126));
  assert(!deye_modbus_range_valid(709, 0));
  assert(!deye_modbus_range_valid(65535, 2));
  assert(deye_modbus_range_valid(65535, 1));
  for (uint32_t raw = 0; raw <= 65535; ++raw) {
    for (uint8_t mode = 1; mode <= 2; ++mode) {
      uint16_t updated = deye_ev_replace_mode((uint16_t)raw, mode);
      assert((updated & 0xFFFC) == (raw & 0xFFFC));
      assert((updated & 3) == mode);
    }
  }
  // Trame publiee R259 = 0x32EE : controle independant du CRC/encodage.
  const uint8_t expected[] = {1, 0x10, 1, 3, 0, 1, 2, 0x32, 0xEE, 0x23, 0x8F};
  uint8_t write_request[11];
  deye_modbus_write_one(1, 259, 0x32EE, write_request);
  assert(memcmp(expected, write_request, sizeof(expected)) == 0);
  uint8_t p[320] = {0}, out[255] = {0}, exception = 0;
  const uint16_t counts[] = {1, 92, 125};
  for (size_t k = 0; k < 3; ++k) {
    size_t n = read_reply(p, counts[k]);
    assert(deye_modbus_response(p, n, 1, 3, 169, counts[k], out, sizeof(out), &exception) == DEYE_RTU_OK);
    for (size_t truncated = 0; truncated < n; ++truncated)
      assert(deye_modbus_response(p, truncated, 1, 3, 169, counts[k], out, sizeof(out), &exception) == DEYE_RTU_IGNORE);
    assert(deye_modbus_response(p, n, 2, 3, 169, counts[k], out, sizeof(out), &exception) == DEYE_RTU_IGNORE);
    assert(deye_modbus_response(p, n, 1, 3, 169, counts[k], out, n-1, &exception) == DEYE_RTU_IGNORE);
    p[n-1] ^= 1;
    assert(deye_modbus_response(p, n, 1, 3, 169, counts[k], out, sizeof(out), &exception) == DEYE_RTU_IGNORE);
  }
  uint8_t ack[] = {1, 0x10, 1, 4, 0, 1, 0, 0};
  crc_append(ack, 6);
  assert(deye_modbus_response(ack, 8, 1, 0x10, 260, 1, out, sizeof(out), &exception) == DEYE_RTU_OK);
  assert(deye_modbus_response(ack, 8, 1, 0x10, 259, 1, out, sizeof(out), &exception) == DEYE_RTU_IGNORE);
  assert(deye_modbus_response(ack, 8, 1, 3, 260, 1, out, sizeof(out), &exception) == DEYE_RTU_IGNORE);
  uint8_t error[] = {1, 0x90, 2, 0, 0};
  crc_append(error, 3);
  assert(deye_modbus_response(error, 5, 1, 0x10, 260, 1, out, sizeof(out), &exception) == DEYE_RTU_EXCEPTION);
  assert(exception == 2);
  error[1] = 0x83; crc_append(error, 3);
  assert(deye_modbus_response(error, 5, 1, 3, 709, 1, out, sizeof(out), &exception) == DEYE_RTU_EXCEPTION);
  // Cadre V5 > 256 octets, 125 registres. CRC RTU et checksum V5 distincts.
  uint8_t request[11] = {0};
  request[5] = 37; request[7] = 0x12; request[8] = 0x34;
  memset(p, 0, sizeof(p));
  size_t rtu_len = read_reply(p + 26, 125);
  const size_t frame_len = 26 + rtu_len + 2;
  p[0] = 0xA5; p[1] = (frame_len-13) & 255; p[2] = (frame_len-13) >> 8;
  p[3] = 0x10; p[4] = 0x15; p[5] = 37; p[7] = 0x12; p[8] = 0x34;
  p[frame_len-1] = 0x15; checksum(p, frame_len);
  assert(frame_len > 256 && frame_len <= sizeof(p));
  assert(deye_v5_reply_matches(p, frame_len, request));
  assert(deye_modbus_response(p+11, frame_len-13, 1, 3, 169, 125, out, sizeof(out), &exception) == DEYE_RTU_OK);
  for (size_t n = 0; n < frame_len; ++n) assert(!deye_v5_reply_matches(p, n, request));
  p[5]++; checksum(p, frame_len); assert(!deye_v5_reply_matches(p, frame_len, request)); p[5]--;
  p[7]++; checksum(p, frame_len); assert(!deye_v5_reply_matches(p, frame_len, request)); p[7]--;
  checksum(p, frame_len); p[frame_len-2] ^= 1; assert(!deye_v5_reply_matches(p, frame_len, request));
  puts("PASS: ranges, 131072 masked updates, FC16 golden frame, FC03 1/92/125 registers,");
  puts("      truncated/corrupt/wrong responses, exceptions, V5 identity and frames >256 bytes.");
  return 0;
}
