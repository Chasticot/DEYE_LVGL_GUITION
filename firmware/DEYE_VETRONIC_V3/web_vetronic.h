#pragma once

static void web_vetronic_page() {
  if (!web_authorized()) return;
  String page = R"HTML(<!doctype html><html lang=fr><meta charset=utf-8>
<meta name=viewport content='width=device-width,initial-scale=1'><title>VE TRONIC V3</title>
<style>body{font:17px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}a{color:#77d5b4}.card{background:#1e3044;padding:16px;border-radius:8px;margin:12px 0}button,input{font:inherit;padding:10px;margin:6px 0}button{cursor:pointer;background:#77d5b4;color:#101b29;border:0;border-radius:6px}button:disabled{opacity:.4;cursor:default}.stop{background:#d9534f;color:white}.actions{display:flex;gap:8px;flex-wrap:wrap}label{display:block}#result,#message{white-space:pre-wrap;overflow-wrap:anywhere}</style>
<h1>VE TRONIC / WB01 · V3</h1><p><a href='/dashboard'>Tableau de bord</a> · <a href='/'>Configuration</a></p>
<div class=card><strong id=mode>Connexion...</strong><p id=measure>Courant : -- A · Puissance estimée : -- W</p><p id=target>Consigne : --</p><p id=message></p><p id=policy></p></div>
<div class=card><label>Intensité manuelle <input id=amps type=number min=6 max=32 step=1 value=6> A</label>
<div class=actions><button class=stop data-mode=stop>Arrêt</button><button data-mode=manual>Charge immédiate</button><button data-mode=solar>Solaire</button><button data-mode=legacy>Rendre la main à la borne</button></div>
<p>Rendre la main laisse la borne gérer sa charge. Vous pouvez reprendre le pilotage ici à tout moment.</p></div>
<div class=card><h2>Protection SOC solaire</h2><label><input id=guard type=checkbox> Activer la protection</label><label>MIN / arrêt <input id=socStop type=number min=0 max=95 step=1 value=30> %</label><label>MAX / reprise <input id=socResume type=number min=5 max=100 step=1 value=35> %</label><p>Écart minimum : 5 points. Ce réglage concerne le mode Solaire et conserve le mode actif.</p><button id=saveSoc>Enregistrer SOC</button><p id=socHint></p></div>
<div class=card id=result>Aucune commande envoyée.</div>
<script>const token=)HTML";
  page += '"' + web_csrf + '"';
  page += R"HTML(;
const el=id=>document.getElementById(id);let sending=false,socDirty=false,socLoaded=false;
const modes=Array.from(document.querySelectorAll('[data-mode]'));
for(const id of ['guard','socStop','socResume'])el(id).addEventListener('input',()=>socDirty=true);
async function status(){try{const r=await fetch('/api/vetronic',{cache:'no-store'});if(!r.ok)throw Error('Passerelle indisponible');const d=await r.json();
el('mode').textContent=d.mode_label;el('measure').textContent=d.measured?'Courant : '+d.amps.toFixed(1)+' A · Puissance estimée : ~'+d.power_w+' W':'Courant : -- A · Puissance estimée : -- W';
el('target').textContent=d.online?(d.mode==='legacy'?'Main rendue à la borne':'Consigne : '+d.target_a+' A · '+(d.confirmed?'confirmée par la borne':'confirmation à vérifier')):'Consigne : --';
el('message').textContent=d.message;el('policy').textContent=d.tariff_pause?'Pause tarifaire · reprise de la charge manuelle en attente':d.manual_allowed?'Charge manuelle autorisée par les tarifs':'Charge manuelle interdite par les tarifs';
if(d.result)el('result').textContent=d.result;const busy=sending||d.busy;
el('amps').max=d.online?d.manual_limit_a:32;
for(const b of modes)b.disabled=busy||!d.enabled||(b.dataset.mode!=='stop'&&!d.online)||(b.dataset.mode==='manual'&&(!d.manual_allowed||d.manual_limit_a<6));
el('saveSoc').disabled=busy||!d.enabled||!d.soc_api;el('socHint').textContent=d.soc_api?'Protection enregistrée sur la passerelle':'Protection indisponible : vérifier la liaison et le firmware de la passerelle';
if(!socDirty&&d.soc_api){el('guard').checked=d.soc_guard;el('socStop').value=d.soc_stop;el('socResume').value=d.soc_resume;socLoaded=true;}
}catch(e){el('mode').textContent='Connexion indisponible';el('measure').textContent='Courant : -- A · Puissance estimée : -- W';el('target').textContent='Consigne : --';el('message').textContent=e.message;for(const b of modes)b.disabled=true;el('saveSoc').disabled=true;}}
async function command(path,data){sending=true;for(const b of modes)b.disabled=true;el('saveSoc').disabled=true;
try{const r=await fetch(path,{method:'POST',headers:{'X-CSRF-Token':token,'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(data)});const text=await r.text();el('result').textContent=text;if(!r.ok)throw Error(text);}
catch(e){el('result').textContent='Résultat à vérifier : '+e.message;}finally{sending=false;await status();}}
for(const b of modes)b.onclick=()=>{const data={mode:b.dataset.mode};if(data.mode==='manual'){if(!el('amps').reportValidity())return;data.amps=el('amps').value;}command('/vetronic/mode',data);};
el('saveSoc').onclick=()=>{if(!socLoaded||!el('socStop').reportValidity()||!el('socResume').reportValidity())return;const stop=+el('socStop').value,resume=+el('socResume').value;if(resume<stop+5){el('result').textContent='MAX doit dépasser MIN d’au moins 5 points.';return;}command('/vetronic/soc',{enabled:el('guard').checked?'1':'0',stop,resume});};
// SOC drafts stay visible until reload; a rejected command must not erase them.
status();setInterval(status,3000);</script></html>)HTML";
  config_web.sendHeader("Cache-Control", "no-store");
  config_web.send(200, "text/html; charset=utf-8", page);
}

static void web_vetronic_begin() {
  config_web.on("/vetronic", HTTP_GET, web_vetronic_page);
  config_web.on("/api/vetronic", HTTP_GET, []() {
    if (!web_authorized()) return;
    config_web.sendHeader("Cache-Control", "no-store");
    config_web.send(200, "application/json", vt_status_json());
  });
  config_web.on("/vetronic-host", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    const bool ok = vt_save_host(config_web.arg("host").c_str());
    web_reply(ok, ok ? "Adresse VE TRONIC enregistree." : "Adresse IPv4 invalide ou commande en cours.");
  });
  config_web.on("/vetronic/mode", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    const VtMode mode = vt_parse_mode(config_web.arg("mode").c_str());
    int amps = 0;
    if (mode == VT_UNKNOWN || (mode == VT_MANUAL && !vt_parse_unsigned(config_web.arg("amps").c_str(), 32, amps))) {
      web_reply(false, "Mode ou intensite invalide."); return;
    }
    const bool ok = vt_submit(mode, amps);
    web_reply(ok, ok ? "Commande en attente. Consulter le resultat et la consigne relue." :
      "Commande refusee : verifier la liaison, les tarifs et le plafond manuel.");
  });
  config_web.on("/vetronic/soc", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    int stop = 0, resume = 0;
    const String enabled = config_web.arg("enabled");
    if ((enabled != "0" && enabled != "1") || !vt_parse_unsigned(config_web.arg("stop").c_str(), 95, stop) ||
        !vt_parse_unsigned(config_web.arg("resume").c_str(), 100, resume) || !vt_soc_guard_valid(stop, resume)) {
      web_reply(false, "SOC invalide : MIN 0-95, MAX 5-100, ecart minimum 5 points."); return;
    }
    const bool ok = vt_submit_soc_guard(enabled == "1", stop, resume);
    web_reply(ok, ok ? "Protection SOC en attente. Consulter les valeurs relues." :
      "Protection SOC indisponible ou commande en cours.");
  });
}
