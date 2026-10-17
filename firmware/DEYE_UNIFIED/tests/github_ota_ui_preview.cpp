#include <lvgl.h>
#include <cassert>
#include <atomic>
#include <string>
#include "../ui_theme.h"
#define LCD_W 480
#define FIRMWARE_VERSION "4.6.0"
struct String : std::string {
 using std::string::string;
 String(const std::string &s):std::string(s){}
 String(int n):std::string(std::to_string(n)){}
};
enum OtaState { OTA_IDLE, OTA_CHECKING, OTA_CURRENT, OTA_READY, OTA_INSTALLING, OTA_DONE, OTA_ERROR };
static std::atomic<int> ota_state{OTA_IDLE},ota_percent{0};
static String ota_version="v4.7.0",ota_error="Transfert interrompu ou fichier invalide. Reessayez.";
static bool ota_busy(){int s=ota_state.load();return s==OTA_CHECKING||s==OTA_INSTALLING||s==OTA_DONE;}
static void ota_start(bool install){ota_state.store(install?OTA_INSTALLING:OTA_CHECKING);}
static UiThemeId cfg_ui_theme=UI_THEME_DARK;
static const UiThemePalette &ui_settings_theme(){return ui_theme_palette(cfg_ui_theme);}
static void ui_show_settings_screen(lv_event_t*){}
static lv_color_t ui_settings_color(uint32_t color) {
  return lv_color_hex(color);
}

static lv_obj_t *ui_settings_make_title(lv_obj_t *parent, const char *text) {
  lv_obj_t *title = lv_label_create(parent);
  lv_label_set_text(title, text);
  lv_obj_set_pos(title, 0, 12);
  lv_obj_set_width(title, LCD_W);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_set_style_text_color(title, ui_settings_color(ui_settings_theme().accent), LV_PART_MAIN);
  return title;
}

static lv_obj_t *ui_settings_make_button(
  lv_obj_t *parent,
  const char *text,
  lv_coord_t x,
  lv_coord_t y,
  lv_coord_t width,
  lv_coord_t height,
  lv_event_cb_t callback
) {
  lv_obj_t *button = lv_btn_create(parent);
  lv_obj_set_size(button, width, height);
  lv_obj_set_pos(button, x, y);
  lv_obj_set_style_bg_color(button, ui_settings_color(ui_settings_theme().accent), LV_PART_MAIN);
  lv_obj_set_style_radius(button, 8, LV_PART_MAIN);
  if (callback != nullptr) {
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
  } else {
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  }

  lv_obj_t *label = lv_label_create(button);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, ui_settings_color(ui_settings_theme().accent_text), LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_center(label);
  return button;
}


#include "../ui_github_ota.h"
static lv_color_t pixels[480*480], draw_buffer[480*40];
static void flush(lv_disp_drv_t *driver,const lv_area_t *area,lv_color_t *colors) {
  for(int y=area->y1;y<=area->y2;++y) for(int x=area->x1;x<=area->x2;++x) pixels[y*480+x]=*colors++;
  lv_disp_flush_ready(driver);
}
static void snapshot(const char *name) {
  lv_obj_update_layout(lv_scr_act()); lv_obj_invalidate(lv_scr_act()); lv_refr_now(nullptr);
  FILE *file=fopen(name,"wb"); assert(file);
  uint8_t header[54]={'B','M'};
  uint32_t size=54+480*480*3,offset=54,dib=40,width=480,height=480;
  memcpy(header+2,&size,4);memcpy(header+10,&offset,4);memcpy(header+14,&dib,4);
  memcpy(header+18,&width,4);memcpy(header+22,&height,4);header[26]=1;header[28]=24;
  fwrite(header,1,54,file);
  for(int y=479;y>=0;--y) for(int x=0;x<480;++x) {
    lv_color32_t c;c.full=lv_color_to32(pixels[y*480+x]);
    uint8_t rgb[]={c.ch.blue,c.ch.green,c.ch.red};fwrite(rgb,1,3,file);
  }
  fclose(file);
}
int main(){
  lv_init(); static lv_disp_draw_buf_t buffer;lv_disp_draw_buf_init(&buffer,draw_buffer,nullptr,480*40);
  static lv_disp_drv_t driver;lv_disp_drv_init(&driver);driver.hor_res=480;driver.ver_res=480;
  driver.draw_buf=&buffer;driver.flush_cb=flush;lv_disp_drv_register(&driver);

 for(auto theme:{UI_THEME_DARK,UI_THEME_LIGHT}) {
  cfg_ui_theme=theme;screen_ota=nullptr;ui_show_ota(nullptr);
  for(int state=OTA_IDLE;state<=OTA_ERROR;++state) {
   ota_state.store(state);ota_percent.store(42);ui_ota_refresh();lv_obj_update_layout(screen_ota);
   assert(lv_obj_has_state(button_ota_install,LV_STATE_DISABLED)==(state!=OTA_READY));
   assert(lv_obj_has_state(button_ota_back,LV_STATE_DISABLED)==ota_busy());
   lv_area_t label,bar;lv_obj_get_coords(label_ota,&label);lv_obj_get_coords(bar_ota,&bar);
   assert(label.y2<bar.y1);assert(label.x2<480);
   if(state==OTA_INSTALLING)snapshot(theme==UI_THEME_DARK?"github-ota-dark.bmp":"github-ota-light.bmp");
  }
 }
 puts("PASS: screen OTA states, buttons, text bounds, dark/light rendering");
}
