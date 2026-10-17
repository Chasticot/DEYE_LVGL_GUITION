#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include "../ve_modbus_codec.h"
#include "../logger_connection.h"
#include "../modbus_tcp_protocol.h"
static DeyeTransport cfg_deye_transport = DeyeTransport::SolarmanV5;
using String = std::string;
static uint32_t now_ms, cfg_logger_serial = 2712345678UL;
static uint32_t FRAME_TIMEOUT_MS = 7000;
static constexpr uint16_t DEYE_PORT = 8899, DEYE_MODBUS_SLAVE_ID = 1;
static uint16_t REG_BATTERY_SOC = 184;
static constexpr int WL_CONNECTED = 3;
static int scenario, wifi_status = WL_CONNECTED;
static bool reader_running = true;
static unsigned sends, tcp_probes;
static uint32_t millis() { return now_ms; }
static void delay(uint32_t ms) { now_ms += ms; if (scenario == 6 && now_ms > 20) wifi_status = 0; }
static void vTaskDelay(uint32_t ms) { delay(ms); }
#define pdMS_TO_TICKS(ms) (ms)
#define portMUX_INITIALIZER_UNLOCKED 0
using portMUX_TYPE = int;
static void portENTER_CRITICAL(int *) {} static void portEXIT_CRITICAL(int *) {}
static struct { template<class... T> void printf(const char *, T...) {} } DBG;
struct IPAddress {
  uint8_t bytes[4];
  IPAddress(uint8_t a=0, uint8_t b=0, uint8_t c=0, uint8_t d=0) : bytes{a,b,c,d} {}
  uint8_t operator[](int i) const { return bytes[i]; }
  String toString() const { char text[16]; snprintf(text,sizeof(text),"%u.%u.%u.%u",bytes[0],bytes[1],bytes[2],bytes[3]); return text; }
};
static struct {
  int status() const { return wifi_status; }
  IPAddress localIP() const { return {192,168,1,1}; }
  IPAddress subnetMask() const { return {255,255,255,252}; } // Only one peer, .2.
} WiFi;
class WiFiUDP {
  IPAddress target;
  String pending;
  std::deque<String> datagrams;
public:
  bool begin(int) { return true; }
  void stop() {}
  bool beginPacket(IPAddress ip, int port) { assert(port == 48899); target = ip; return true; }
  size_t write(const uint8_t *request, size_t size) {
    assert(String(reinterpret_cast<const char *>(request),size) == "WIFIKIT-214028-READ"); return size;
  }
  bool endPacket() {
    ++sends;
    const bool broadcast = target[3] == 255 || target[3] == 3;
    if ((scenario == 0 && broadcast) || (scenario == 1 && target[3] == 2))
      datagrams.push_back("192.168.1.2,AABBCCDDEEFF,2712345678\r\n");
    if (scenario == 3) datagrams.push_back("192.168.1.99,AABBCCDDEEFF,2712345678");
    if (scenario == 7 && broadcast) { String reply = "192.168.1.2,AABBCCDDEEFF,2712345678"; reply.push_back('\0'); datagrams.push_back(reply); }
    if (scenario == 8 && broadcast) {
      datagrams.push_back(String(200, 'X'));
      datagrams.push_back("192.168.1.2,AABBCCDDEEFF,2712345678");
    }
    if (scenario == 11) datagrams.push_back("192.168.1.2,AABBCCDDEEFF,2712345679");
    if (scenario == 16) datagrams.push_back("192.168.1.2,AABBCCDDEEFF,D123456789012\r\n");
    if (scenario == 17) datagrams.push_back("192.168.1.2,AABBCCDDEEFF,2712345679");
    return true;
  }
  int parsePacket() {
    if (!pending.empty() || datagrams.empty()) return 0; // Match the ESP32 WiFiUDP behavior.
    pending=datagrams.front(); datagrams.pop_front(); return int(pending.size());
  }
  IPAddress remoteIP() { return {192,168,1,2}; }
  int read(uint8_t *out,size_t capacity) { size_t n=std::min(capacity,pending.size()); memcpy(out,pending.data(),n); pending.erase(0,n); return int(n); }
  void flush() { pending.clear(); }
};
class WiFiClient {
public:
  std::vector<uint8_t> received;
  size_t position = 0;
  uint32_t ready_at = 0;
  bool connect(IPAddress ip, int port, int timeout) {
    ++tcp_probes; assert(ip[3] == 2 && (port == 8899 || port == 502) && timeout == 500);
    return scenario == 2 || scenario == 4 || scenario == 5 || scenario == 9 || scenario == 10 || scenario == 12 || scenario == 13 || scenario >= 14;
  }
  size_t write(const uint8_t *request, size_t size) {
    if (size==12) {
      assert(request[7]==3 && request[10]==0 && request[11]==1);
      assert(((uint16_t(request[8])<<8)|request[9])==REG_BATTERY_SOC);
      received={request[0],request[1],0,0,0,5,request[6],3,2,0,72};
      if (scenario==15) received={request[0],request[1],0,0,0,3,request[6],0x83,2};
      if (scenario==18) received[1]^=1;
      if (scenario==19) received[6]^=1;
      if (scenario==20) received.clear();
      return size;
    }
    assert(size == 36 && request[27] == 0x03); // All probes are FC03.
    assert(((uint16_t(request[28]) << 8) | request[29]) == REG_BATTERY_SOC);
    assert(sends == 4); // TCP starts after broadcast, before full UDP unicast.
    ready_at = now_ms + (scenario == 9 ? 2000 : 0);
    received.assign(35,0);
    received[0]=0xA5; received[1]=22; received[3]=0x10; received[4]=0x15;
    received[5]=request[5]; memcpy(received.data()+7,request+7,4);
    uint8_t rtu[7]={1,3,2,0,1,0,0};
    const uint16_t crc=modbus_crc16(rtu,5); rtu[5]=crc & 255; rtu[6]=crc >> 8;
    memcpy(received.data()+26,rtu,7);
    if (scenario == 10) received[30] ^= 1; // Valid V5 identity, invalid inverter RTU CRC.
    if (scenario == 5) received[7] ^= 1;
    if (scenario == 13) received[5] ^= 1; // Wrong V5 transaction sequence.
    uint8_t sum=0; for (size_t i=1;i<33;++i) sum+=received[i];
    received[33]=scenario == 4 ? uint8_t(sum ^ 1) : sum; received[34]=0x15;
    if (scenario == 12) received.clear(); // Open port with no response.
    return size;
  }
  int available() { return now_ms < ready_at ? 0 : int(received.size()-position); }
  uint8_t read() { return received[position++]; }
  void stop() {}
};
// Extract the exact production V5 builder and bounded reader at test time.
#include "logger_protocol_under_test.h"
#include "../logger_discovery.h"
static void reset(int selected) {
  scenario=selected; now_ms=0; sends=0; tcp_probes=0; wifi_status=WL_CONNECTED;
  logger_discovery_status={};
}
int main() {
  reset(0);
  assert(!deye_logger_discovery_start(0));
  assert(deye_logger_discovery_start(cfg_logger_serial));
  assert(!deye_logger_discovery_start(cfg_logger_serial)); // Prevent duplicate starts.
  assert(deye_logger_discovery_process());
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && sends == 1 && tcp_probes == 0);
  assert(String(logger_discovery_status.host) == "192.168.1.2" && logger_discovery_status.serial == 2712345678UL);
  assert(!deye_logger_discovery_process());
  const auto generation=logger_discovery_status.generation;
  assert(deye_logger_discovery_start(cfg_logger_serial) && logger_discovery_status.generation == generation+1);
  reset(1); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && sends == 5);
  reset(7); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && sends == 1);
  reset(8); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && sends == 1);
  assert(logger_discovery_udp_received == 2 && logger_discovery_udp_rejected == 1);
  reset(2); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && tcp_probes == 1 && sends == 4);
  assert(logger_discovery_status.phase == DEYE_LOGGER_PHASE_TCP);
  for (int valid : {9,10}) {
    reset(valid); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
    assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND && sends == 4);
    if (valid == 9) assert(now_ms >= 6000); // A 2s logger response survives the former 900ms cutoff.
  }
  for (int invalid : {3,4,5,11,12,13}) {
    reset(invalid); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
    assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_NOT_FOUND);
    assert(logger_discovery_status.scanned_hosts == logger_discovery_status.total_hosts);
    if (invalid == 12) assert(now_ms >= 15000 && now_ms < 16000);
  }
  reset(2); assert(!deye_logger_discovery_start(123456789UL));
  reset(6); assert(deye_logger_discovery_start(cfg_logger_serial)); deye_logger_discovery_process();
  assert(logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_ERROR && now_ms < 100);
  assert(!deye_logger_discovery_start(0));
  // An oversized frame must never overwrite the caller's buffer.
  WiFiClient client; client.received.assign(11,0); client.received[0]=0xA5; client.received[1]=200;
  uint8_t guarded[32]; memset(guarded,0xCC,sizeof(guarded)); size_t len=0;
  assert(!receive_one_v5_frame(client,guarded,&len,16,900));
  for (auto value : guarded) assert(value == 0xCC);
  // Partial headers and bodies honor the probe's deadline, not the 7s normal timeout.
  client.received={0xA5}; client.position=0; now_ms=0;
  assert(!receive_one_v5_frame(client,guarded,&len,sizeof(guarded),30) && now_ms <= 30);
  client.received.assign(11,0); client.received[0]=0xA5; client.received[1]=1; client.position=0; now_ms=0;
  assert(!receive_one_v5_frame(client,guarded,&len,sizeof(guarded),30) && now_ms <= 30);
  puts("Discovery service: oversized UDP recovery, TCP priority, delayed V5, logger identity, rejected SN/sequence/checksum, deadlines and Wi-Fi loss passed.");
  for (LoggerModel model : {LoggerModel::Lse,LoggerModel::Dl1000B}) {
    reset(model==LoggerModel::Lse ? 0 : 16);
    const char *sn=model==LoggerModel::Lse ? "2712345678" : "D123456789012";
    assert(deye_logger_discovery_start(sn,model,logger_default_port(model),1));deye_logger_discovery_process();
    assert(logger_discovery_status.state==DEYE_LOGGER_DISCOVERY_FOUND && !strcmp(logger_discovery_status.identity,sn));
  }
  for (int valid : {14,15}) {
    reset(valid);assert(deye_logger_discovery_start("D123456789012",LoggerModel::Dl1000B,502,2));
    deye_logger_discovery_process();
    assert(logger_discovery_status.state==DEYE_LOGGER_DISCOVERY_CANDIDATES && logger_discovery_status.candidate_count==1);
    assert(logger_discovery_status.host[0]==0 && !strcmp(logger_discovery_status.candidates[0],"192.168.1.2"));
    assert(logger_discovery_status.model==LoggerModel::Dl1000B && logger_discovery_status.port==502 && logger_discovery_status.unit==2);
  }
  for (int invalid : {17,18,19,20}) {
    reset(invalid);assert(deye_logger_discovery_start("2712345678",LoggerModel::Lse,8899,1));deye_logger_discovery_process();
    assert(logger_discovery_status.state==DEYE_LOGGER_DISCOVERY_NOT_FOUND && !logger_discovery_status.candidate_count);
  }
  reset(14);assert(!deye_logger_discovery_start("",LoggerModel::Dl1000B,502,1));
  assert(!deye_logger_discovery_start("D123",LoggerModel::Lse,8899,1));
  assert(!deye_logger_discovery_start("D123",LoggerModel::Dl1000B,0,1));
  assert(!deye_logger_discovery_start("D123",LoggerModel::Dl1000B,502,0));
  assert(!deye_logger_discovery_start("D123",LoggerModel::Dl1000B,502,1,true));
  reset(14);
  for (int i=1;i<=20;++i) { char host[16];snprintf(host,sizeof(host),"192.168.1.%d",i);logger_discovery_add_candidate(host);logger_discovery_add_candidate(host); }
  assert(logger_discovery_status.candidate_count==8 && !strcmp(logger_discovery_status.candidates[7],"192.168.1.8"));
  reset(14);logger_discovery_local_network=0xC0A80100UL;
  memset(logger_discovery_rejected_identity,0,sizeof(logger_discovery_rejected_identity));
  logger_discovery_add_candidate("192.168.1.2",0xC0A80102UL);assert(logger_discovery_status.candidate_count==1);
  logger_discovery_reject_identity(0xC0A80102UL);assert(!logger_discovery_status.candidate_count);
  logger_discovery_add_candidate("192.168.1.2",0xC0A80102UL);assert(!logger_discovery_status.candidate_count);
  puts("Three logger models: mandatory SN, UDP identity, numeric/alphanumeric matches, validated Modbus candidates, mismatch rejection, limits and no implicit selection passed.");
  cfg_deye_transport = DeyeTransport::SolarmanV5;
  return 0;
}
