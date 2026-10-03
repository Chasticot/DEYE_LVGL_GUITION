#pragma once
#include <ArduinoJson.h>
#include "ev_backend.h"

// Legacy ev_enabled meant LoRa in V3 and WB01 in the Vetronic edition.
// An enabled legacy export cannot safely identify which system to activate.
static bool web_ev_backend_parse(JsonObjectConst object, bool native_supported,
                                 EvBackend &backend, bool &legacy_ambiguous) {
  legacy_ambiguous = false;
  if (object.isNull() || !object["ev_enabled"].is<bool>()) return false;
  const bool enabled = object["ev_enabled"].as<bool>();
  if (!object.containsKey("ev_backend")) {
    if (enabled) { legacy_ambiguous = true; return false; }
    backend = EvBackend::None;
    return true;
  }
  if (!object["ev_backend"].is<const char *>() ||
      !ev_backend_parse(object["ev_backend"].as<const char *>(), backend) ||
      !ev_backend_supported(backend, native_supported)) return false;
  return enabled == (backend != EvBackend::None);
}

struct V2WebField { const char *key; const char *label; size_t offset; uint8_t kind; };
// kind: bool, uint16, float, uint8, local HH:MM
#define V2_WEB(field, label, kind) {#field, label, offsetof(V2Config, field), kind}
static const V2WebField v2_web_fields[] = {
  {"pv1", "Afficher PV1", offsetof(V2Config, pv_visible), 0},
  {"pv2", "Afficher PV2", offsetof(V2Config, pv_visible) + 1, 0},
  {"pv3", "Afficher PV3", offsetof(V2Config, pv_visible) + 2, 0},
  V2_WEB(pv4_visible, "Afficher PV4", 0),
  V2_WEB(add_gen, "Cumuler GEN MO avec PV", 0),
  V2_WEB(gen_daily_register, "Registre energie GEN du jour", 1),
  V2_WEB(gen_daily_scale, "Coefficient GEN kWh", 2),
  V2_WEB(tariff_mode, "Heures creuses", 3),
  {"hc1_start", "Debut HC 1", offsetof(V2Config, hc_start), 4},
  {"hc1_end", "Fin HC 1", offsetof(V2Config, hc_end), 4},
  {"hc2_start", "Debut HC 2", offsetof(V2Config, hc_start) + sizeof(uint16_t), 4},
  {"hc2_end", "Fin HC 2", offsetof(V2Config, hc_end) + sizeof(uint16_t), 4},
  V2_WEB(ev_tariff_enabled, "Conditionner la recharge VE aux tarifs", 0),
  V2_WEB(ev_hc_only, "Recharge reseau en HC seulement", 0),
  V2_WEB(ev_block_red_hp, "Interdire le reseau en HP rouge Tempo", 0),
  V2_WEB(relay_enabled, "Activer le relais", 0),
  V2_WEB(relay_register, "Registre du relais", 1),
  V2_WEB(relay_signed, "Registre signe", 0),
  V2_WEB(relay_coefficient, "Coefficient du relais", 2),
  V2_WEB(relay_comparator, "ON si (registre x coefficient) compare au seuil ; OFF sinon", 3),
  V2_WEB(relay_threshold, "Seuil du relais", 2),
  V2_WEB(relay_delay_s, "Delai de commutation (s)", 1),
  V2_WEB(sleep_enabled, "Activer la veille horaire", 0),
  V2_WEB(sleep_start, "Debut de veille", 4),
  V2_WEB(sleep_end, "Fin de veille", 4),
  V2_WEB(wake_seconds, "Duree de reveil tactile (s)", 1)
};
#undef V2_WEB
static bool web_v2_is_gen_daily(const V2WebField &field) {
  return strcmp(field.key, "gen_daily_register") == 0 || strcmp(field.key, "gen_daily_scale") == 0;
}
static String web_v2_json() {
  StaticJsonDocument<2048> doc;
  for (const auto &field : v2_web_fields) {
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&cfg_v2) + field.offset;
    if (field.kind == 0) doc[field.key] = *reinterpret_cast<const bool *>(p);
    else if (field.kind == 2) doc[field.key] = *reinterpret_cast<const float *>(p);
    else doc[field.key] = field.kind == 3 ? *p : *reinterpret_cast<const uint16_t *>(p);
  }
  String result; serializeJson(doc, result); return result;
}
static bool web_v2_parse(JsonObjectConst object, V2Config &config) {
  if (object.isNull()) return false;
  for (const auto &field : v2_web_fields) {
    if (strcmp(field.key, "pv4_visible") == 0 && !object.containsKey(field.key)) continue;
    const JsonVariantConst value = object[field.key];
    uint8_t *p = reinterpret_cast<uint8_t *>(&config) + field.offset;
    if (field.kind == 0) {
      if (!value.is<bool>()) return false;
      *reinterpret_cast<bool *>(p) = value.as<bool>();
    } else {
      if (!value.is<double>()) return false;
      const double number = value.as<double>();
      if (!isfinite(number)) return false;
      if (field.kind == 2) *reinterpret_cast<float *>(p) = number;
      else {
        if (number < 0 || number > (field.kind == 3 ? 255 : 65535) || floor(number) != number) return false;
        if (field.kind == 3) *p = uint8_t(number);
        else *reinterpret_cast<uint16_t *>(p) = uint16_t(number);
      }
    }
  }
  return v2_config_valid(config);
}
#ifndef DEYE_V2_CONFIG_TEST
static String web_v2_form() {
  String page = web_form("/v2", "PV / GEN / Tarifs VE / Relais / Veille");
  page += web_check("smartload", "Mode SmartLoad (decoche : GEN MO)", settings_get_gen_mode());
  for (const auto &field : v2_web_fields) {
    if (web_v2_is_gen_daily(field)) continue;
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&cfg_v2) + field.offset;
    if (field.kind == 0) { page += web_check(field.key, field.label, *reinterpret_cast<const bool *>(p)); continue; }
    if (field.kind == 3) {
      const bool tariff = strcmp(field.key, "tariff_mode") == 0;
      const char *labels[] = {tariff ? "2 plages personnelles" : "&lt;", tariff ? "Tempo" : "=", "&gt;"};
      page += String("<label>") + field.label + "<select name='" + field.key + "'>";
      for (uint8_t i = 0; i < (tariff ? 2 : 3); ++i)
        page += String("<option value='") + i + "'" + (i == *p ? " selected" : "") + ">" + labels[i] + "</option>";
      page += "</select></label>";
    } else if (field.kind == 4) {
      const unsigned minute = *reinterpret_cast<const uint16_t *>(p);
      char value[6]; snprintf(value, sizeof(value), "%02u:%02u", minute / 60, minute % 60);
      page += web_input(field.key, field.label, value, "time");
    } else page += web_input(field.key, field.label,
      field.kind == 2 ? String(*reinterpret_cast<const float *>(p), 6) : String(*reinterpret_cast<const uint16_t *>(p)));
  }
  return page + WEB_FORM_END;
}
static void web_v2_post() {
  if (!web_write_allowed()) return;
  StaticJsonDocument<2048> doc;
  for (const auto &field : v2_web_fields) {
    if (web_v2_is_gen_daily(field)) {
      if (field.kind == 2) doc[field.key] = cfg_v2.gen_daily_scale;
      else doc[field.key] = cfg_v2.gen_daily_register;
      continue;
    }
    if (field.kind == 0) { doc[field.key] = config_web.hasArg(field.key); continue; }
    const String text = config_web.arg(field.key);
    if (field.kind == 4) {
      if (text.length() != 5 || text[2] != ':' || !isDigit(text[0]) || !isDigit(text[1]) ||
          !isDigit(text[3]) || !isDigit(text[4]) || text.substring(0, 2).toInt() > 23 || text.substring(3).toInt() > 59) {
        web_reply(false, "Horaire invalide."); return;
      }
      doc[field.key] = text.substring(0, 2).toInt() * 60 + text.substring(3).toInt();
    } else {
      char *end = nullptr;
      const double value = strtod(text.c_str(), &end);
      if (text.isEmpty() || end == text.c_str() || *end || !isfinite(value)) { web_reply(false, "Valeur invalide."); return; }
      doc[field.key] = value;
    }
  }
  V2Config config = cfg_v2;
  if (!web_v2_parse(doc.as<JsonObjectConst>(), config)) { web_reply(false, "Reglages hors limites."); return; }
  const bool ok = v2_save(config) && settings_set_gen_mode(config_web.hasArg("smartload"));
  web_reply(ok, ok ? "Reglages v2 enregistres. Redemarrage..." : "Sauvegarde impossible.", ok);
}
#endif
