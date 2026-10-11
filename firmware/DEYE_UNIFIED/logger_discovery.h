#pragma once
#include <WiFiUdp.h>
#include "logger_discovery_logic.h"

// WIFIKIT protocol restored from auto_logger2. Only the Solarman reader
// performs network transactions, so discovery cannot race normal V5 reads.
static constexpr char LOGGER_DISCOVERY_REQUEST[] = "WIFIKIT-214028-READ";
static constexpr uint16_t LOGGER_DISCOVERY_PORT = 48899;
static portMUX_TYPE logger_discovery_mux = portMUX_INITIALIZER_UNLOCKED;
static DeyeLoggerDiscoveryStatus logger_discovery_status;
static uint32_t logger_discovery_probe_serial = 0;
static uint32_t logger_discovery_expected_serial = 0;
static uint16_t logger_discovery_udp_received = 0, logger_discovery_udp_rejected = 0;
static uint16_t logger_discovery_udp_sent = 0, logger_discovery_tcp_open = 0;
static constexpr uint32_t LOGGER_TCP_CONNECT_MS = 500;
static constexpr uint32_t LOGGER_TCP_RESPONSE_MS = 10000;

static bool deye_logger_discovery_copy_status(DeyeLoggerDiscoveryStatus *out) {
  if (!out) return false;
  portENTER_CRITICAL(&logger_discovery_mux);
  *out = logger_discovery_status;
  portEXIT_CRITICAL(&logger_discovery_mux);
  return true;
}
static void logger_discovery_set_status(DeyeLoggerDiscoveryState state, uint16_t scanned,
                                       uint16_t total, uint32_t serial = 0,
                                       const char *host = "", const char *mac = "") {
  portENTER_CRITICAL(&logger_discovery_mux);
  logger_discovery_status.state = state;
  logger_discovery_status.scanned_hosts = scanned;
  logger_discovery_status.total_hosts = total;
  logger_discovery_status.serial = serial;
  snprintf(logger_discovery_status.host, sizeof(logger_discovery_status.host), "%s", host);
  snprintf(logger_discovery_status.mac, sizeof(logger_discovery_status.mac), "%s", mac);
  portEXIT_CRITICAL(&logger_discovery_mux);
}
static void logger_discovery_set_phase(DeyeLoggerDiscoveryPhase phase) {
  portENTER_CRITICAL(&logger_discovery_mux);
  logger_discovery_status.phase = phase;
  portEXIT_CRITICAL(&logger_discovery_mux);
  DBG.printf("Recherche logger : %s\n", logger_discovery_phase_name(phase));
}
static bool deye_logger_discovery_start(uint32_t serial, bool automatic = false) {
  if (!reader_running || WiFi.status() != WL_CONNECTED) return false;
  portENTER_CRITICAL(&logger_discovery_mux);
  if (logger_discovery_busy(logger_discovery_status.state)) {
    portEXIT_CRITICAL(&logger_discovery_mux);
    return false;
  }
  const uint32_t next = logger_discovery_status.generation + 1;
  logger_discovery_status = {};
  logger_discovery_status.generation = next ? next : 1;
  logger_discovery_status.state = DEYE_LOGGER_DISCOVERY_QUEUED;
  logger_discovery_status.automatic = automatic;
  // A placeholder is not a usable credential for TCP discovery.
  logger_discovery_probe_serial = serial == 123456789UL ? 0 : serial;
  portEXIT_CRITICAL(&logger_discovery_mux);
  return true;
}
static uint32_t logger_discovery_ip_value(const IPAddress &ip) {
  return (uint32_t(ip[0]) << 24) | (uint32_t(ip[1]) << 16) | (uint32_t(ip[2]) << 8) | ip[3];
}
static IPAddress logger_discovery_ip(uint32_t ip) {
  return IPAddress(ip >> 24, (ip >> 16) & 255, (ip >> 8) & 255, ip & 255);
}
static bool logger_discovery_send(WiFiUDP &udp, const IPAddress &ip) {
  const bool ok = udp.beginPacket(ip, LOGGER_DISCOVERY_PORT) &&
    udp.write(reinterpret_cast<const uint8_t *>(LOGGER_DISCOVERY_REQUEST), sizeof(LOGGER_DISCOVERY_REQUEST) - 1) ==
      sizeof(LOGGER_DISCOVERY_REQUEST) - 1 && udp.endPacket();
  if (ok) ++logger_discovery_udp_sent;
  else DBG.printf("Recherche logger : envoi UDP impossible vers %s\n", ip.toString().c_str());
  return ok;
}
static bool logger_discovery_wait(WiFiUDP &udp, uint32_t timeout,
                                  uint32_t local, uint32_t mask, uint16_t scanned, uint16_t total) {
  const uint32_t started = millis();
  while (WiFi.status() == WL_CONNECTED && uint32_t(millis() - started) < timeout) {
    const int packet_size = udp.parsePacket();
    if (packet_size > 0) {
      ++logger_discovery_udp_received;
      char reply[96] = {}, mac[18] = {};
      const IPAddress source = udp.remoteIP();
      const uint32_t value = logger_discovery_ip_value(source);
      const int count = udp.read(reinterpret_cast<uint8_t *>(reply), sizeof(reply) - 1);
      // WiFiUDP::parsePacket refuses the next datagram while unread bytes remain.
      // Always discard the tail, including for oversized or malformed replies.
      udp.flush();
      const int text_size = count > 0 && reply[count - 1] == '\0' ? count - 1 : count;
      uint32_t serial = 0;
      const String host = source.toString();
      if (packet_size < int(sizeof(reply)) && count == packet_size &&
          text_size > 0 && !memchr(reply, '\0', text_size) && value != local && value &&
          (value & mask) == (local & mask) &&
          logger_discovery_parse_reply(reply, host.c_str(), &serial, mac, sizeof(mac)) &&
          (!logger_discovery_expected_serial || serial == logger_discovery_expected_serial)) {
        logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_FOUND, scanned, total, serial, host.c_str(), mac);
        DBG.printf("Logger trouve par UDP : %s SN %lu\n", host.c_str(), (unsigned long)serial);
        return true;
      }
      ++logger_discovery_udp_rejected;
      DBG.printf("Recherche logger : reponse UDP rejetee de %s (%d octets)\n", host.c_str(), packet_size);
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
  return false;
}
static bool logger_discovery_probe_tcp_v5(const IPAddress &candidate, uint32_t serial) {
  if (!serial || WiFi.status() != WL_CONNECTED) return false;
  uint8_t rtu[8], request[64], frame[320];
  // Use the configured battery SOC register already read by the dashboard.
  build_modbus_rtu_read(REG_BATTERY_SOC, 1, rtu); // FC03 only; no inverter write.
  const size_t length = build_solarman_v5_request(rtu, sizeof(rtu), request, serial);
  WiFiClient client;
  if (!client.connect(candidate, DEYE_PORT, LOGGER_TCP_CONNECT_MS)) {
    client.stop(); return false;
  }
  ++logger_discovery_tcp_open;
  DBG.printf("Recherche logger : TCP ouvert sur %s, attente V5 SN %lu\n",
    candidate.toString().c_str(), (unsigned long)serial);
  if (client.write(request, length) != length) { client.stop(); return false; }
  const uint32_t started = millis();
  while (WiFi.status() == WL_CONNECTED && uint32_t(millis() - started) < LOGGER_TCP_RESPONSE_MS) {
    if (!client.available()) { delay(2); continue; }
    size_t frame_len = 0;
    const uint32_t elapsed = millis() - started;
    if (elapsed >= LOGGER_TCP_RESPONSE_MS) break;
    const uint32_t remaining = LOGGER_TCP_RESPONSE_MS - elapsed;
    if (!receive_one_v5_frame(client, frame, &frame_len, sizeof(frame), remaining)) break;
    if (deye_v5_reply_matches(frame, frame_len, request)) {
      // A valid V5 envelope with the requested serial and sequence identifies
      // the logger even when its inverter cannot supply a valid Modbus payload.
      client.stop(); return true;
    }
    DBG.printf("Recherche logger : trame V5 rejetee sur %s\n", candidate.toString().c_str());
  }
  DBG.printf("Recherche logger : aucune reponse V5 correspondante sur %s\n", candidate.toString().c_str());
  client.stop(); return false;
}
static void logger_discovery_run(uint32_t serial) {
  logger_discovery_expected_serial = serial;
  logger_discovery_udp_received = logger_discovery_udp_rejected = 0;
  logger_discovery_udp_sent = logger_discovery_tcp_open = 0;
  const uint32_t local = logger_discovery_ip_value(WiFi.localIP());
  const uint32_t mask = logger_discovery_ip_value(WiFi.subnetMask());
  WiFiUDP udp;
  if (WiFi.status() != WL_CONNECTED || !local || !mask || !udp.begin(0)) {
    logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_ERROR, 0, 0); return;
  }
  const auto subnet = logger_discovery_subnet(local, mask);
  const uint16_t total = 4 + subnet.total_hosts * (serial ? 2 : 1);
  uint16_t scanned = 0;
  DBG.printf("Recherche logger : IP %s masque %s SN %lu, %u sondes\n",
    WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str(), (unsigned long)serial, total);
  logger_discovery_set_phase(DEYE_LOGGER_PHASE_BROADCAST);
  // Preserve the global broadcast from auto_logger2 and try directed broadcast.
  for (uint8_t attempt = 0; attempt < 4 && WiFi.status() == WL_CONNECTED; ++attempt) {
    ++scanned;
    logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_RUNNING, scanned, total);
    logger_discovery_send(udp, attempt % 2 ? logger_discovery_ip(local | ~mask) : IPAddress(255,255,255,255));
    if (logger_discovery_wait(udp, 1000, local, mask, scanned, total)) { udp.stop(); return; }
  }
  // Try V5 immediately after broadcast when a serial is supplied. Do not make
  // the user wait for a full UDP scan when the logger ignores WIFIKIT.
  if (serial) logger_discovery_set_phase(DEYE_LOGGER_PHASE_TCP);
  for (uint16_t i = 1; serial && i <= subnet.last_host && WiFi.status() == WL_CONNECTED; ++i) {
    const uint32_t address = subnet.network + i;
    if (address == local) continue;
    ++scanned;
    logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_RUNNING, scanned, total);
    const auto candidate = logger_discovery_ip(address);
    if (logger_discovery_probe_tcp_v5(candidate, serial)) {
      const String host = candidate.toString();
      logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_FOUND, scanned, total, serial, host.c_str());
      DBG.printf("Logger trouve par TCP/V5 : %s SN %lu\n", host.c_str(), (unsigned long)serial);
      udp.stop(); return;
    }
    // Also collect delayed broadcast replies while walking the TCP candidates.
    if (logger_discovery_wait(udp, 5, local, mask, scanned, total)) { udp.stop(); return; }
  }
  logger_discovery_set_phase(DEYE_LOGGER_PHASE_UNICAST);
  // Unicast recovers IP and serial when broadcast is filtered.
  for (uint16_t i = 1; i <= subnet.last_host && WiFi.status() == WL_CONNECTED; ++i) {
    const uint32_t address = subnet.network + i;
    if (address == local) continue;
    ++scanned;
    logger_discovery_set_status(DEYE_LOGGER_DISCOVERY_RUNNING, scanned, total);
    logger_discovery_send(udp, logger_discovery_ip(address));
    if (logger_discovery_wait(udp, 250, local, mask, scanned, total)) { udp.stop(); return; }
  }
  if (logger_discovery_wait(udp, 1000, local, mask, scanned, total)) { udp.stop(); return; }
  udp.stop();
  DBG.printf("Recherche logger terminee : UDP envoyes %u, recus %u, rejetes %u ; TCP ouverts %u\n",
    logger_discovery_udp_sent, logger_discovery_udp_received, logger_discovery_udp_rejected, logger_discovery_tcp_open);
  logger_discovery_set_status(WiFi.status() == WL_CONNECTED ? DEYE_LOGGER_DISCOVERY_NOT_FOUND :
    DEYE_LOGGER_DISCOVERY_ERROR, scanned, total);
}
static bool deye_logger_discovery_process() {
  portENTER_CRITICAL(&logger_discovery_mux);
  const bool queued = logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_QUEUED;
  const uint32_t serial = logger_discovery_probe_serial;
  if (queued) logger_discovery_status.state = DEYE_LOGGER_DISCOVERY_RUNNING;
  portEXIT_CRITICAL(&logger_discovery_mux);
  if (queued) logger_discovery_run(serial);
  return queued;
}
