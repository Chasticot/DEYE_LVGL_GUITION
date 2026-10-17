#pragma once
static String web_logger_discovery_script() {
  return R"LOGGER(<script>
let loggerGeneration=-1, loggerPoll=null, loggerLatest=null;
const loggerForm=document.querySelector("form[action='/deye']");
const loggerStatus=document.getElementById('logger-status');
const loggerTransport=document.getElementById('logger-transport');
const loggerCandidates=document.getElementById('logger-candidates');
const loggerField=name=>loggerForm.querySelector('[name='+name+']');
function loggerIdentity() {
  const value=String(loggerField('serial').value).trim().toUpperCase();
  return loggerTransport.value==='dl1000b' ? value : value.replace(/^0+(?=\d)/,'');
}
function matchingLoggerResult(d) {
  return d.model===loggerTransport.value && d.identity===loggerIdentity() &&
    Number(loggerField('port').value)===d.port && Number(loggerField('unit').value)===d.unit;
}
function updateLoggerTransport(changePort) {
  if (!loggerTransport) return;
  if (changePort) loggerField('port').value=loggerTransport.value==='dl1000b' ? 502 : 8899;
  document.getElementById('logger-search').disabled=false;
  loggerCandidates.replaceChildren();
  if (changePort) loggerStatus.textContent='Renseigner le numero de serie avant de rechercher le logger.';
}
async function pollLogger() {
  try {
    const r=await fetch('/logger-discovery/status',{cache:'no-store'});
    if (!r.ok) throw new Error('Lecture impossible ('+r.status+').');
    const d=await r.json();loggerLatest=d;
    if (d.state===1 || d.state===2) {
      const phase=['Demarrage','UDP broadcast','TCP/V5','UDP unicast','Modbus TCP'][d.phase] || 'Demarrage';
      loggerStatus.textContent='Recherche '+(d.automatic ? 'auto ' : '')+phase+' : '+d.scanned+' / '+d.total+'. Patientez...';
      if (!loggerPoll) loggerPoll=setInterval(pollLogger,1000);
      return;
    }
    clearInterval(loggerPoll);loggerPoll=null;
    if (d.state===3 && d.automatic) {
      loggerStatus.textContent=d.restart_pending ? 'Nouvelle IP : '+d.host+'. Sauvegardee, numero de serie conserve. Redemarrage...'
        : d.save_failed ? 'IP trouvee : '+d.host+'. Sauvegarde impossible, aucun redemarrage.'
        : 'Logger retrouve : '+d.host+'. Numero de serie enregistre conserve.';
      loggerPoll=setInterval(pollLogger,1000);return;
    }
    if ((d.state===3 || d.state===6) && !matchingLoggerResult(d)) {
      loggerCandidates.replaceChildren();loggerStatus.textContent='Resultat pour un autre type, numero ou port. Relancer la recherche.';return;
    }
    if (d.state===3) {
      loggerStatus.textContent='Logger trouve : '+d.host+' / SN '+d.identity+' verifie. Verifier puis Sauvegarder.';
      loggerCandidates.replaceChildren();
      if (loggerGeneration!==d.generation) { loggerField('host').value=d.host;loggerGeneration=d.generation; }
    } else if (d.state===6) {
      loggerStatus.textContent='Serveurs Modbus detectes ; numero de serie non verifie. Choisir uniquement le logger attendu, puis Sauvegarder.';
      loggerCandidates.replaceChildren();
      for (const host of d.candidates || []) {
        const button=document.createElement('button');button.type='button';
        button.textContent='Choisir '+host+':'+d.port+' (SN non verifie)';
        button.addEventListener('click',()=>{
          if (!matchingLoggerResult(d) || !loggerLatest || loggerLatest.generation!==d.generation) return;
          loggerField('host').value=host;
          loggerStatus.textContent='IP candidate choisie : '+host+'. Numero non verifie ; verifier puis Sauvegarder.';
        });loggerCandidates.appendChild(button);
      }
    } else if (d.state===4) loggerStatus.textContent='Aucun logger trouve. Verifier le numero, le reseau et le service local du logger.';
    else if (d.state===5) loggerStatus.textContent='Recherche interrompue. Verifier le Wi-Fi.';
  } catch(e) { clearInterval(loggerPoll);loggerPoll=null;loggerStatus.textContent=e.message; }
}
async function discoverLogger() {
  const serial=loggerIdentity();
  if (!serial || serial==='0' || (loggerTransport.value!=='dl1000b' && serial==='123456789')) {
    loggerStatus.textContent='Renseigner le vrai numero de serie du logger.';return;
  }
  loggerCandidates.replaceChildren();loggerStatus.textContent='Demarrage de la recherche...';
  try {
    const body=new URLSearchParams({serial,model:loggerTransport.value,port:loggerField('port').value,unit:loggerField('unit').value});
    const r=await fetch('/logger-discovery/start',{method:'POST',headers:{'X-CSRF-Token':token},body});
    if (!r.ok) throw new Error(await r.text());
    await pollLogger();
  } catch(e) { loggerStatus.textContent=e.message; }
}
document.getElementById('logger-search').addEventListener('click',discoverLogger);
if (loggerTransport) loggerTransport.addEventListener('change',()=>updateLoggerTransport(true));
updateLoggerTransport(false);pollLogger();
</script>)LOGGER";
}
static void web_logger_discovery_begin() {
  config_web.on("/logger-discovery/start",HTTP_POST,[]() {
    if (!web_write_allowed()) return;
    LoggerModel model;uint32_t port=0,unit=0,numeric=0;char identity[33];
    if (!logger_model_parse(config_web.arg("model").c_str(),model) ||
        !web_number(config_web.arg("port"),65535,port) || !port ||
        !web_number(config_web.arg("unit"),247,unit) || !unit ||
        !logger_identity_parse(config_web.arg("serial").c_str(),model,identity,sizeof(identity),numeric) ||
        (model!=LoggerModel::Dl1000B && numeric==123456789UL)) {
      config_web.send(400,"text/plain; charset=utf-8","Choisir le type et renseigner le vrai numero, le port et l'unite.");return;
    }
    const bool ok=deye_logger_discovery_start(identity,model,uint16_t(port),uint8_t(unit));
    config_web.send(ok ? 202 : 409,"text/plain; charset=utf-8",ok ? "Recherche en attente." :
      WiFi.status()!=WL_CONNECTED ? "Connecter d'abord l'ecran au Wi-Fi." : "Recherche deja en cours ou lecteur indisponible.");
  });
  config_web.on("/logger-discovery/status",HTTP_GET,[]() {
    if (!web_authorized()) return;
    DeyeLoggerDiscoveryStatus s;deye_logger_discovery_copy_status(&s);
    String json=String("{\"state\":")+unsigned(s.state)+",\"phase\":"+unsigned(s.phase)+
      ",\"automatic\":"+(s.automatic ? "true" : "false")+",\"restart_pending\":"+(s.restart_pending ? "true" : "false")+
      ",\"save_failed\":"+(s.save_failed ? "true" : "false")+",\"scanned\":"+s.scanned_hosts+",\"total\":"+s.total_hosts+
      ",\"generation\":"+s.generation+",\"serial\":"+s.serial+",\"identity\":\""+s.identity+"\",\"model\":\""+
      logger_model_key(s.model)+"\",\"port\":"+s.port+",\"unit\":"+s.unit+",\"host\":\""+s.host+"\",\"mac\":\""+s.mac+"\",\"candidates\":[";
    for (uint8_t i=0;i<s.candidate_count;++i) { if (i) json+=",";json+=String("\"")+s.candidates[i]+"\""; }
    json+="]}";config_web.sendHeader("Cache-Control","no-store");config_web.send(200,"application/json",json);
  });
}
