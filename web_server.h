#pragma once
#include <WebServer.h>
#include <Update.h>
#include <esp_system.h>
#include <math.h>
#include "history.h"
#include "settings.h"
#include "wifi_manager.h"

static WebServer config_web(80);
static String web_csrf;
static uint32_t web_restart_at = 0;
static bool web_upload_allowed = false;
static bool web_upload_ok = false;
static bool web_config_import_allowed = false;
static bool web_config_import_ok = false;
static String web_config_import_payload;

static String web_escape(String value) {
  value.replace("&", "&amp;");
  value.replace("<", "&lt;");
  value.replace(">", "&gt;");
  value.replace("\"", "&quot;");
  value.replace("'", "&#39;");
  return value;
}
static bool web_authorized() {
  if (!cfg_web_auth || config_web.authenticate(cfg_web_user.c_str(), cfg_web_password.c_str())) return true;
  config_web.requestAuthentication();
  return false;
}
static bool web_write_allowed() {
  if (!web_authorized()) return false;
  if (web_restart_at || config_web.header("X-CSRF-Token") != web_csrf) {
    config_web.send(403, "text/plain", "Session invalide. Rechargez la page.");
    return false;
  }
  return true;
}
static void web_reply(bool ok, const char *message, bool restart = false) {
  config_web.send(ok ? 200 : 400, "text/plain; charset=utf-8", message);
  if (ok && restart) web_restart_at = millis() + 1500;
}
static String web_input(const char *name, const char *label, String value, const char *type = "text") {
  return String("<label>") + label + "<input name='" + name + "' type='" + type + "' value='" + web_escape(value) + "'></label>";
}
static String web_check(const char *name, const char *label, bool value) {
  return String("<label><input type='checkbox' name='") + name + "' " + (value ? "checked" : "") + "> " + label + "</label>";
}
static String web_form(const char *action, const char *title) {
  return String("<details><summary>") + title + "</summary><form action='" + action + "'>";
}
static const char *WEB_FORM_END = "<button>Sauvegarder</button></form></details>";

struct WebRegisterField {
  const char *name;
  size_t offset;
  uint8_t kind;
};
#define WEB_REG(name, kind) \
  { #name, offsetof(CustomRegisters, name), kind }
static const WebRegisterField web_register_fields[] = {
  WEB_REG(pv1_power, 0), WEB_REG(pv2_power, 0), WEB_REG(pv3_power, 0), WEB_REG(pv_daily, 0),
  WEB_REG(battery_soc, 0), WEB_REG(battery_voltage, 0), WEB_REG(battery_power, 0), WEB_REG(battery_temp, 0),
  WEB_REG(grid_power, 0), WEB_REG(grid_status, 0), WEB_REG(grid_buy_daily, 0), WEB_REG(grid_sell_daily, 0),
  WEB_REG(load_power, 0), WEB_REG(ups_power, 0), WEB_REG(load_daily, 0), WEB_REG(dc_temp, 0),
  WEB_REG(ac_temp, 0), WEB_REG(smartload, 0), WEB_REG(connect_timeout, 1), WEB_REG(response_window, 1),
  WEB_REG(frame_timeout, 1), WEB_REG(block_interval, 1), WEB_REG(coeff_grid_power, 2),
  WEB_REG(coeff_load_power, 2), WEB_REG(coeff_ups_power, 2), WEB_REG(coeff_smartload, 2)
};
#undef WEB_REG

static String web_timezone_select() {
  struct TimezoneChoice { const char *name; const char *rule; };
  static const TimezoneChoice choices[] = {
    {"Europe / Paris", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"Europe / Londres", "GMT0BST,M3.5.0/1,M10.5.0/2"},
    {"Europe / Athenes", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"America / New York", "EST5EDT,M3.2.0,M11.1.0"},
    {"America / Chicago", "CST6CDT,M3.2.0,M11.1.0"},
    {"America / Denver", "MST7MDT,M3.2.0,M11.1.0"},
    {"America / Los Angeles", "PST8PDT,M3.2.0,M11.1.0"},
    {"UTC", "UTC0"}
  };
  String select = "<label>Fuseau horaire<select name='tz'>";
  bool selected = false;
  for (const auto &choice : choices) {
    const bool active = cfg_tz_rule == choice.rule;
    select += "<option value='" + String(choice.rule) + "'" + (active ? " selected" : "") + ">" + choice.name + "</option>";
    selected = selected || active;
  }
  // Une ancienne regle personnalisee reste selectionnable et n'est jamais perdue.
  if (!selected) select += "<option value='" + web_escape(cfg_tz_rule) + "' selected>Regle personnalisee existante</option>";
  return select + "</select></label>";
}

static String web_json_escape(String value) {
  value.replace("\\", "\\\\");
  value.replace("\"", "\\\"");
  value.replace("\n", "\\n");
  value.replace("\r", "\\r");
  return value;
}

static String web_dashboard_json() {
  DashboardData data = {};
  uint16_t pv_daily = 0, daily_load = 0, daily_buy = 0, daily_sell = 0;
  bool pv_daily_valid = false, on_grid = false;
  deye_copy_snapshot(&data, &pv_daily, &pv_daily_valid, &daily_load, &daily_buy, &daily_sell, &on_grid);
  DeyeDiagnostics diagnostic = {};
  deye_copy_diagnostics(&diagnostic);
  String payload;
  payload.reserve(600);
  payload += "{\"valid\":" + String(data.valid ? "true" : "false") + ",\"pv_w\":" + String(data.pv1_w + data.pv2_w + data.pv3_w);
  payload += ",\"grid_w\":" + String(data.grid_power) + ",\"load_w\":" + String(data.load_power);
  payload += ",\"battery_w\":" + String(data.battery_power) + ",\"soc\":" + String(data.battery_soc);
  payload += ",\"battery_v\":" + String(data.battery_voltage, 2) + ",\"on_grid\":" + String(on_grid ? "true" : "false");
  payload += ",\"daily_pv_dkwh\":" + String(pv_daily) + ",\"daily_load_dkwh\":" + String(daily_load);
  payload += ",\"daily_buy_dkwh\":" + String(daily_buy) + ",\"daily_sell_dkwh\":" + String(daily_sell);
  payload += ",\"block_age_s\":[";
  for (uint8_t i = 0; i < 3; ++i) {
    if (i) payload += ',';
    payload += diagnostic.block_last_success_ms[i] ? String((millis() - diagnostic.block_last_success_ms[i]) / 1000) : "null";
  }
  payload += "]}";
  return payload;
}

static String web_config_json() {
  const CustomRegisters regs = settings_load_registers();
  String payload = "{\n  \"format\": \"deye-guition-config-v1\",\n  \"firmware\": \"" + String(FIRMWARE_VERSION) + "\",\n";
  payload += "  \"logger_host\": \"" + web_json_escape(cfg_deye_host) + "\",\n  \"logger_serial\": " + String(cfg_logger_serial) + ",\n";
  payload += "  \"timezone\": \"" + web_json_escape(cfg_tz_rule) + "\",\n  \"ntp_primary\": \"" + web_json_escape(cfg_ntp_primary) + "\",\n  \"ntp_secondary\": \"" + web_json_escape(cfg_ntp_secondary) + "\",\n";
  payload += "  \"tempo_enabled\": " + String(cfg_tempo_enabled ? "true" : "false") + ",\n  \"tempo_colorblind\": " + String(cfg_tempo_colorblind_mode ? "true" : "false") + ",\n  \"ev_enabled\": " + String(cfg_ev_charger_enabled ? "true" : "false") + ",\n";
  payload += "  \"theme\": " + String(uint8_t(cfg_ui_theme)) + ",\n  \"day_brightness\": " + String(cfg_display.day_brightness) + ",\n  \"night_brightness\": " + String(cfg_display.night_brightness) + ",\n  \"night_enabled\": " + String(cfg_display.night_enabled ? "true" : "false") + ",\n  \"sunset_mode\": " + String(cfg_display.sunset_mode ? "true" : "false") + ",\n  \"latitude\": " + String(cfg_display.latitude, 5) + ",\n  \"longitude\": " + String(cfg_display.longitude, 5) + ",\n  \"night_start_hour\": " + String(cfg_display.night_start_hour) + ",\n  \"night_end_hour\": " + String(cfg_display.night_end_hour) + ",\n";
  payload += "  \"smartload\": " + String(settings_get_gen_mode() ? "true" : "false") + ",\n  \"registers\": {\n";
  for (size_t i = 0; i < sizeof(web_register_fields) / sizeof(web_register_fields[0]); ++i) {
    const WebRegisterField &field = web_register_fields[i];
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&regs) + field.offset;
    payload += "    \"" + String(field.name) + "\": ";
    payload += field.kind == 2 ? String(*reinterpret_cast<const float *>(ptr), 6) : String(field.kind == 1 ? *reinterpret_cast<const uint32_t *>(ptr) : *reinterpret_cast<const uint16_t *>(ptr));
    payload += i + 1 == sizeof(web_register_fields) / sizeof(web_register_fields[0]) ? "\n" : ",\n";
  }
  payload += "  }\n}\n";
  return payload;
}

static bool web_json_number(const String &json, const char *key, double &value) {
  const String needle = String('"') + key + '"';
  const int key_pos = json.indexOf(needle);
  if (key_pos < 0) return false;
  const int colon = json.indexOf(':', key_pos + needle.length());
  if (colon < 0) return false;
  char *end = nullptr;
  value = strtod(json.c_str() + colon + 1, &end);
  return end != json.c_str() + colon + 1 && isfinite(value);
}
static bool web_json_bool(const String &json, const char *key, bool &value) {
  const String needle = String('"') + key + '"';
  const int key_pos = json.indexOf(needle);
  const int colon = key_pos < 0 ? -1 : json.indexOf(':', key_pos + needle.length());
  if (colon < 0) return false;
  const String rest = json.substring(colon + 1); rest.startsWith("true") ? value = true : value = false;
  return rest.startsWith("true") || rest.startsWith("false");
}
static bool web_json_string(const String &json, const char *key, String &value) {
  const String needle = String('"') + key + '"';
  const int key_pos = json.indexOf(needle);
  const int colon = key_pos < 0 ? -1 : json.indexOf(':', key_pos + needle.length());
  if (colon < 0) return false;
  const int first = json.indexOf('"', colon + 1);
  const int last = first < 0 ? -1 : json.indexOf('"', first + 1);
  if (first < 0 || last < 0) return false;
  value = json.substring(first + 1, last);
  return value.indexOf('\\') < 0;  // L'export ne produit pas de champs configurables echappes.
}

static bool web_apply_config_json(const String &json) {
  String format, host, tz, primary, secondary;
  if (!web_json_string(json, "format", format) || format != "deye-guition-config-v1" ||
      !web_json_string(json, "logger_host", host) || host.isEmpty() || host.length() > 253 ||
      !web_json_string(json, "timezone", tz) || tz.isEmpty() || tz.length() > 127 ||
      !web_json_string(json, "ntp_primary", primary) || primary.isEmpty() || primary.length() > 253 ||
      !web_json_string(json, "ntp_secondary", secondary) || secondary.length() > 253) return false;
  double value = 0;
  if (!web_json_number(json, "logger_serial", value) || value < 1 || value > UINT32_MAX || floor(value) != value) return false;
  const uint32_t serial = uint32_t(value);
  bool tempo, colorblind, ev, night_enabled, sunset_mode, smartload;
  if (!web_json_bool(json, "tempo_enabled", tempo) || !web_json_bool(json, "tempo_colorblind", colorblind) ||
      !web_json_bool(json, "ev_enabled", ev) || !web_json_bool(json, "night_enabled", night_enabled) || !web_json_bool(json, "sunset_mode", sunset_mode) ||
      !web_json_bool(json, "smartload", smartload)) return false;
  if (!web_json_number(json, "theme", value) || value < 0 || value > 1 || floor(value) != value) return false;
  const UiThemeId theme = static_cast<UiThemeId>(uint8_t(value));
  DisplayConfig display = cfg_display;
  if (!web_json_number(json, "day_brightness", value) || value < DISPLAY_BRIGHTNESS_MIN || value > 255 || floor(value) != value) return false;
  display.day_brightness = uint8_t(value);
  if (!web_json_number(json, "night_brightness", value) || value < DISPLAY_BRIGHTNESS_MIN || value > 255 || floor(value) != value) return false;
  display.night_brightness = uint8_t(value);
  if (!web_json_number(json, "night_start_hour", value) || value < 0 || value > 23 || floor(value) != value) return false;
  display.night_start_hour = uint8_t(value);
  if (!web_json_number(json, "night_end_hour", value) || value < 0 || value > 23 || floor(value) != value) return false;
  display.night_end_hour = uint8_t(value); display.night_enabled = night_enabled; display.sunset_mode = sunset_mode;
  if (!web_json_number(json, "latitude", value) || value < -89 || value > 89) return false;
  display.latitude = float(value);
  if (!web_json_number(json, "longitude", value) || value < -180 || value > 180) return false;
  display.longitude = float(value);
  CustomRegisters regs = settings_load_registers();
  for (const auto &field : web_register_fields) {
    if (!web_json_number(json, field.name, value) || !isfinite(value)) return false;
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&regs) + field.offset;
    if (field.kind == 2) *reinterpret_cast<float *>(ptr) = float(value);
    else if (value < 0 || value > (field.kind == 1 ? 60000 : 65535) || floor(value) != value) return false;
    else if (field.kind == 1) *reinterpret_cast<uint32_t *>(ptr) = uint32_t(value);
    else *reinterpret_cast<uint16_t *>(ptr) = uint16_t(value);
  }
  if (!settings_registers_valid(regs) || !settings_display_valid(display)) return false;
  return settings_save_registers(regs) && settings_save_deye(host, serial) && settings_save_ntp(tz, primary, secondary) &&
    settings_save_tempo(tempo, colorblind, ev) && settings_set_ui_theme(theme) && settings_save_display(display) &&
    settings_set_gen_mode(smartload);
}

static void web_history_page() {
  if (!web_authorized()) return;
  const char *page = "<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'><title>Historique Deye</title><style>body{font:16px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}canvas{width:100%;background:#1e3044;border-radius:12px}a{color:#77d5b4}</style><h1>Historique 24 h</h1><p><a href='/dashboard'>Tableau de bord</a> · echantillon toutes les 5 min, remplacement du plus ancien apres 288 points.</p><canvas id=c width=720 height=260></canvas><script>fetch('/api/history').then(r=>r.json()).then(a=>{let c=document.getElementById('c'),x=c.getContext('2d'),w=c.width,h=c.height;x.clearRect(0,0,w,h);if(!a.length){x.fillStyle='#eef4fb';x.fillText('En attente des premiers echantillons.',20,30);return}let m=Math.max(1,...a.map(p=>Math.max(p[1],Math.abs(p[2]),p[3])));[['#facc15',1],['#77d5b4',3],['#60a5fa',2]].forEach(q=>{x.strokeStyle=q[0];x.beginPath();a.forEach((p,i)=>{let px=10+i*(w-20)/Math.max(1,a.length-1),py=h-15-(p[q[1]]/m)*(h-30);i?x.lineTo(px,py):x.moveTo(px,py)});x.stroke()});x.fillStyle='#eef4fb';x.fillText('Jaune: PV  Vert: consommation  Bleu: reseau',12,18)})</script>";
  config_web.send(200, "text/html; charset=utf-8", page);
}

static void web_dashboard_page() {
  if (!web_authorized()) return;
  const char *page = "<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'><title>Deye</title><style>body{font:17px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}.grid{display:grid;grid-template-columns:repeat(2,1fr);gap:12px}.card{background:#1e3044;padding:16px;border-radius:12px}b{font-size:26px}a{color:#77d5b4}</style><h1>Deye - tableau de bord</h1><p><a href='/'>Configuration</a> · <a href='/diagnostic'>Diagnostic</a> · <a href='/history'>Historique 24 h</a></p><div class=grid id=data>Lecture...</div><script>async function u(){let d=await(await fetch('/api/dashboard')).json();let x=[['PV',d.pv_w+' W'],['Reseau',d.grid_w+' W'],['Conso',d.load_w+' W'],['Batterie',d.soc+'% / '+d.battery_w+' W'],['Production jour',(d.daily_pv_dkwh/10).toFixed(1)+' kWh'],['Etat',d.valid?'Connecte':'Indisponible']];data.innerHTML=x.map(v=>'<div class=card>'+v[0]+'<br><b>'+v[1]+'</b></div>').join('')}u();setInterval(u,5000)</script>";
  config_web.send(200, "text/html; charset=utf-8", page);
}

static void web_diagnostic_page() {
  if (!web_authorized()) return;
  DeyeDiagnostics d = {}; deye_copy_diagnostics(&d);
  String page = "<!doctype html><meta charset=utf-8><title>Diagnostic Deye</title><style>body{font:17px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}table{border-collapse:collapse}td,th{padding:8px;border:1px solid #7990a5}a{color:#77d5b4}</style><h1>Diagnostic Deye / Solarman</h1><p><a href='/dashboard'>Tableau de bord</a> · <a href='/'>Configuration</a></p><p>Firmware : " + String(FIRMWARE_VERSION) + "<br>Derniere transaction : R" + String(d.last_reg) + " + " + String(d.last_count) + " | exception Modbus : " + String(d.last_exception) + "</p><table><tr><th>Bloc</th><th>OK</th><th>Echecs</th><th>Age derniere lecture</th></tr>";
  for (uint8_t i = 0; i < 3; ++i) page += "<tr><td>B" + String(i + 1) + "</td><td>" + String(d.block_success[i]) + "</td><td>" + String(d.block_failure[i]) + "</td><td>" + (d.block_last_success_ms[i] ? String((millis() - d.block_last_success_ms[i]) / 1000) + " s" : String("jamais")) + "</td></tr>";
  page += "</table><p><a href='/ve-probe'>Sonde VE (lecture seule)</a></p><p>Les ages sont des horodatages depuis la derniere lecture reussie. Rafraichir la page pour actualiser.</p>";
  config_web.send(200, "text/html; charset=utf-8", page);
}

// Cartographie des registres VE du SG02LP1. Les boutons n'envoient qu'une
// demande de lecture FC03 a SolarmanReader ; aucune ecriture Modbus n'existe
// dans ce parcours.
static void web_ve_probe_page() {
  if (!web_authorized()) return;
  String page = "<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'><title>Sonde VE</title><style>body{font:17px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}button{padding:12px;margin:6px 6px 6px 0;background:#77d5b4;color:#102030;border:0;border-radius:7px;font-weight:bold}a{color:#77d5b4}code{background:#1e3044;padding:2px 4px}</style><h1>Sonde VE SG02LP1</h1><p><a href='/diagnostic'>Diagnostic</a> · lecture seule <code>FC03</code> de R0 a R1023.</p><ol><li>Ne modifie rien, puis lancer <b>1. AVANT</b> et attendre la fin.</li><li>Sans modifier le LCD, lancer <b>2. TEMOIN</b>. Les mesures variables seront ecartees.</li><li>Modifier <em>un seul</em> reglage VE au LCD ou dans DeyeCloud.</li><li>Lancer <b>3. APRES</b>, puis telecharger le rapport.</li></ol><p id=status>" + web_escape(deye_probe_status_text()) + "</p><button onclick=go('/ve-probe/before')>1. Instantane AVANT</button><button onclick=go('/ve-probe/reference')>2. Instantane TEMOIN</button><button onclick=go('/ve-probe/after')>3. Instantane APRES</button><p><a href='/ve-probe/report.txt' download>Telecharger le rapport sonde (.txt)</a></p><script>const token='" + web_csrf + "';async function go(p){let r=await fetch(p,{method:'POST',headers:{'X-CSRF-Token':token}});status.textContent=await r.text();setTimeout(()=>location.reload(),2500)}if(/attente|en cours/.test(status.textContent))setTimeout(()=>location.reload(),2500)</script>";
  config_web.send(200, "text/html; charset=utf-8", page);
}

static void web_home() {
  if (!web_authorized()) return;
  config_web.sendHeader("Cache-Control", "no-store");
  config_web.sendHeader("X-Frame-Options", "DENY");
  String page;
  page.reserve(16000);
  page = F("<!doctype html><html lang='fr'><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Configuration Deye</title><style>body{font:16px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}details{background:#1e3044;margin:12px 0;padding:18px;border-radius:12px}summary{cursor:pointer;font-weight:bold}label{display:block;margin:16px 0}input,select,button,textarea{font:inherit;padding:10px;border-radius:6px;border:1px solid #7990a5;box-sizing:border-box}input:not([type=checkbox]),select,textarea{display:block;width:100%;margin-top:6px}button{background:#77d5b4;cursor:pointer}a{color:#77d5b4}#status{white-space:pre-wrap}progress{width:100%}</style><h1>Configuration Deye</h1><p><a href='/dashboard'>Tableau de bord</a> · <a href='/diagnostic'>Diagnostic Solarman</a></p><p>Les modifications de configuration sont appliquees apres redemarrage.</p>");
  page += web_form("/wifi", "Wi-Fi");
  page += web_input("ssid", "Reseau Wi-Fi (SSID)", cfg_wifi_ssid);
  page += web_input("password", "Mot de passe Wi-Fi : vide pour conserver", "", "password");
  page += WEB_FORM_END;
  page += web_form("/network", "Reseau : DHCP / IP statique");
  page += "<label>Mode d'adressage<select name='mode'><option value='dhcp'>DHCP (automatique)</option><option value='static'";
  page += cfg_network.use_static ? " selected" : "";
  page += ">IP statique</option></select></label>";
  page += web_input("ip", "Adresse IP", cfg_network.ip ? wifi_network_address(cfg_network.ip).toString() : WiFi.localIP().toString());
  page += web_input("mask", "Masque de sous-reseau", cfg_network.mask ? wifi_network_address(cfg_network.mask).toString() : WiFi.subnetMask().toString());
  page += web_input("gateway", "Passerelle", cfg_network.gateway ? wifi_network_address(cfg_network.gateway).toString() : WiFi.gatewayIP().toString());
  page += web_input("dns", "Serveur DNS (facultatif)", cfg_network.dns ? wifi_network_address(cfg_network.dns).toString() : String(""));
  page += "<p>En DHCP, les champs sont ignores. En IP statique, un DNS vide utilise la passerelle. Apres sauvegarde, retrouvez l'adresse IP sur l'ecran, menu WIFI / RESEAU.</p>";
  page += WEB_FORM_END;
  page += web_form("/deye", "Logger Deye");
  page += web_input("host", "Adresse du logger", cfg_deye_host);
  page += web_input("serial", "Numero de serie", String(cfg_logger_serial), "number");
  page += WEB_FORM_END;
  page += web_form("/ntp", "Heure / NTP");
  page += web_timezone_select();
  page += web_input("primary", "Serveur principal", cfg_ntp_primary);
  page += web_input("secondary", "Serveur secondaire", cfg_ntp_secondary);
  page += WEB_FORM_END;
  page += web_form("/display", "Tempo / VE / Theme / Ecran");
  page += web_check("tempo", "Activer Tempo", cfg_tempo_enabled);
  page += web_check("colorblind", "Mode daltonien", cfg_tempo_colorblind_mode);
  page += web_check("ev", "Activer la page VE (developpement en cours)", cfg_ev_charger_enabled);
  page += "<label>Theme<select name='theme'><option value='0'>Sombre</option><option value='1'";
  page += cfg_ui_theme == UI_THEME_LIGHT ? " selected" : "";
  page += ">Clair</option></select></label>";
  page += web_input("day_brightness", "Luminosite jour (220 min, 255 = 100 %)", String(cfg_display.day_brightness), "number");
  page += web_input("night_brightness", "Luminosite nuit (220 min, 255 = 100 %)", String(cfg_display.night_brightness), "number");
  page += web_check("night_enabled", "Activer le mode nuit programme", cfg_display.night_enabled);
  page += web_check("sunset_mode", "Suivre coucher / lever du soleil", cfg_display.sunset_mode);
  page += web_input("latitude", "Latitude installation (ex. 48.8566)", String(cfg_display.latitude, 5), "number");
  page += web_input("longitude", "Longitude installation (ex. 2.3522)", String(cfg_display.longitude, 5), "number");
  page += web_input("night_start", "Debut nuit, heure 0 a 23", String(cfg_display.night_start_hour), "number");
  page += web_input("night_end", "Fin nuit, heure 0 a 23", String(cfg_display.night_end_hour), "number");
  page += WEB_FORM_END;
  page += F("<details><summary>VE / registres d'ecriture</summary><form action='/ve'><p><b>DANGER :</b> ces adresses recoivent des ecritures Modbus vers l'onduleur. Une adresse, un bit ou une valeur incorrects peuvent causer des dommages importants. Ne les modifiez que si leur cartographie est confirmee pour votre onduleur.</p>");
  page += web_input("mode_register", "Registre mode VE (defaut R489)", String(cfg_ev_registers.mode_register), "number");
  page += web_input("max_power_register", "Registre puissance VE (defaut R490)", String(cfg_ev_registers.max_power_register), "number");
  page += F("<p>Les registres doivent etre distincts et ecartes de 125 positions maximum.</p>");
  page += web_check("write_enabled", "Debloquer les ecritures VE vers l'onduleur", cfg_ev_registers.write_enabled);
  page += web_check("write_confirm", "Je confirme connaitre ces registres et les risques materiels", false);
  page += WEB_FORM_END;
  page += F("<details><summary>Export / import de configuration JSON</summary><p>L'export est lisible et ne contient ni mot de passe Wi-Fi ni identifiants Web.</p><p><a href='/config/export'>Telecharger la configuration JSON</a></p><form action='/config/import'><label>Fichier JSON<input type='file' name='config' accept='.json,application/json' required></label><button>Importer et redemarrer</button></form></details>");
  page += web_form("/registers", "Registres / Coefficients / Temporisations (ms)");
  page += web_check("smartload", "Mode SmartLoad (decoche : GEN MO)", settings_get_gen_mode());
  CustomRegisters regs = settings_load_registers();
  for (const auto &field : web_register_fields) {
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&regs) + field.offset;
    String value = field.kind == 2 ? String(*reinterpret_cast<const float *>(ptr), 6) : String(field.kind == 1 ? *reinterpret_cast<const uint32_t *>(ptr) : *reinterpret_cast<const uint16_t *>(ptr));
    page += web_input(field.name, field.name, value);
  }
  page += WEB_FORM_END;
  page += web_form("/auth", "Authentification serveur Web");
  page += cfg_web_auth ? "<p>Authentification active.</p>" : "<p>Authentification desactivee.</p>";
  page += web_check("enabled", "Activer l'authentification", cfg_web_auth);
  page += web_input("user", "Identifiant", cfg_web_user);
  page += web_input("password", "Mot de passe : vide pour conserver", "", "password");
  page += "<p>Pour effacer les identifiants : menu Wi-Fi de l'ecran, cocher Reset authentification serveur Web, puis Sauvegarder.</p>";
  page += WEB_FORM_END;
  page += F("<details><summary>Mise a jour OTA</summary><form action='/update'><p>Selectionner le firmware applicatif .bin de cet ecran.</p><input type='file' name='firmware' accept='.bin' required><button>Installer la mise a jour</button></form><progress id='progress' max='100' value='0'></progress></details><p id='status' role='status'></p><script>const token='");
  page += web_csrf;
  page += F("';document.querySelectorAll('form').forEach(form=>form.addEventListener('submit',event=>{event.preventDefault();if(form.action.endsWith('/ve')&&form.querySelector('[name=write_enabled]').checked&&!confirm(\"DANGER : vous allez debloquer des ecritures vers l'onduleur. Confirmer uniquement si les registres et leurs valeurs sont verifies.\"))return;const buttons=document.querySelectorAll('button');buttons.forEach(b=>b.disabled=true);const status=document.getElementById('status');status.textContent='Envoi en cours...';const request=new XMLHttpRequest();request.open('POST',form.action);request.setRequestHeader('X-CSRF-Token',token);request.upload.onprogress=e=>{if(e.lengthComputable)document.getElementById('progress').value=e.loaded/e.total*100};request.onload=()=>{status.textContent=request.responseText;buttons.forEach(b=>b.disabled=false)};request.onerror=()=>{status.textContent='Connexion interrompue. Rechargez la page pour verifier.';buttons.forEach(b=>b.disabled=false)};request.send(new FormData(form))}));</script></html>");
  config_web.send(200, "text/html; charset=utf-8", page);
}
static bool web_number(const String &text, uint32_t maximum, uint32_t &value) {
  if (text.isEmpty()) return false;
  uint64_t number = 0;
  for (size_t i = 0; i < text.length(); ++i) {
    if (text[i] < '0' || text[i] > '9') return false;
    number = number * 10 + text[i] - '0';
    if (number > maximum) return false;
  }
  value = number;
  return true;
}
static void web_server_begin() {
  char token[33];
  snprintf(token, sizeof(token), "%08lx%08lx%08lx%08lx", (unsigned long)esp_random(), (unsigned long)esp_random(), (unsigned long)esp_random(), (unsigned long)esp_random());
  web_csrf = token;
  const char *headers[] = { "X-CSRF-Token" };
  config_web.collectHeaders(headers, 1);
  config_web.on("/", HTTP_GET, web_home);
  config_web.on("/dashboard", HTTP_GET, web_dashboard_page);
  config_web.on("/history", HTTP_GET, web_history_page);
  config_web.on("/diagnostic", HTTP_GET, web_diagnostic_page);
  config_web.on("/ve-probe", HTTP_GET, web_ve_probe_page);
  config_web.on("/ve-probe/report.txt", HTTP_GET, []() {
    if (!web_authorized()) return;
    config_web.sendHeader("Content-Disposition", "attachment; filename=sonde-ve-sg02lp1.txt");
    config_web.sendHeader("Cache-Control", "no-store");
    config_web.send(200, "text/plain; charset=utf-8", deye_probe_report_text());
  });
  config_web.on("/ve-probe/before", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    web_reply(deye_probe_queue_snapshot(1), "Instantane AVANT programme. Attendre la fin dans le moniteur serie.");
  });
  config_web.on("/ve-probe/reference", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    web_reply(deye_probe_queue_snapshot(2), "Instantane TEMOIN programme. Ne modifier aucun reglage avant la fin.");
  });
  config_web.on("/ve-probe/after", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    web_reply(deye_probe_queue_snapshot(3), "Instantane APRES programme. Attendre la fin dans le moniteur serie.");
  });
  config_web.on("/api/dashboard", HTTP_GET, []() { if (web_authorized()) config_web.send(200, "application/json", web_dashboard_json()); });
  config_web.on("/api/history", HTTP_GET, []() {
    if (!web_authorized()) return;
    HistoryPoint points[HISTORY_CAPACITY];
    const uint16_t count = history_copy(points, HISTORY_CAPACITY);
    String json = "["; json.reserve(count * 52 + 2);
    for (uint16_t i = 0; i < count; ++i) {
      if (i) json += ',';
      json += "[" + String(points[i].captured_ms) + "," + String(points[i].pv_w) + "," + String(points[i].grid_w) + "," + String(points[i].load_w) + "," + String(points[i].battery_w) + "," + String(points[i].battery_soc) + "]";
    }
    json += ']';
    config_web.send(200, "application/json", json);
  });
  config_web.on("/config/export", HTTP_GET, []() {
    if (!web_authorized()) return;
    config_web.sendHeader("Content-Disposition", "attachment; filename=deye-guition-config.json");
    config_web.send(200, "application/json; charset=utf-8", web_config_json());
  });
  config_web.on(
    "/config/import", HTTP_POST, []() {
      if (!web_write_allowed()) return;
      const bool ok = web_config_import_allowed && web_config_import_ok && web_apply_config_json(web_config_import_payload);
      web_config_import_allowed = false;
      web_config_import_payload = "";
      web_reply(ok, ok ? "Configuration importee. Redemarrage..." : "Import refuse : fichier JSON invalide ou sauvegarde impossible.", ok);
    },
    []() {
      HTTPUpload &upload = config_web.upload();
      if (upload.status == UPLOAD_FILE_START) {
        web_config_import_allowed = web_write_allowed();
        web_config_import_ok = web_config_import_allowed;
        web_config_import_payload = "";
      } else if (upload.status == UPLOAD_FILE_WRITE && web_config_import_allowed && web_config_import_ok) {
        if (web_config_import_payload.length() + upload.currentSize > 12000) web_config_import_ok = false;
        else web_config_import_payload.concat(reinterpret_cast<const char *>(upload.buf), upload.currentSize);
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        web_config_import_ok = false;
      }
    }
  );
  config_web.on("/auth", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String user = config_web.arg("user"), password = config_web.arg("password");
    if (password.isEmpty()) password = cfg_web_password;
    if (user.length() > 64 || password.length() > 128 || user.indexOf('\n') >= 0 || user.indexOf(':') >= 0 || password.indexOf('\n') >= 0) {
      web_reply(false, "Identifiants invalides.");
      return;
    }
    bool ok = settings_save_web_auth(config_web.hasArg("enabled"), user, password);
    web_reply(ok, ok ? "Authentification enregistree. Rechargez la page." : "Identifiant et mot de passe requis, ou sauvegarde impossible.");
  });
  config_web.on("/wifi", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String ssid = config_web.arg("ssid"), password = config_web.arg("password");
    if (password.isEmpty()) password = cfg_wifi_password;
    bool ok = ssid.length() <= 32 && password.length() <= 63 && settings_save_wifi(ssid, password);
    web_reply(ok, ok ? "Wi-Fi sauvegarde. Redemarrage..." : "Wi-Fi invalide ou sauvegarde impossible.", ok);
  });
  config_web.on("/network", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    NetworkConfig cfg = cfg_network;
    const String mode = config_web.arg("mode");
    if (mode != "dhcp" && mode != "static") {
      web_reply(false, "Mode reseau invalide.");
      return;
    }
    cfg.use_static = mode == "static";
    if (cfg.use_static) {
      String ip = config_web.arg("ip"), mask = config_web.arg("mask"), gateway = config_web.arg("gateway"), dns = config_web.arg("dns");
      ip.trim();
      mask.trim();
      gateway.trim();
      dns.trim();
      cfg.dns = 0;
      if (!network_parse_ipv4(ip.c_str(), cfg.ip) || !network_parse_ipv4(mask.c_str(), cfg.mask) || !network_parse_ipv4(gateway.c_str(), cfg.gateway) || (!dns.isEmpty() && !network_parse_ipv4(dns.c_str(), cfg.dns)) || !network_config_valid(cfg)) {
        web_reply(false, "Configuration invalide : verifier les adresses IPv4, le masque et la passerelle dans le meme sous-reseau, differente de l'adresse de l'ecran.");
        return;
      }
    }
    if (!settings_save_network(cfg)) {
      web_reply(false, "Sauvegarde reseau impossible.");
      return;
    }
    String message = cfg.use_static
                       ? String("Reseau sauvegarde. Redemarrage... Nouvelle adresse : http://") + wifi_network_address(cfg.ip).toString() + "/"
                       : String("DHCP sauvegarde. Redemarrage... Consultez l'adresse attribuee dans WIFI / RESEAU sur l'ecran.");
    web_reply(true, message.c_str(), true);
  });
  config_web.on("/deye", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t serial;
    String host = config_web.arg("host");
    if (host.isEmpty() || host.length() > 253 || !web_number(config_web.arg("serial"), UINT32_MAX, serial) || !serial) {
      web_reply(false, "Logger invalide.");
      return;
    }
    const bool ok = settings_save_deye(host, serial);
    web_reply(ok, ok ? "Logger sauvegarde. Redemarrage..." : "Sauvegarde logger impossible.", ok);
  });
  config_web.on("/ntp", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String tz = config_web.arg("tz"), primary = config_web.arg("primary"), secondary = config_web.arg("secondary");
    if (tz.isEmpty() || primary.isEmpty() || tz.length() > 127 || primary.length() > 253 || secondary.length() > 253) {
      web_reply(false, "Parametres NTP invalides.");
      return;
    }
    const bool ok = settings_save_ntp(tz, primary, secondary);
    web_reply(ok, ok ? "Heure sauvegardee. Redemarrage..." : "Sauvegarde NTP impossible.", ok);
  });
  config_web.on("/display", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t theme;
    if (!web_number(config_web.arg("theme"), 1, theme)) {
      web_reply(false, "Theme invalide.");
      return;
    }
    uint32_t day, night, night_start, night_end;
    const String latitude_text = config_web.arg("latitude"), longitude_text = config_web.arg("longitude");
    char *latitude_end = nullptr, *longitude_end = nullptr;
    const float latitude = strtof(latitude_text.c_str(), &latitude_end);
    const float longitude = strtof(longitude_text.c_str(), &longitude_end);
    if (!web_number(config_web.arg("day_brightness"), 255, day) || day < DISPLAY_BRIGHTNESS_MIN ||
        !web_number(config_web.arg("night_brightness"), 255, night) || night < DISPLAY_BRIGHTNESS_MIN ||
        !web_number(config_web.arg("night_start"), 23, night_start) || !web_number(config_web.arg("night_end"), 23, night_end) ||
        latitude_end == latitude_text.c_str() || *latitude_end || !isfinite(latitude) || latitude < -89 || latitude > 89 ||
        longitude_end == longitude_text.c_str() || *longitude_end || !isfinite(longitude) || longitude < -180 || longitude > 180) {
      web_reply(false, "Luminosite ou horaires invalides."); return;
    }
    DisplayConfig display = cfg_display;
    display.day_brightness = day; display.night_brightness = night; display.night_start_hour = night_start;
    display.night_end_hour = night_end; display.night_enabled = config_web.hasArg("night_enabled");
    display.sunset_mode = config_web.hasArg("sunset_mode"); display.latitude = latitude; display.longitude = longitude;
    bool ok = settings_save_tempo(config_web.hasArg("tempo"), config_web.hasArg("colorblind"), config_web.hasArg("ev"));
    ok = ok && settings_set_ui_theme(static_cast<UiThemeId>(theme)) && settings_save_display(display);
    web_reply(ok, ok ? "Affichage sauvegarde. Redemarrage..." : "Sauvegarde impossible.", ok);
  });
  config_web.on("/ve", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t mode_register = 0, max_power_register = 0;
    const bool write_enabled = config_web.hasArg("write_enabled");
    if (!web_number(config_web.arg("mode_register"), UINT16_MAX, mode_register) ||
        !web_number(config_web.arg("max_power_register"), UINT16_MAX, max_power_register) ||
        mode_register == max_power_register ||
        (mode_register > max_power_register ? mode_register - max_power_register : max_power_register - mode_register) >= 125) {
      web_reply(false, "Registres VE invalides : ils doivent etre distincts et espaces de 125 positions maximum.");
      return;
    }
    if (write_enabled && !config_web.hasArg("write_confirm")) {
      web_reply(false, "Confirmation obligatoire avant de debloquer les ecritures VE.");
      return;
    }
    EvRegisterConfig cfg = cfg_ev_registers;
    cfg.mode_register = uint16_t(mode_register);
    cfg.max_power_register = uint16_t(max_power_register);
    cfg.write_enabled = write_enabled;
    const bool ok = settings_save_ev_registers(cfg);
    web_reply(ok, ok ? (write_enabled ? "Ecritures VE debloquees. Redemarrage..." : "Ecritures VE verrouillees. Redemarrage...") : "Sauvegarde VE impossible.", ok);
  });
  config_web.on("/registers", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    CustomRegisters regs = settings_load_registers();
    for (const auto &field : web_register_fields) {
      String input = config_web.arg(field.name);
      uint8_t *ptr = reinterpret_cast<uint8_t *>(&regs) + field.offset;
      if (field.kind == 2) {
        char *end = nullptr;
        float value = strtof(input.c_str(), &end);
        if (input.isEmpty() || end == input.c_str() || *end || !isfinite(value)) {
          web_reply(false, "Coefficient invalide.");
          return;
        }
        *reinterpret_cast<float *>(ptr) = value;
      } else {
        uint32_t value;
        if (!web_number(input, field.kind == 1 ? 60000 : 65535, value)) {
          web_reply(false, "Registre ou temporisation invalide.");
          return;
        }
        if (field.kind == 1) *reinterpret_cast<uint32_t *>(ptr) = value;
        else *reinterpret_cast<uint16_t *>(ptr) = value;
      }
    }
    const bool ok = settings_save_registers(regs) && settings_set_gen_mode(config_web.hasArg("smartload"));
    web_reply(ok, ok ? "Registres sauvegardes. Redemarrage..." : "Registres invalides ou sauvegarde impossible.", ok);
  });
  config_web.on(
    "/update", HTTP_POST, []() {
      if (!web_write_allowed()) return;
      if (!web_upload_allowed) {
        web_reply(false, "Aucun firmware recu.");
        return;
      }
      web_upload_allowed = false;
      web_reply(web_upload_ok, web_upload_ok ? "Mise a jour terminee. Redemarrage..." : "Mise a jour refusee ou interrompue. Firmware actuel conserve.", web_upload_ok);
    },
    []() {
      HTTPUpload &upload = config_web.upload();
      if (upload.status == UPLOAD_FILE_START) {
        web_upload_ok = false;
        web_upload_allowed = web_write_allowed();
        if (web_upload_allowed) web_upload_ok = Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
      } else if (upload.status == UPLOAD_FILE_WRITE && web_upload_allowed && web_upload_ok) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          web_upload_ok = false;
          Update.abort();
        }
      } else if (upload.status == UPLOAD_FILE_END && web_upload_allowed && web_upload_ok) {
        web_upload_ok = Update.end(true);
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        if (web_upload_allowed) Update.abort();
        web_upload_ok = false;
        web_upload_allowed = false;
      }
    });
  config_web.onNotFound([]() {
    if (web_authorized()) config_web.send(404, "text/plain", "Page introuvable");
  });
  config_web.begin();
}
static void web_server_process() {
  config_web.handleClient();
  if (web_restart_at && static_cast<int32_t>(millis() - web_restart_at) >= 0) ESP.restart();
}
