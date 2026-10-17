#pragma once
static lv_obj_t *screen_ota=nullptr, *label_ota=nullptr, *bar_ota=nullptr;
static lv_obj_t *button_ota_check=nullptr, *button_ota_install=nullptr, *button_ota_back=nullptr;
static void ui_ota_refresh() {
  if(!screen_ota) return;
  int state=ota_state.load();
  String text="Version installee : " FIRMWARE_VERSION "\n\n";
  switch(state) {
    case OTA_CHECKING: text+="Recherche sur GitHub..."; break;
    case OTA_CURRENT: text+="Votre ecran est a jour."; break;
    case OTA_READY: text+="Nouvelle version : "+ota_version+"\nPrete a installer."; break;
    case OTA_INSTALLING: text+="Installation : "+String(ota_percent.load())+" %\nGardez l'ecran allume."; break;
    case OTA_DONE: text+="Mise a jour terminee !\nRedemarrage..."; break;
    case OTA_ERROR: text+=ota_error; break;
    default: text+="Verifiez si une mise a jour est disponible.";
  }
  lv_label_set_text(label_ota,text.c_str());
  lv_bar_set_value(bar_ota,ota_percent.load(),LV_ANIM_OFF);
  for(auto button:{button_ota_check,button_ota_back}) {
    if(ota_busy()) lv_obj_add_state(button,LV_STATE_DISABLED); else lv_obj_clear_state(button,LV_STATE_DISABLED);
  }
  if(state==OTA_READY) lv_obj_clear_state(button_ota_install,LV_STATE_DISABLED);
  else lv_obj_add_state(button_ota_install,LV_STATE_DISABLED);
}
static void ui_ota_confirm(lv_event_t *e) {
  lv_obj_t *box=lv_event_get_current_target(e);
  if(lv_msgbox_get_active_btn(box)==0) ota_start(true);
  lv_msgbox_close(box);
}
static void ui_show_ota(lv_event_t *) {
  if(!screen_ota) {
    screen_ota=lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_ota,ui_settings_color(ui_settings_theme().screen_bg),0);
    lv_obj_set_style_text_color(screen_ota,ui_settings_color(ui_settings_theme().text),0);
    lv_obj_clear_flag(screen_ota,LV_OBJ_FLAG_SCROLLABLE);
    ui_settings_make_title(screen_ota,"MISE A JOUR");
    label_ota=lv_label_create(screen_ota); lv_obj_set_pos(label_ota,25,65); lv_obj_set_width(label_ota,430);
    lv_obj_set_style_text_align(label_ota,LV_TEXT_ALIGN_CENTER,0);
    bar_ota=lv_bar_create(screen_ota); lv_obj_set_pos(bar_ota,35,225); lv_obj_set_size(bar_ota,410,18);
    button_ota_check=ui_settings_make_button(screen_ota,"VERIFIER SUR GITHUB",35,265,410,45,[](lv_event_t *){ota_start(false);ui_ota_refresh();});
    button_ota_install=ui_settings_make_button(screen_ota,"INSTALLER",35,322,410,45,[](lv_event_t *){
      static const char *choices[]={"Installer","Annuler",""};
      lv_obj_t *box=lv_msgbox_create(nullptr,"Installer la mise a jour ?","Les reglages seront conserves.\nGardez l'ecran alimente jusqu'au redemarrage.",choices,false);
      lv_obj_add_event_cb(box,ui_ota_confirm,LV_EVENT_VALUE_CHANGED,nullptr); lv_obj_center(box);
    });
    button_ota_back=ui_settings_make_button(screen_ota,"RETOUR",35,405,410,45,ui_show_settings_screen);
  }
  for(auto button:{button_ota_check,button_ota_install,button_ota_back}) {
    lv_obj_set_style_bg_color(button,ui_settings_color(ui_settings_theme().card_bg),LV_STATE_DISABLED);
    lv_obj_set_style_text_color(button,ui_settings_color(ui_settings_theme().muted_text),LV_STATE_DISABLED);
  }
  ui_ota_refresh(); lv_scr_load(screen_ota);
}
