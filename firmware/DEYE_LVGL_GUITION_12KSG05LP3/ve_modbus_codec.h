#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// Fonctions pures partagees avec les tests sur ordinateur.
static inline uint16_t modbus_crc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; ++bit)
      crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : crc >> 1;
  }
  return crc;
}

static inline bool deye_modbus_range_valid(uint16_t start, uint16_t count) {
  return count > 0 && count <= 125 && (uint32_t)start + count <= 65536UL;
}

static inline bool deye_modbus_contains(uint16_t start, uint16_t count, uint16_t reg) {
  return deye_modbus_range_valid(start, count) && reg >= start &&
    (uint32_t)reg < (uint32_t)start + count;
}

static inline uint16_t deye_ev_replace_mode(uint16_t original, uint8_t mode) {
  return (original & ~(uint16_t)0x0003) | (mode & 0x03);
}

static inline void deye_modbus_write_one(uint8_t slave, uint16_t reg, uint16_t value, uint8_t out[11]) {
  out[0] = slave; out[1] = 0x10;
  out[2] = reg >> 8; out[3] = reg & 0xFF;
  out[4] = 0; out[5] = 1; out[6] = 2;
  out[7] = value >> 8; out[8] = value & 0xFF;
  const uint16_t crc = modbus_crc16(out, 9);
  out[9] = crc & 0xFF; out[10] = crc >> 8;
}

typedef enum { DEYE_RTU_IGNORE, DEYE_RTU_OK, DEYE_RTU_EXCEPTION } DeyeRtuResult;

static inline bool deye_v5_reply_matches(const uint8_t *frame, size_t length, const uint8_t *request) {
  if (length < 14 || frame[0] != 0xA5 || frame[length-1] != 0x15 ||
      (size_t)(frame[1] | ((uint16_t)frame[2] << 8)) + 13 != length ||
      frame[3] != 0x10 || frame[4] != 0x15 || frame[5] != request[5] ||
      memcmp(frame + 7, request + 7, 4) != 0) return false;
  uint8_t sum = 0;
  for (size_t i = 1; i < length - 2; ++i) sum += frame[i];
  return frame[length-2] == sum;
}

// Le cadre V5 est valide avant cet appel. Le RTU peut avoir un decalage variable.
static inline DeyeRtuResult deye_modbus_response(
  const uint8_t *payload, size_t length, uint8_t slave, uint8_t function,
  uint16_t reg, uint16_t count, uint8_t *out, size_t capacity, uint8_t *exception
) {
  if (!deye_modbus_range_valid(reg, count)) return DEYE_RTU_IGNORE;
  for (size_t i = 0; i + 5 <= length; ++i) {
    const uint8_t *p = payload + i;
    if (p[0] != slave) continue;
    const bool error = p[1] == (function | 0x80);
    if (!error && p[1] != function) continue;
    const size_t n = error ? 5 : (function == 0x03 ? 5 + (size_t)count * 2 : 8);
    if (i + n > length) continue;
    if (modbus_crc16(p, n - 2) != ((uint16_t)p[n-2] | ((uint16_t)p[n-1] << 8))) continue;
    if (error) { if (exception) *exception = p[2]; return DEYE_RTU_EXCEPTION; }
    if (function == 0x03) {
      if (p[2] != count * 2) continue;
    } else if (function == 0x10) {
      if (p[2] != (reg >> 8) || p[3] != (reg & 0xFF) || p[4] != (count >> 8) || p[5] != (count & 0xFF)) continue;
    } else continue;
    if (n > capacity) return DEYE_RTU_IGNORE;
    memcpy(out, p, n);
    return DEYE_RTU_OK;
  }
  return DEYE_RTU_IGNORE;
}
