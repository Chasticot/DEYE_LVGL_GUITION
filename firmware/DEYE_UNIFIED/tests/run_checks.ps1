param(
  [string]$Zig = 'zig',
  [string]$Node = 'node',
  [switch]$SkipPreview
)
$ErrorActionPreference = 'Stop'
$testDir = $PSScriptRoot
$projectDir = (Resolve-Path (Join-Path $testDir '..\..\..')).Path
$env:ZIG_GLOBAL_CACHE_DIR = Join-Path $env:TEMP 'deye-v3-435-zig-cache'
$includes = @(('-I' + (Join-Path $testDir 'nvs_compat')), ('-I' + (Join-Path $testDir 'compat')), ('-I' + (Join-Path $projectDir '.pio\libdeps\deye_unified\ArduinoJson\src')))
foreach ($test in @('profile_test', 'backend_test', 'logic_test', 'deye_tariff_test', 'config_json_test', 'vetronic_test', 'vetronic_json_test')) {
  $output = Join-Path $testDir ($test + '.exe')
  & $Zig c++ -std=c++17 -O1 @includes (Join-Path $testDir ($test + '.cpp')) -o $output
  if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $test" }
  & $output
  if ($LASTEXITCODE -ne 0) { throw "Test failed: $test" }
}
& $Node (Join-Path $testDir 'web_test.cjs')
if ($LASTEXITCODE -ne 0) { throw 'Production Web tests failed' }
if ($SkipPreview) { return }
$lvgl = Join-Path $projectDir '.pio\libdeps\deye_unified\lvgl'
$sources = @(Get-ChildItem -LiteralPath (Join-Path $lvgl 'src') -Filter '*.c' -Recurse | ForEach-Object FullName)
$configPath = (Join-Path $testDir 'preview_lv_conf.h').Replace('\', '/')
$arguments = @('cc', '-O1', ('-DLV_CONF_PATH=' + $configPath), ('-I' + $lvgl), ('-I' + (Join-Path $testDir 'compat')))
$arguments += $sources
$arguments += @((Join-Path $testDir 'ui_preview.cpp'), '-o', (Join-Path $testDir 'ui_preview.exe'))
& $Zig @arguments
if ($LASTEXITCODE -ne 0) { throw 'Preview compilation failed' }
Push-Location $testDir
try {
  & (Join-Path $testDir 'ui_preview.exe')
  if ($LASTEXITCODE -ne 0) { throw 'Preview checks failed' }
} finally { Pop-Location }
