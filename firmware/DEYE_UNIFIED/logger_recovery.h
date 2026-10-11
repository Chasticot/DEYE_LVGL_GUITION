#pragma once
#include "logger_recovery_logic.h"

static LoggerRecoveryPolicy logger_recovery_policy;
static std::atomic<bool> logger_recovery_paused{false};
static std::atomic<bool> logger_recovery_waiting_for_apply{false};
static char logger_recovery_previous_host[254] = {};
static uint32_t logger_recovery_serial = 0;
static uint32_t logger_recovery_generation = 0;
static bool logger_recovery_apply_pending = false; // Protected by discovery mux.

// Called in the reader task, after processing any queued manual discovery.
static bool deye_logger_recovery_maybe_start() {
  DeyeLoggerDiscoveryStatus status;
  deye_logger_discovery_copy_status(&status);
  if (!logger_recovery_policy.due(millis(), cfg_logger_serial, WiFi.status() == WL_CONNECTED,
      logger_discovery_busy(status.state) || logger_recovery_waiting_for_apply.load(), logger_recovery_paused.load())) return false;
  const uint32_t serial = cfg_logger_serial;
  char previous[sizeof(logger_recovery_previous_host)] = {};
  snprintf(previous, sizeof(previous), "%s", cfg_deye_host.c_str());
  if (!deye_logger_discovery_start(serial, true)) return false;
  deye_logger_discovery_copy_status(&status);
  portENTER_CRITICAL(&logger_discovery_mux);
  memcpy(logger_recovery_previous_host, previous, sizeof(previous));
  logger_recovery_serial = serial;
  logger_recovery_generation = status.generation;
  portEXIT_CRITICAL(&logger_discovery_mux);
  DBG.printf("Connexion Deye perdue : recherche automatique du logger SN %lu, ancienne IP %s\n",
    (unsigned long)serial, previous);
  return true;
}

static void deye_logger_recovery_scan_finished(uint32_t now) {
  portENTER_CRITICAL(&logger_discovery_mux);
  if (logger_discovery_status.automatic && logger_discovery_status.generation == logger_recovery_generation) {
    logger_recovery_policy.attempt_finished(now);
    logger_recovery_apply_pending = logger_discovery_status.state == DEYE_LOGGER_DISCOVERY_FOUND &&
      logger_discovery_status.serial == logger_recovery_serial &&
      strcmp(logger_discovery_status.host, logger_recovery_previous_host) != 0;
    logger_recovery_waiting_for_apply.store(logger_recovery_apply_pending);
  }
  portEXIT_CRITICAL(&logger_discovery_mux);
}

// Called only by the main loop, outside an OTA upload. Settings/UI/Web writes
// are serialized there; the network task never writes Preferences or restarts.
// Return true only after verified persistence, to schedule the normal reboot.
static bool deye_logger_recovery_apply() {
  DeyeLoggerDiscoveryStatus status;
  char previous[sizeof(logger_recovery_previous_host)];
  uint32_t serial;
  portENTER_CRITICAL(&logger_discovery_mux);
  const bool pending = logger_recovery_apply_pending;
  logger_recovery_apply_pending = false;
  status = logger_discovery_status;
  memcpy(previous, logger_recovery_previous_host, sizeof(previous));
  serial = logger_recovery_serial;
  const bool matching = status.generation == logger_recovery_generation;
  portEXIT_CRITICAL(&logger_discovery_mux);
  if (!pending) return false;
  if (!matching || !status.automatic || status.state != DEYE_LOGGER_DISCOVERY_FOUND ||
      !logger_recovery_serial_valid(serial) || status.serial != serial || !status.host[0] ||
      cfg_logger_serial != serial || cfg_deye_host != previous || cfg_deye_host == status.host ||
      WiFi.status() != WL_CONNECTED) {
    logger_recovery_waiting_for_apply.store(false);
    return false;
  }
  const bool saved = settings_save_discovered_deye_host(status.host, previous, serial);
  portENTER_CRITICAL(&logger_discovery_mux);
  logger_discovery_status.restart_pending = saved;
  logger_discovery_status.save_failed = !saved;
  portEXIT_CRITICAL(&logger_discovery_mux);
  // Keep the reader paused after a successful save until the scheduled reboot.
  // This avoids mutating cfg_deye_host while a network read uses its buffer.
  if (!saved) logger_recovery_waiting_for_apply.store(false);
  DBG.printf(saved ? "Nouvelle IP Deye sauvegardee : %s, SN %lu conserve. Redemarrage...\n" :
    "Nouvelle IP Deye %s : sauvegarde impossible, aucun redemarrage (SN %lu).\n",
    status.host, (unsigned long)serial);
  return saved;
}
