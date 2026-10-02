#pragma once
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
using String = std::string;
class Preferences {
  std::string space;
  bool read_only = true;
public:
  static inline std::map<std::string, std::map<std::string,std::vector<uint8_t>>> records;
  static inline bool fail_write = false;
  bool begin(const char *name, bool ro) { space=name; read_only=ro; return !ro || records.count(space); }
  void end() {}
  size_t getBytesLength(const char *key) { return records[space][key].size(); }
  size_t getBytes(const char *key, void *out, size_t size) {
    const auto &v=records[space][key]; if (v.size()>size || v.empty()) return 0;
    memcpy(out,v.data(),v.size()); return v.size();
  }
  size_t putBytes(const char *key, const void *data, size_t size) {
    if (fail_write || read_only) return 0;
    const auto *bytes=static_cast<const uint8_t *>(data);
    records[space][key]={bytes,bytes+size}; return size;
  }
  size_t putString(const char *key, const char *text) { return putBytes(key,text,strlen(text)); }
  String getString(const char *key, const char *fallback) {
    const auto &v=records[space][key]; return v.empty() ? fallback : String(v.begin(),v.end());
  }
  uint16_t getUShort(const char *, uint16_t fallback) { return fallback; }
  uint32_t getUInt(const char *, uint32_t fallback) { return fallback; }
  float getFloat(const char *, float fallback) { return fallback; }
};
