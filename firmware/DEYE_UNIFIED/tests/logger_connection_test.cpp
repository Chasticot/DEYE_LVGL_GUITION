#include <assert.h>
#include <stdio.h>
#include <string>
#include <map>
#include "../logger_connection.h"
struct String : std::string {
  using std::string::string;
  String(const std::string &s) : std::string(s) {}
  bool isEmpty() const { return empty(); }
};
static constexpr uint32_t DEFAULT_LOGGER_SERIAL = 2712345678UL;
static String cfg_deye_host = "old";
static uint32_t cfg_logger_serial = DEFAULT_LOGGER_SERIAL;
static uint16_t cfg_deye_port = 8899;
static uint8_t cfg_deye_unit = 1;
static DeyeTransport cfg_deye_transport = DeyeTransport::SolarmanV5;
static LoggerModel cfg_logger_model = LoggerModel::Lsw;
static String cfg_logger_identity;
static struct Preferences {
  std::map<std::string,uint32_t> values;
  std::map<std::string,String> strings;
  bool fail = false;
  bool begin(const char *,bool) { return true; }
  void end() {}
  size_t putString(const char *key,const String &v) { if(fail) return 0; strings[key]=v; return v.size(); }
  String getString(const char *key,const char *fallback) { return strings.count(key) ? strings[key] : String(fallback); }
  size_t putUInt(const char *key,uint32_t v) { if(fail) return 0; values[key]=v; return 4; }
  size_t putUShort(const char *key,uint16_t v) { if(fail) return 0; values[key]=v; return 2; }
  size_t putUChar(const char *key,uint8_t v) { if(fail) return 0; values[key]=v; return 1; }
  uint32_t getUInt(const char *key,uint32_t fallback) { return values.count(key) ? values[key] : fallback; }
  uint16_t getUShort(const char *key,uint16_t fallback) { return getUInt(key,fallback); }
  uint8_t getUChar(const char *key,uint8_t fallback) { return getUInt(key,fallback); }
  bool getBool(const char *key,bool fallback) { return getUInt(key,fallback); }
} preferences;
#include "connection_settings_under_test.h"
int main() {
  load_connection(); assert(cfg_deye_transport == DeyeTransport::SolarmanV5 && cfg_deye_port == 8899 && cfg_deye_unit == 1);
  preferences.values["deye_mode_lse"] = 1; preferences.values["deye_port"] = 1502;
  load_connection(); assert(cfg_deye_transport == DeyeTransport::ModbusTcp && cfg_deye_port == 1502);
  assert(settings_save_deye("192.168.1.50",0,DeyeTransport::ModbusTcp,502,7));
  cfg_deye_transport = DeyeTransport::SolarmanV5; cfg_deye_port = 8899; cfg_deye_unit = 1;
  load_connection(); assert(cfg_deye_transport == DeyeTransport::ModbusTcp && cfg_deye_port == 502 && cfg_deye_unit == 7 && cfg_logger_serial == 0);
  assert(settings_save_deye("192.168.1.50",2101769527UL,DeyeTransport::ModbusTcp,8899,1));
  cfg_logger_serial = 0;
  load_connection(); assert(cfg_logger_serial == 2101769527UL && cfg_deye_transport == DeyeTransport::ModbusTcp && cfg_deye_port == 8899 && cfg_deye_unit == 1);
  assert(!settings_save_deye("192.168.1.50",0,DeyeTransport::SolarmanV5,8899,1));
  assert(!settings_save_deye("192.168.1.50",0,DeyeTransport::ModbusTcp,0,1));
  assert(!settings_save_logger("192.168.1.50","",LoggerModel::Lse,8899,1));
  assert(!settings_save_logger("192.168.1.50","0",LoggerModel::Lse,8899,1));
  assert(!settings_save_logger("192.168.1.50","D123456789012",LoggerModel::Lse,8899,1));
  assert(settings_save_logger("192.168.1.50","d123456789012",LoggerModel::Dl1000B,502,1));
  cfg_logger_identity="changed";cfg_logger_model=LoggerModel::Lsw;
  load_connection();assert(cfg_logger_identity=="D123456789012" && cfg_logger_serial==0 && cfg_logger_model==LoggerModel::Dl1000B);
  assert(logger_default_port(LoggerModel::Lsw)==8899 && logger_default_port(LoggerModel::Lse)==8899 && logger_default_port(LoggerModel::Dl1000B)==502);
  preferences.fail = true;
  assert(!settings_save_deye("192.168.1.99",123,DeyeTransport::SolarmanV5,8899,1));
  assert(cfg_deye_host == "192.168.1.50" && cfg_deye_transport == DeyeTransport::ModbusTcp);
  puts("Logger settings: old/default/LSE migration, TCP without serial, persisted port/unit and failed saves passed");
}
