param([string]$Zig = 'zig',
  [string]$LibraryDir = (Join-Path $PSScriptRoot '..\..\..\.pio\libdeps\deye_unified'))
$ErrorActionPreference='Stop'
$source=[IO.File]::ReadAllText((Join-Path $PSScriptRoot '..\ui_settings.h'))
function Slice-Source([string]$startText,[string]$endText) {
  $start=$source.IndexOf($startText);$end=$source.IndexOf($endText,$start)
  if($start -lt 0 -or $end -le $start) {throw 'Production UI section missing.'}
  return $source.Substring($start,$end-$start)
}
$helpers=Slice-Source 'static lv_color_t ui_settings_color(' 'static void ui_settings_restart_message('
$helpers+=Slice-Source 'static lv_obj_t *ui_settings_make_textarea(' '// ==================== FONCTIONS DE NAVIGATION'
$helpers+=Slice-Source 'static void ui_settings_update_deye_status() {' 'static void ui_settings_create() {'
$page=$source.Substring($source.IndexOf('  screen_deye = lv_obj_create(nullptr);')).TrimEnd()
$helpers+='static void create_logger_page() { lv_obj_t *label;' + [Environment]::NewLine + $page
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'logger_ui_under_test.h'),$helpers)
$lvgl=Join-Path $LibraryDir 'lvgl'
$config=(Join-Path $PSScriptRoot 'preview_lv_conf.h').Replace('\','/')
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $env:TEMP 'deye-v3-435-zig-cache'
$arguments=@('cc','-O1','-UNDEBUG',('-DLV_CONF_PATH='+$config),('-I'+$lvgl),('-I'+(Join-Path $PSScriptRoot 'compat')))
$arguments+=@(Get-ChildItem -LiteralPath (Join-Path $lvgl 'src') -Filter '*.c' -Recurse | ForEach-Object FullName)
$arguments+=@((Join-Path $PSScriptRoot 'logger_ui_preview.cpp'),'-o',(Join-Path $PSScriptRoot 'logger_ui_preview.exe'))
& $Zig @arguments
if($LASTEXITCODE -ne 0) {throw 'Logger preview compilation failed.'}
Push-Location $PSScriptRoot
try { & '.\logger_ui_preview.exe';if($LASTEXITCODE -ne 0) {throw 'Logger preview assertions failed.'} }
finally { Pop-Location }
