#pragma once
#include <WebServer.h>
#include <Update.h>
#include <esp_system.h>
#include <math.h>
#include "settings.h"
#include "wifi_manager.h"

static WebServer config_web(80);
static String web_csrf;
static uint32_t web_restart_at = 0;
static bool web_upload_allowed = false;
static bool web_upload_ok = false;

static String web_escape(String value) {
  value.replace("&", "&amp;"); value.replace("<", "&lt;");
  value.replace(">", "&gt;"); value.replace("\"", "&quot;"); value.replace("'", "&#39;");
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

struct WebRegisterField { const char *name; size_t offset; uint8_t kind; };
#define WEB_REG(name, kind) {#name, offsetof(CustomRegisters, name), kind}
static const WebRegisterField web_register_fields[] = {
  WEB_REG(pv1_power,0), WEB_REG(pv2_power,0), WEB_REG(pv3_power,0), WEB_REG(pv_daily,0),
  WEB_REG(battery_soc,0), WEB_REG(battery_voltage,0), WEB_REG(battery_power,0), WEB_REG(battery_temp,0),
  WEB_REG(grid_power,0), WEB_REG(grid_status,0), WEB_REG(grid_buy_daily,0), WEB_REG(grid_sell_daily,0),
  WEB_REG(load_power,0), WEB_REG(ups_power,0), WEB_REG(load_daily,0), WEB_REG(dc_temp,0),
  WEB_REG(ac_temp,0), WEB_REG(smartload,0), WEB_REG(connect_timeout,1), WEB_REG(response_window,1),
  WEB_REG(frame_timeout,1), WEB_REG(block_interval,1), WEB_REG(coeff_grid_power,2),
  WEB_REG(coeff_load_power,2), WEB_REG(coeff_ups_power,2), WEB_REG(coeff_smartload,2)
};
#undef WEB_REG

static void web_home() {
  if (!web_authorized()) return;
  config_web.sendHeader("Cache-Control", "no-store");
  config_web.sendHeader("X-Frame-Options", "DENY");
  String page;
  page.reserve(16000);
  page = F("<!doctype html><html lang='fr'><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>Configuration Deye</title><style>body{font:16px system-ui;background:#101b29;color:#eef4fb;max-width:760px;margin:24px auto;padding:16px}details{background:#1e3044;margin:12px 0;padding:18px;border-radius:12px}summary{cursor:pointer;font-weight:bold}label{display:block;margin:16px 0}input,select,button{font:inherit;padding:10px;border-radius:6px;border:1px solid #7990a5;box-sizing:border-box}input:not([type=checkbox]),select{display:block;width:100%;margin-top:6px}button{background:#77d5b4;cursor:pointer}#status{white-space:pre-wrap}progress{width:100%}</style><h1>Configuration Deye</h1><p>Les modifications de configuration sont appliquees apres redemarrage.</p>");
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
  page += web_input("tz", "Regle du fuseau horaire", cfg_tz_rule);
  page += web_input("primary", "Serveur principal", cfg_ntp_primary);
  page += web_input("secondary", "Serveur secondaire", cfg_ntp_secondary);
  page += WEB_FORM_END;
  page += web_form("/display", "Tempo / VE / Theme");
  page += web_check("tempo", "Activer Tempo", cfg_tempo_enabled);
  page += web_check("colorblind", "Mode daltonien", cfg_tempo_colorblind_mode);
  page += web_check("ev", "Activer la page VE", cfg_ev_charger_enabled);
  page += "<label>Theme<select name='theme'><option value='0'>Sombre</option><option value='1'";
  page += cfg_ui_theme == UI_THEME_LIGHT ? " selected" : "";
  page += ">Clair</option></select></label>";
  page += WEB_FORM_END;
  page += web_form("/registers", "Registres / Coefficients / Temporisations (ms)");
  page += web_check("smartload", "Mode SmartLoad (decoche : GEN MO)", settings_get_gen_mode());
  CustomRegisters regs = settings_load_registers();
  for (const auto &field : web_register_fields) {
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&regs) + field.offset;
    String value = field.kind == 2 ? String(*reinterpret_cast<const float *>(ptr), 6) :
      String(field.kind == 1 ? *reinterpret_cast<const uint32_t *>(ptr) : *reinterpret_cast<const uint16_t *>(ptr));
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
  page += F("';document.querySelectorAll('form').forEach(form=>form.addEventListener('submit',event=>{event.preventDefault();const buttons=document.querySelectorAll('button');buttons.forEach(b=>b.disabled=true);const status=document.getElementById('status');status.textContent='Envoi en cours...';const request=new XMLHttpRequest();request.open('POST',form.action);request.setRequestHeader('X-CSRF-Token',token);request.upload.onprogress=e=>{if(e.lengthComputable)document.getElementById('progress').value=e.loaded/e.total*100};request.onload=()=>{status.textContent=request.responseText;buttons.forEach(b=>b.disabled=false)};request.onerror=()=>{status.textContent='Connexion interrompue. Rechargez la page pour verifier.';buttons.forEach(b=>b.disabled=false)};request.send(new FormData(form))}));</script></html>");
  config_web.send(200, "text/html; charset=utf-8", page);
}
static bool web_number(const String &text, uint32_t maximum, uint32_t &value) {
  if (text.isEmpty()) return false;
  uint64_t number = 0;
  for (size_t i=0; i<text.length(); ++i) {
    if (text[i] < '0' || text[i] > '9') return false;
    number = number * 10 + text[i] - '0';
    if (number > maximum) return false;
  }
  value = number; return true;
}
static void web_server_begin() {
  char token[33];
  snprintf(token, sizeof(token), "%08lx%08lx%08lx%08lx", (unsigned long)esp_random(), (unsigned long)esp_random(), (unsigned long)esp_random(), (unsigned long)esp_random());
  web_csrf = token;
  const char *headers[] = {"X-CSRF-Token"};
  config_web.collectHeaders(headers, 1);
  config_web.on("/", HTTP_GET, web_home);
  config_web.on("/auth", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String user = config_web.arg("user"), password = config_web.arg("password");
    if (password.isEmpty()) password = cfg_web_password;
    if (user.length()>64 || password.length()>128 || user.indexOf('\n')>=0 || user.indexOf(':')>=0 || password.indexOf('\n')>=0) {
      web_reply(false, "Identifiants invalides."); return;
    }
    bool ok = settings_save_web_auth(config_web.hasArg("enabled"), user, password);
    web_reply(ok, ok ? "Authentification enregistree. Rechargez la page." : "Identifiant et mot de passe requis, ou sauvegarde impossible.");
  });
  config_web.on("/wifi", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String ssid = config_web.arg("ssid"), password = config_web.arg("password");
    if (password.isEmpty()) password = cfg_wifi_password;
    bool ok = ssid.length()<=32 && password.length()<=63 && settings_save_wifi(ssid,password);
    web_reply(ok, ok ? "Wi-Fi sauvegarde. Redemarrage..." : "Wi-Fi invalide ou sauvegarde impossible.", ok);
  });
  config_web.on("/network", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    NetworkConfig cfg = cfg_network;
    const String mode = config_web.arg("mode");
    if (mode != "dhcp" && mode != "static") {web_reply(false,"Mode reseau invalide.");return;}
    cfg.use_static = mode == "static";
    if (cfg.use_static) {
      String ip = config_web.arg("ip"), mask = config_web.arg("mask"), gateway = config_web.arg("gateway"), dns = config_web.arg("dns");
      ip.trim(); mask.trim(); gateway.trim(); dns.trim();
      cfg.dns = 0;
      if (!network_parse_ipv4(ip.c_str(), cfg.ip) || !network_parse_ipv4(mask.c_str(), cfg.mask) ||
          !network_parse_ipv4(gateway.c_str(), cfg.gateway) ||
          (!dns.isEmpty() && !network_parse_ipv4(dns.c_str(), cfg.dns)) || !network_config_valid(cfg)) {
        web_reply(false,"Configuration invalide : verifier les adresses IPv4, le masque et la passerelle dans le meme sous-reseau, differente de l'adresse de l'ecran.");
        return;
      }
    }
    if (!settings_save_network(cfg)) {web_reply(false,"Sauvegarde reseau impossible.");return;}
    String message = cfg.use_static
      ? String("Reseau sauvegarde. Redemarrage... Nouvelle adresse : http://") + wifi_network_address(cfg.ip).toString() + "/"
      : String("DHCP sauvegarde. Redemarrage... Consultez l'adresse attribuee dans WIFI / RESEAU sur l'ecran.");
    web_reply(true,message.c_str(),true);
  });
  config_web.on("/deye", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t serial;
    String host = config_web.arg("host");
    if (host.isEmpty() || host.length()>253 || !web_number(config_web.arg("serial"), UINT32_MAX, serial) || !serial) {web_reply(false,"Logger invalide.");return;}
    settings_save_deye(host, serial); web_reply(true,"Logger sauvegarde. Redemarrage...",true);
  });
  config_web.on("/ntp", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    String tz=config_web.arg("tz"), primary=config_web.arg("primary"), secondary=config_web.arg("secondary");
    if(tz.isEmpty() || primary.isEmpty() || tz.length()>127 || primary.length()>253 || secondary.length()>253) {web_reply(false,"Parametres NTP invalides.");return;}
    settings_save_ntp(tz,primary,secondary);web_reply(true,"Heure sauvegardee. Redemarrage...",true);
  });
  config_web.on("/display", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    uint32_t theme;
    if(!web_number(config_web.arg("theme"),1,theme)) {web_reply(false,"Theme invalide.");return;}
    bool ok=settings_save_tempo(config_web.hasArg("tempo"),config_web.hasArg("colorblind"),config_web.hasArg("ev"));
    if(ok) settings_set_ui_theme(static_cast<UiThemeId>(theme));
    web_reply(ok,ok ? "Affichage sauvegarde. Redemarrage..." : "Sauvegarde impossible.",ok);
  });
  config_web.on("/registers", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    CustomRegisters regs = settings_load_registers();
    for (const auto &field : web_register_fields) {
      String input=config_web.arg(field.name);
      uint8_t *ptr=reinterpret_cast<uint8_t *>(&regs)+field.offset;
      if(field.kind==2) {
        char *end=nullptr; float value=strtof(input.c_str(), &end);
        if(input.isEmpty() || end==input.c_str() || *end || !isfinite(value)) {web_reply(false,"Coefficient invalide.");return;}
        *reinterpret_cast<float *>(ptr)=value;
      } else {
        uint32_t value;
        if(!web_number(input,field.kind==1 ? 60000 : 65535,value)) {web_reply(false,"Registre ou temporisation invalide.");return;}
        if(field.kind==1) *reinterpret_cast<uint32_t *>(ptr)=value;
        else *reinterpret_cast<uint16_t *>(ptr)=value;
      }
    }
    settings_save_registers(regs);
    settings_set_gen_mode(config_web.hasArg("smartload"));
    web_reply(true,"Registres sauvegardes. Redemarrage...",true);
  });
  config_web.on("/update", HTTP_POST, []() {
    if (!web_write_allowed()) return;
    if (!web_upload_allowed) {web_reply(false,"Aucun firmware recu.");return;}
    web_upload_allowed=false;
    web_reply(web_upload_ok,web_upload_ok ? "Mise a jour terminee. Redemarrage..." : "Mise a jour refusee ou interrompue. Firmware actuel conserve.",web_upload_ok);
  }, []() {
    HTTPUpload &upload=config_web.upload();
    if(upload.status==UPLOAD_FILE_START) {
      web_upload_ok=false;
      web_upload_allowed=web_write_allowed();
      if(web_upload_allowed) web_upload_ok=Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
    } else if(upload.status==UPLOAD_FILE_WRITE && web_upload_allowed && web_upload_ok) {
      if(Update.write(upload.buf,upload.currentSize)!=upload.currentSize) {web_upload_ok=false;Update.abort();}
    } else if(upload.status==UPLOAD_FILE_END && web_upload_allowed && web_upload_ok) {
      web_upload_ok=Update.end(true);
    } else if(upload.status==UPLOAD_FILE_ABORTED) {
      if(web_upload_allowed) Update.abort();
      web_upload_ok=false;web_upload_allowed=false;
    }
  });
  config_web.onNotFound([](){if(web_authorized())config_web.send(404,"text/plain","Page introuvable");});
  config_web.begin();
}
static void web_server_process() {
  config_web.handleClient();
  if(web_restart_at && static_cast<int32_t>(millis()-web_restart_at)>=0) ESP.restart();
}
