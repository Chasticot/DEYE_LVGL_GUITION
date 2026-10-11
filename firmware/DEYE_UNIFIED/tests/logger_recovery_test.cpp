#include <atomic>
#define main logger_discovery_suite
#include "logger_service_test.cpp"
#undef main
#include "../network_config.h"

static String cfg_deye_host;
class Preferences {
public:
  static inline String host;
  static inline uint32_t serial;
  static inline unsigned host_writes = 0;
  static inline bool fail_write = false, fail_open = false;
  bool begin(const char *name, bool readonly) { assert(String(name) == "deye-ui" && !readonly); return !fail_open; }
  uint32_t getUInt(const char *key, uint32_t) { assert(String(key) == "logger"); return serial; }
  String getString(const char *key, const char *) { assert(String(key) == "deye_host"); return host; }
  size_t putString(const char *key, const String &value) {
    assert(String(key) == "deye_host"); ++host_writes;
    if (fail_write) return 0;
    host = value; return value.length();
  }
  void end() {}
};
// No putUInt in this fake: the exact production saver must never write the SN.
#include "logger_settings_under_test.h"
#include "../logger_recovery.h"

static void prepare() {
  reset(2);
  cfg_logger_serial = 2712345678UL;
  cfg_deye_host = Preferences::host = "192.168.1.99";
  Preferences::serial = cfg_logger_serial;
  Preferences::host_writes = 0;
  Preferences::fail_open = Preferences::fail_write = false;
  logger_recovery_apply_pending = false;
  logger_recovery_paused.store(false);
  logger_recovery_waiting_for_apply.store(false);
  logger_recovery_policy.reset(0);
  for (int i=0; i<3; ++i) logger_recovery_policy.observe(10000*i, false);
  now_ms = 60000;
}
static void find() {
  assert(deye_logger_recovery_maybe_start());
  assert(logger_discovery_status.automatic);
  assert(deye_logger_discovery_process());
  deye_logger_recovery_scan_finished(millis());
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND);
}
int main() {
  LoggerRecoveryPolicy policy;
  policy.reset(0);
  assert(!policy.due(60000, cfg_logger_serial, true, false, false));
  for (int i=0; i<3; ++i) policy.observe(i*10000, false);
  assert(!policy.due(59999, cfg_logger_serial, true, false, false));
  assert(policy.due(60000, cfg_logger_serial, true, false, false));
  for (uint32_t serial : {0UL,123456789UL}) assert(!policy.due(60000,serial,true,false,false));
  assert(!policy.due(60000,cfg_logger_serial,false,false,false));
  assert(!policy.due(60000,cfg_logger_serial,true,true,false));
  assert(!policy.due(60000,cfg_logger_serial,true,false,true));
  policy.observe(60000, true); // Valid V5, including a Modbus exception, resets loss.
  assert(!policy.due(120000,cfg_logger_serial,true,false,false));
  for (int i=0;i<3;++i) policy.observe(70000+i*10000,false);
  policy.attempt_finished(120000);
  assert(!policy.due(419999,cfg_logger_serial,true,false,false));
  assert(policy.due(420000,cfg_logger_serial,true,false,false));
  policy.wifi_unavailable(420000);
  assert(!policy.due(480000,cfg_logger_serial,true,false,false));
  policy.reset(UINT32_MAX-30000);
  for (int i=0;i<3;++i) policy.observe(0,false);
  assert(!policy.due(29998,cfg_logger_serial,true,false,false));
  assert(policy.due(29999,cfg_logger_serial,true,false,false));

  prepare(); find();
  assert(deye_logger_recovery_apply());
  assert(cfg_deye_host == "192.168.1.2" && Preferences::host == cfg_deye_host);
  assert(cfg_logger_serial == 2712345678UL && Preferences::serial == cfg_logger_serial);
  assert(Preferences::host_writes == 1);
  assert(logger_discovery_status.restart_pending && !deye_logger_recovery_apply());
  assert(logger_recovery_waiting_for_apply.load());
  assert(!deye_logger_recovery_maybe_start());
  prepare(); cfg_deye_host = Preferences::host = "192.168.1.2"; find();
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); Preferences::fail_write = true;
  assert(!deye_logger_recovery_apply() && logger_discovery_status.save_failed && !logger_discovery_status.restart_pending);
  assert(!logger_recovery_waiting_for_apply.load());
  assert(cfg_deye_host == "192.168.1.99" && Preferences::host == cfg_deye_host);
  prepare(); find(); Preferences::fail_open = true;
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); cfg_logger_serial++;
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); cfg_deye_host = "192.168.1.44";
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); Preferences::serial++;
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); Preferences::host = "192.168.1.55";
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); logger_discovery_status.serial++;
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); find(); wifi_status = 0;
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); logger_recovery_paused.store(true);
  assert(!deye_logger_recovery_maybe_start());
  prepare(); scenario=3;
  assert(deye_logger_recovery_maybe_start());
  deye_logger_discovery_process();deye_logger_recovery_scan_finished(millis());
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_NOT_FOUND);
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes && !logger_recovery_waiting_for_apply.load());
  assert(!deye_logger_recovery_maybe_start());
  now_ms += LOGGER_RECOVERY_RETRY_MS;
  assert(deye_logger_recovery_maybe_start());
  prepare(); find(); assert(deye_logger_discovery_start(cfg_logger_serial));
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  prepare(); assert(deye_logger_discovery_start(cfg_logger_serial));
  deye_logger_discovery_process(); deye_logger_recovery_scan_finished(millis());
  assert(!deye_logger_recovery_apply() && !Preferences::host_writes);
  puts("Automatic DHCP recovery: loss/cooldown/wraparound, exact SN, verified IP-only persistence, edit races, OTA pause and no reboot on failure passed.");
}
