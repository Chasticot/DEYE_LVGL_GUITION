#pragma once
#include <atomic>
#include <algorithm>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <mbedtls/sha256.h>
#include "ota_ca.h"

// Only the unified screen application is accepted; never USB/bootloader images.
enum OtaState { OTA_IDLE, OTA_CHECKING, OTA_CURRENT, OTA_READY, OTA_INSTALLING, OTA_DONE, OTA_ERROR };
static std::atomic<int> ota_state{OTA_IDLE}, ota_percent{0};
static String ota_version, ota_url, ota_digest, ota_error;
static size_t ota_size = 0;
static bool ota_busy() { int s=ota_state.load(); return s==OTA_CHECKING || s==OTA_INSTALLING || s==OTA_DONE; }
#include "ota_version.h"
static void ota_fail(const char *message) { ota_error=message; ota_state.store(OTA_ERROR); }
static bool ota_http(HTTPClient &http, WiFiClientSecure &client, const String &url) {
  client.setCACert(OTA_ROOT_CA);
  client.setHandshakeTimeout(15);
  http.setTimeout(15000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if(!http.begin(client,url)) return false;
  http.addHeader("User-Agent","Deye-Monitor");
  http.useHTTP10(true);
  return true;
}
static void ota_worker(void *arg) {
  const bool install = arg != nullptr;
  do {
    if(WiFi.status()!=WL_CONNECTED) { ota_fail("Connectez l'ecran au Wi-Fi puis reessayez."); break; }
    if(time(nullptr)<1700000000) { ota_fail("Synchronisez l'heure dans RESEAU / HEURE puis reessayez."); break; }
    WiFiClientSecure client; HTTPClient http;
    if(!install) {
      ota_url=""; ota_size=0; ota_digest="";
      if(!ota_http(http,client,"https://api.github.com/repos/Chasticot/DEYE_LVGL_GUITION/releases/latest") || http.GET()!=200) {
        ota_fail("GitHub indisponible. Reessayez dans quelques minutes."); break;
      }
      // Filter avoids retaining release notes and unrelated assets in RAM.
      StaticJsonDocument<512> filter;
      filter["tag_name"]=true; filter["draft"]=true; filter["prerelease"]=true;
      for(const char *key : {"name","browser_download_url","size","digest"}) filter["assets"][0][key]=true;
      DynamicJsonDocument doc(24576);
      auto err=deserializeJson(doc,http.getStream(),DeserializationOption::Filter(filter));
      http.end();
      if(err || doc["draft"].as<bool>() || doc["prerelease"].as<bool>()) { ota_fail("Reponse GitHub invalide."); break; }
      ota_version=doc["tag_name"].as<String>();
      unsigned ma,mi,pa; int consumed=0; String parsed=ota_version;
      if(parsed.startsWith("v")) parsed.remove(0,1);
      if(sscanf(parsed.c_str(),"%u.%u.%u%n",&ma,&mi,&pa,&consumed)!=3 || consumed!=(int)parsed.length()) {
        ota_fail("Version publiee non reconnue. Consultez GitHub."); break;
      }
      if(!ota_newer(ota_version)) { ota_state.store(OTA_CURRENT); break; }
      String number=ota_version; if(number.startsWith("v")) number.remove(0,1);
      String expected="DEYE_V3_"+number+"_OTA.bin";
      String unified="DEYE_UNIFIED_"+number+"_OTA.bin";
      for(JsonObject asset:doc["assets"].as<JsonArray>()) {
        String name=asset["name"].as<String>();
        if(name!=expected && name!=unified) continue;
        if(!ota_url.isEmpty()) { ota_url=""; break; } // Ambiguous release: refuse to guess.
        ota_url=asset["browser_download_url"].as<String>();
        ota_digest=asset["digest"].as<String>(); ota_size=asset["size"].as<size_t>();
      }
      String prefix="https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/"+ota_version+"/";
      if(!ota_url.startsWith(prefix) || !ota_size || ota_size>ESP.getFreeSketchSpace() ||
         !ota_digest.startsWith("sha256:") || ota_digest.length()!=71) {
        ota_fail("Aucun firmware compatible et verifiable dans cette version."); break;
      }
      ota_state.store(OTA_READY); break;
    }
    if(!ota_http(http,client,ota_url) || http.GET()!=200 || http.getSize()!=(int)ota_size) {
      ota_fail("Telechargement impossible. Firmware actuel conserve."); break;
    }
    if(!Update.begin(ota_size,U_FLASH)) { ota_fail("Espace de mise a jour indisponible."); break; }
    mbedtls_sha256_context sha; mbedtls_sha256_init(&sha); mbedtls_sha256_starts_ret(&sha,0);
    uint8_t buffer[2048], digest[32]; size_t received=0; uint32_t last=millis(); bool ok=true;
    auto stream=http.getStreamPtr();
    while(received<ota_size) {
      int available=stream->available();
      if(available>0) {
        size_t count=std::min(sizeof(buffer),std::min((size_t)available,ota_size-received));
        int got=stream->read(buffer,count);
        if(got<=0 || Update.write(buffer,got)!=(size_t)got) { ok=false; break; }
        mbedtls_sha256_update_ret(&sha,buffer,got); received+=got; last=millis();
        ota_percent.store(received*100/ota_size);
      } else if(!http.connected() || millis()-last>20000) { ok=false; break; }
      vTaskDelay(1);
    }
    mbedtls_sha256_finish_ret(&sha,digest); mbedtls_sha256_free(&sha); http.end();
    char hex[65]; for(int i=0;i<32;++i) snprintf(hex+2*i,3,"%02x",digest[i]);
    if(!ok || received!=ota_size || ota_digest.substring(7)!=hex) {
      Update.abort(); ota_fail("Transfert interrompu ou fichier invalide. Reessayez."); break;
    }
    if(!Update.end()) { Update.abort(); ota_fail("Installation refusee. Firmware actuel conserve."); break; }
    ota_state.store(OTA_DONE);
  } while(false);
  vTaskDelete(nullptr);
}
static void ota_start(bool install) {
  if(ota_busy() || Update.isRunning() || (install && ota_state.load()!=OTA_READY)) return;
  ota_percent.store(0); ota_state.store(install?OTA_INSTALLING:OTA_CHECKING);
  if(xTaskCreate(ota_worker,"github_ota",12288,install?(void*)1:nullptr,1,nullptr)!=pdPASS)
    ota_fail("Memoire insuffisante. Redemarrez puis reessayez.");
}
