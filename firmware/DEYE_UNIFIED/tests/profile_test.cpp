#include <assert.h>
#include <stdio.h>
#include <Preferences.h>
static Preferences preferences;
#include "../inverter_settings.h"
#include "../v2_settings.h"
namespace legacy_v2 {
#include "fixtures/legacy_v2/v2_logic.h"
#include "fixtures/legacy_v2/v2_settings.h"
}

int main() {
  const uint16_t starts[] = {76,520,520,76,76,76,76,520,520,76,76,76,76};
  const uint16_t ends[] = {195,673,675,194,195,195,195,673,675,195,195,195,195};
  const uint16_t gen_daily[] = {62,536,536,0,62,62,62,536,536,62,62,62,62};
  assert(DEYE_PROFILE_COUNT == 13);
  for (unsigned i=0; i<DEYE_PROFILE_COUNT; ++i) {
    cfg_inverter_model=i;
    const auto &p=inverter_profile();
    assert(strlen(p.storage)<=15 && strlen(p.options_storage)<=15);
    assert(deye_profile_index(p.id)==int(i));
    assert(settings_registers_valid(p.registers));
    DeyeReadBlock a,b;
    assert(deye_register_blocks(p.registers,a,b));
    assert(a.start==starts[i] && b.start+b.count-1==ends[i]);
    assert(a.count<=125 && b.count<=125);
    assert(p.ev_supported==(i==0));
    assert(p.gen_daily==gen_daily[i]);
    for(unsigned j=0;j<i;++j) {
      assert(strcmp(p.id,deye_profiles[j].id));
      assert(strcmp(p.storage,deye_profiles[j].storage));
      assert(strcmp(p.options_storage,deye_profiles[j].options_storage));
    }
  }
  assert(deye_profile_index("unknown")==-1);
  assert(!settings_save_model(65535) && !settings_save_model(9));
  assert(deye_grid_connected(1,deye_profiles[0]));
  assert(!deye_grid_connected(4,deye_profiles[0]));
  assert(deye_grid_connected(12,deye_profiles[1]));
  assert(!deye_grid_connected(8,deye_profiles[1]));
  assert(deye_scale_power(5000,-1,deye_profiles[2].battery_power_scale)==-50000);
  for (unsigned i : {3U,4U,5U,6U,10U,11U})
    assert(deye_scale_power(1500,deye_profiles[i].registers.coeff_grid_power,10)==1500);
  cfg_inverter_model=0;
  auto invalid=inverter_profile().registers;
  invalid.pv4_power=500; assert(!settings_registers_valid(invalid));
  invalid=inverter_profile().registers; invalid.battery_soc=DEYE_NO_REGISTER;
  assert(!settings_registers_valid(invalid));
  // v2 migration, then model isolation and restoring modified registers.
  Preferences legacy; legacy.begin("deye-ui",false);
  auto original=inverter_profile().registers; original.gen_power=165;
  legacy.putBytes("regs_v3",&original,offsetof(CustomRegisters,pv4_power));
  assert(settings_load_registers().gen_power==165);
  legacy_v2::V2Config old_options;
  old_options.gen_daily_register=536; // Settings from before the LP1 fix.
  old_options.relay_threshold=1234; old_options.pv_visible[1]=false;
  legacy.begin("deye-v2",false);
  legacy.putBytes("config",&old_options,sizeof(old_options));
  v2_load();
  assert(cfg_v2.relay_threshold==1234 && !cfg_v2.pv_visible[1] && cfg_v2.pv4_visible);
  assert(cfg_v2.gen_daily_register==62 && cfg_v2.gen_daily_scale==0.1f);
  auto custom=settings_load_registers(); custom.gen_power=164;
  assert(settings_save_registers(custom));
  assert(settings_save_model(2)); assert(cfg_inverter_model==0); // deferred until reboot
  settings_load_model(); assert(cfg_inverter_model==2);
  assert(settings_load_registers().gen_power==667);
  custom=settings_load_registers(); custom.block_interval=5000;
  assert(settings_save_registers(custom));
  v2_load(); assert(cfg_v2.relay_register==588 && !cfg_v2.relay_enabled);
  cfg_v2.pv4_visible=false; assert(v2_save(cfg_v2));
  assert(settings_save_model(0)); settings_load_model();
  assert(settings_load_registers().gen_power==164);
  v2_load(); assert(cfg_v2.pv4_visible && cfg_v2.relay_register==184);
  assert(settings_save_model(2)); settings_load_model(); v2_load();
  assert(!cfg_v2.pv4_visible && settings_load_registers().block_interval==5000);
  // Failed flash writes cannot change the live or saved selection.
  Preferences::fail_write=true; assert(!settings_save_model(1));
  assert(!settings_save_registers(custom));
  Preferences::fail_write=false; settings_load_model(); assert(cfg_inverter_model==2);
  // Unknown saved model falls back to the original decoder.
  legacy.begin("deye-v3",false); legacy.putString("model","removed-model");
  settings_load_model(); assert(cfg_inverter_model==0);
  // Real v2 loader/save path: migrate stored R536 and persist the corrected address.
  legacy_v2::v2_load();
  assert(legacy_v2::cfg_v2.gen_daily_register==62);
  assert(legacy_v2::cfg_v2.relay_threshold==1234 && !legacy_v2::cfg_v2.pv_visible[1]);
  legacy.begin("deye-v2",true);
  legacy_v2::V2Config corrected;
  assert(legacy.getBytes("config",&corrected,sizeof(corrected))==sizeof(corrected));
  assert(corrected.gen_daily_register==62);
  for (uint16_t address : {uint16_t(536),uint16_t(0),uint16_t(123)}) {
    corrected.gen_daily_register=address; corrected.gen_daily_scale=0.25f;
    assert(legacy_v2::v2_save(corrected)); legacy_v2::v2_load();
    assert(legacy_v2::cfg_v2.gen_daily_register==(address==536 ? 62 : address));
    assert(legacy_v2::cfg_v2.gen_daily_scale==0.25f);
  }
  // Existing v3 settings: only LP1 replaces R536; preserve custom/estimated sources.
  for (unsigned i=0;i<DEYE_PROFILE_COUNT;++i) {
    if (!deye_profiles[i].available) continue;
    cfg_inverter_model=i;
    V2Config saved; saved.gen_daily_register=536; saved.relay_threshold=1234;
    legacy.begin(inverter_profile().options_storage,false);
    legacy.putBytes("config",&saved,sizeof(saved));
    v2_load();
    const uint16_t expected=gen_daily[i]==62 ? 62 : 536;
    assert(cfg_v2.gen_daily_register==expected && cfg_v2.relay_threshold==1234);
    legacy.getBytes("config",&saved,sizeof(saved));
    assert(saved.gen_daily_register==expected);
    for (uint16_t address : {uint16_t(536),uint16_t(0),uint16_t(123)}) {
      saved.gen_daily_register=address; saved.gen_daily_scale=0.25f;
      assert(v2_save(saved)); v2_load();
      assert(cfg_v2.gen_daily_register==(address==536 ? expected : address));
      assert(cfg_v2.gen_daily_scale==0.25f);
    }
  }
  // A failed migration write still uses R62 in RAM and retries on next boot.
  cfg_inverter_model=0;
  V2Config pending; pending.gen_daily_register=536;
  legacy.begin(inverter_profile().options_storage,false);
  legacy.putBytes("config",&pending,sizeof(pending));
  Preferences::fail_write=true; v2_load();
  assert(cfg_v2.gen_daily_register==62);
  Preferences::fail_write=false; v2_load();
  legacy.getBytes("config",&pending,sizeof(pending));
  assert(pending.gen_daily_register==62);
  // AI-W5.1: the verified standalone P1 map replaces all old saved addresses.
  cfg_inverter_model=deye_profile_index("ai-w5.1-ess");
  const auto &ai=inverter_profile();
  assert(!ai.experimental && !ai.gen_supported && !ai.ev_supported && ai.pv_count==2);
  assert(deye_grid_connected(1,ai) && !deye_grid_connected(0,ai) && !deye_grid_connected(4,ai));
  const uint16_t expected_ai[]={186,187,65535,108,184,183,190,182,169,194,76,77,178,65535,84,90,91,65535,0};
  assert(memcmp(&ai.registers,expected_ai,sizeof(expected_ai))==0);
  DeyeReadBlock ai_one,ai_two;
  assert(deye_register_blocks(ai.registers,ai_one,ai_two));
  assert(ai_one.start==76 && ai_one.count==33 && ai_two.start==169 && ai_two.count==26);
  CustomRegisters old_ai{672,673,DEYE_NO_REGISTER,529,588,587,590,586,607,552,520,521,637,DEYE_NO_REGISTER,526,540,541,DEYE_NO_REGISTER,0,
    10000,10000,7000,4500,0.1f,0.1f,1,-1};
  legacy.begin(ai.storage,false); legacy.putBytes("registers",&old_ai,sizeof(old_ai));
  Preferences::fail_write=true;
  auto migrated=settings_load_registers();
  assert(memcmp(&migrated,expected_ai,sizeof(expected_ai))==0 && migrated.block_interval==4500);
  Preferences::fail_write=false;
  migrated=settings_load_registers();
  migrated.grid_power=172; assert(settings_save_registers(migrated));
  assert(settings_load_registers().grid_power==172); // No repeated migration.
  // The earlier loop populated config_p1; clear only that AI options namespace.
  legacy.begin(ai.options_storage,false);
  Preferences::records[ai.options_storage].erase("config_p1");
  V2Config old_ai_options; old_ai_options.relay_register=588; old_ai_options.wake_seconds=120;
  legacy.putBytes("config",&old_ai_options,sizeof(old_ai_options));
  Preferences::fail_write=true; v2_load();
  assert(cfg_v2.relay_register==184 && cfg_v2.wake_seconds==120);
  Preferences::fail_write=false; v2_load();
  cfg_v2.relay_register=190; assert(v2_save(cfg_v2));
  v2_load(); assert(cfg_v2.relay_register==190);
  cfg_inverter_model=2; // HP3 persisted settings and bitmask remain intact.
  assert(settings_load_registers().block_interval==5000 && deye_grid_connected(4,inverter_profile()));
  puts("PASS: profiles, blocks, scaling, v2/v3 GEN migration, model isolation and failed writes");
}
