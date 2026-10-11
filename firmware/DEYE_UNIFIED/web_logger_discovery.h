#pragma once
static String web_logger_discovery_script() {
  return R"LOGGER(<script>
let loggerGeneration = -1;
let loggerPoll = null;
const loggerForm = document.querySelector("form[action='/deye']");
const loggerStatus = document.getElementById('logger-status');
async function pollLogger() {
  try {
    const r = await fetch('/logger-discovery/status', {cache:'no-store'});
    if (!r.ok) throw new Error('Lecture impossible ('+r.status+').');
    const d = await r.json();
    if (d.state === 1 || d.state === 2) {
      const phase = ['Demarrage', 'UDP broadcast', 'TCP/V5', 'UDP unicast'][d.phase] || 'Demarrage';
      loggerStatus.textContent = 'Recherche '+(d.automatic ? 'auto ' : '')+phase+' : '+d.scanned+' / '+d.total+'. Patientez...';
      if (!loggerPoll) loggerPoll = setInterval(pollLogger, 1000);
    } else {
      clearInterval(loggerPoll); loggerPoll = null;
      if (d.state === 3) {
        if (d.automatic) {
          loggerStatus.textContent = d.restart_pending ? 'Nouvelle IP : '+d.host+'. Sauvegardee, numero de serie conserve. Redemarrage...'
            : d.save_failed ? 'IP trouvee : '+d.host+'. Sauvegarde impossible, aucun redemarrage.'
            : 'Logger retrouve : '+d.host+'. Numero de serie enregistre conserve.';
          if (!loggerPoll) loggerPoll = setInterval(pollLogger, 1000);
          return;
        }
        loggerStatus.textContent = 'Logger trouve : '+d.host+' / SN '+d.serial+'. Verifier puis Sauvegarder.';
        if (loggerGeneration !== d.generation) {
          loggerForm.querySelector('[name=host]').value = d.host;
          loggerForm.querySelector('[name=serial]').value = d.serial;
          loggerGeneration = d.generation;
        }
      } else if (d.state === 4) loggerStatus.textContent = 'Aucun logger trouve. Verifier le reseau et le numero de serie pour le recours TCP.';
      else if (d.state === 5) loggerStatus.textContent = 'Recherche interrompue. Verifier le Wi-Fi.';
    }
  } catch (e) {
    clearInterval(loggerPoll); loggerPoll = null;
    loggerStatus.textContent = e.message;
  }
}
async function discoverLogger() {
  loggerStatus.textContent = 'Demarrage de la recherche...';
  try {
    const serial = loggerForm.querySelector('[name=serial]').value;
    const r = await fetch('/logger-discovery/start', {method:'POST', headers:{'X-CSRF-Token':token}, body:new URLSearchParams({serial})});
    if (!r.ok) throw new Error(await r.text());
    await pollLogger();
  } catch (e) { loggerStatus.textContent = e.message; }
}
document.getElementById('logger-search').addEventListener('click', discoverLogger);
pollLogger();
</script>)LOGGER";
}
static void web_logger_discovery_begin() {
  config_web.on("/logger-discovery/start", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t serial = 0;
    const String text = config_web.arg("serial");
    logger_discovery_serial(text.c_str(), &serial);
    const bool ok = deye_logger_discovery_start(serial);
    config_web.send(ok ? 202 : 409, "text/plain; charset=utf-8", ok ? "Recherche en attente." :
      WiFi.status() != WL_CONNECTED ? "Connecter d'abord l'ecran au Wi-Fi." : "Recherche deja en cours ou lecteur indisponible.");
  });
  config_web.on("/logger-discovery/status", HTTP_GET, []() {
    if (!web_authorized()) return;
    DeyeLoggerDiscoveryStatus status;
    deye_logger_discovery_copy_status(&status);
    const String json = String("{\"state\":") + unsigned(status.state) +
      ",\"phase\":" + unsigned(status.phase) +
      ",\"automatic\":" + (status.automatic ? "true" : "false") +
      ",\"restart_pending\":" + (status.restart_pending ? "true" : "false") +
      ",\"save_failed\":" + (status.save_failed ? "true" : "false") +
      ",\"scanned\":" + status.scanned_hosts + ",\"total\":" + status.total_hosts +
      ",\"generation\":" + status.generation + ",\"serial\":" + status.serial +
      ",\"host\":\"" + status.host + "\",\"mac\":\"" + status.mac + "\"}";
    config_web.sendHeader("Cache-Control", "no-store");
    config_web.send(200, "application/json", json);
  });
}
