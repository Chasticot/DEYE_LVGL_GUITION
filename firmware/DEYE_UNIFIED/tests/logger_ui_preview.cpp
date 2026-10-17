#include <lvgl.h>
#include <assert.h>
#include "../ui_theme.h"
#include "../logger_discovery_logic.h"
#include "../logger_connection.h"
#define LCD_W 480
struct String {
  char text[64];
  String(const char *value) { snprintf(text,sizeof(text),"%s",value); }
  String(unsigned value) { snprintf(text,sizeof(text),"%u",value); }
  const char *c_str() const { return text; }
};
static UiThemeId cfg_ui_theme = UI_THEME_DARK;
static const UiThemePalette &ui_settings_theme() { return ui_theme_palette(cfg_ui_theme); }
static lv_obj_t *screen_deye, *textarea_deye_host, *textarea_logger_serial, *keyboard_deye,
  *label_deye_blocks, *label_deye_discovery;
static lv_obj_t *dropdown_deye_transport, *textarea_deye_port, *textarea_deye_unit, *button_deye_discovery;
static lv_obj_t *button_deye_results,*screen_logger_candidates,*dropdown_logger_candidates,*label_logger_candidates;
static lv_obj_t *screen_ntp, *screen_manual_clock, *textarea_manual_date, *textarea_manual_time, *keyboard_manual_clock,
  *dropdown_timezone, *textarea_ntp_primary, *textarea_ntp_secondary, *keyboard_ntp;
static const char *timezone_names = "Europe/Paris\nUTC";
static String cfg_ntp_primary = "pool.ntp.org", cfg_ntp_secondary = "time.google.com";
static DeyeTransport cfg_deye_transport = DeyeTransport::SolarmanV5;
static LoggerModel cfg_logger_model=LoggerModel::Lsw;
static String cfg_logger_identity="4294967295";
static uint16_t cfg_deye_port = 8899;
static uint8_t cfg_deye_unit = 1;
static uint32_t ui_deye_discovery_applied_generation;
static String cfg_deye_host = "192.168.1.12";
static uint32_t cfg_logger_serial = 2712345678UL;
static uint32_t millis() { return lv_tick_get(); }
struct DeyeDiagnostics { uint32_t block_last_success_ms[3] = {}, block_success[3] = {}, block_failure[3] = {}; };
static bool deye_copy_diagnostics(DeyeDiagnostics *out) { *out={}; return true; }
static DeyeLoggerDiscoveryStatus discovery;
static bool deye_logger_discovery_copy_status(DeyeLoggerDiscoveryStatus *out) { *out=discovery; return true; }
static void ui_discover_deye_logger(lv_event_t *) {}
static void ui_show_v2_deye(lv_event_t *) {} static void ui_save_deye(lv_event_t *) {}
static void ui_show_deye_screen(lv_event_t *) {}
static void ui_textarea_event(lv_event_t *) {}
static void ui_save_ntp(lv_event_t *) {} static void ui_show_v2_network(lv_event_t *) {}
static void ui_show_manual_clock(lv_event_t *) {} static void ui_show_ntp_screen(lv_event_t *) {}
static void ui_apply_manual_clock(lv_event_t *) {}
#include "logger_ui_under_test.h"
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
int main() {
  lv_init(); static lv_disp_draw_buf_t buffer;lv_disp_draw_buf_init(&buffer,draw_buffer,nullptr,480*40);
  static lv_disp_drv_t driver;lv_disp_drv_init(&driver);driver.hor_res=480;driver.ver_res=480;
  driver.draw_buf=&buffer;driver.flush_cb=flush;lv_disp_drv_register(&driver);
  const UiThemeId themes[] = {UI_THEME_DARK,UI_THEME_LIGHT};
  const DeyeLoggerDiscoveryState states[] = {DEYE_LOGGER_DISCOVERY_IDLE,DEYE_LOGGER_DISCOVERY_QUEUED,
    DEYE_LOGGER_DISCOVERY_RUNNING,DEYE_LOGGER_DISCOVERY_FOUND,DEYE_LOGGER_DISCOVERY_NOT_FOUND,DEYE_LOGGER_DISCOVERY_ERROR};
  for(auto theme : themes) {
    cfg_ui_theme=theme;keyboard_deye=nullptr;ui_deye_discovery_applied_generation=0;
    discovery={};create_logger_page();lv_scr_load(screen_deye);
    for(auto state : states) {
      discovery.state=state;discovery.generation=1;discovery.serial=UINT32_MAX;
      strcpy(discovery.identity,"4294967295");
      snprintf(discovery.host,sizeof(discovery.host),"192.168.255.254");
      discovery.scanned_hosts=510;discovery.total_hosts=510;
      discovery.phase=DEYE_LOGGER_PHASE_BROADCAST;
      ui_settings_update_deye_status();lv_obj_update_layout(screen_deye);
      lv_area_t diagnostic,status;lv_obj_get_coords(label_deye_blocks,&diagnostic);lv_obj_get_coords(label_deye_discovery,&status);
      if (!(diagnostic.y2 < status.y1 && status.y2 < 378 && status.x2 < 480)) {
        printf("Layout state %d: diagnostics %d-%d, status %d-%d x2 %d\n",int(state),diagnostic.y1,diagnostic.y2,status.y1,status.y2,status.x2);
        snapshot("logger-layout-check.bmp"); fflush(stdout);
      }
      assert(diagnostic.y2 < status.y1 && status.y2 < 378 && status.x2 < 480);
      if(state==DEYE_LOGGER_DISCOVERY_FOUND) {
        assert(!strcmp(lv_textarea_get_text(textarea_deye_host),discovery.host));
        assert(!strcmp(lv_textarea_get_text(textarea_logger_serial),"4294967295"));
        snapshot(theme==UI_THEME_DARK ? "logger-found-dark.bmp" : "logger-found-light.bmp");
        lv_textarea_set_text(textarea_deye_host,"user-edit");ui_settings_update_deye_status();
        assert(!strcmp(lv_textarea_get_text(textarea_deye_host),"user-edit"));
      }
    }
    discovery.automatic=true;discovery.state=DEYE_LOGGER_DISCOVERY_RUNNING;
    discovery.phase=DEYE_LOGGER_PHASE_BROADCAST;
    ui_settings_update_deye_status();lv_obj_update_layout(screen_deye);
    lv_area_t automatic;lv_obj_get_coords(label_deye_discovery,&automatic);
    assert(automatic.y2 < 378 && automatic.x2 < 480);
    discovery.state=DEYE_LOGGER_DISCOVERY_FOUND;
    discovery.restart_pending=true;
    ui_settings_update_deye_status();lv_obj_update_layout(screen_deye);
    lv_obj_get_coords(label_deye_discovery,&automatic);
    assert(automatic.y2 < 378);
    snapshot(theme==UI_THEME_DARK ? "logger-auto-dark.bmp" : "logger-auto-light.bmp");
    discovery.restart_pending=false;discovery.save_failed=true;
    ui_settings_update_deye_status();lv_obj_update_layout(screen_deye);
    lv_obj_get_coords(label_deye_discovery,&automatic);
    assert(automatic.y2 < 378);
    lv_dropdown_set_selected(dropdown_deye_transport,1);
    lv_event_send(dropdown_deye_transport,LV_EVENT_VALUE_CHANGED,nullptr);
    ui_settings_update_deye_status();
    assert(!strcmp(lv_textarea_get_text(textarea_deye_port),"8899"));
    assert(!lv_obj_has_state(button_deye_discovery,LV_STATE_DISABLED));
    assert(!lv_obj_has_state(textarea_logger_serial,LV_STATE_DISABLED));
    snapshot(theme==UI_THEME_DARK ? "logger-lse-dark.bmp" : "logger-lse-light.bmp");
    lv_dropdown_set_selected(dropdown_deye_transport,2);lv_event_send(dropdown_deye_transport,LV_EVENT_VALUE_CHANGED,nullptr);
    assert(!strcmp(lv_textarea_get_text(textarea_deye_port),"502"));
    lv_textarea_set_text(textarea_logger_serial,"D123456789012");
    discovery={};discovery.model=LoggerModel::Dl1000B;discovery.port=502;discovery.unit=1;
    discovery.state=DEYE_LOGGER_DISCOVERY_CANDIDATES;discovery.candidate_count=2;
    strcpy(discovery.identity,"D123456789012");strcpy(discovery.candidates[0],"192.168.1.47");strcpy(discovery.candidates[1],"192.168.1.48");
    ui_settings_update_deye_status();assert(!lv_obj_has_state(button_deye_results,LV_STATE_DISABLED));
    snapshot(theme==UI_THEME_DARK ? "logger-dl1000b-dark.bmp" : "logger-dl1000b-light.bmp");
    ui_show_logger_candidates(nullptr);assert(lv_scr_act()==screen_logger_candidates);
    snapshot(theme==UI_THEME_DARK ? "logger-candidates-dark.bmp" : "logger-candidates-light.bmp");
    lv_dropdown_set_selected(dropdown_logger_candidates,1);ui_use_logger_candidate(nullptr);
    assert(!strcmp(lv_textarea_get_text(textarea_deye_host),"192.168.1.48"));
    lv_textarea_set_text(textarea_logger_serial,"D999");ui_settings_update_deye_status();
    assert(lv_obj_has_state(button_deye_results,LV_STATE_DISABLED));
    lv_scr_load(screen_ntp); snapshot(theme==UI_THEME_DARK ? "clock-ntp-dark.bmp" : "clock-ntp-light.bmp");
    lv_textarea_set_text(textarea_manual_date,"2026-10-17");
    lv_textarea_set_text(textarea_manual_time,"14:25");
    lv_scr_load(screen_manual_clock);
    snapshot(theme==UI_THEME_DARK ? "clock-manual-dark.bmp" : "clock-manual-light.bmp");
  }
  puts("Production logger page layout and one-time field filling passed in both themes.");
}
