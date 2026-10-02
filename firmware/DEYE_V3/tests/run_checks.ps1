param([string]$Zig = 'zig')
$ErrorActionPreference = 'Stop'
$testDir = $PSScriptRoot
$projectDir = (Resolve-Path (Join-Path $testDir '..\..\..')).Path
$env:ZIG_GLOBAL_CACHE_DIR = Join-Path $env:TEMP 'deye-v2-zig-cache'
& $Zig c++ -std=c++17 -O1 ('-I' + (Join-Path $testDir 'nvs_compat')) ('-I' + (Join-Path $testDir 'compat')) (Join-Path $testDir 'profile_test.cpp') -o (Join-Path $testDir 'profile_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile test compilation failed' }
& (Join-Path $testDir 'profile_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile tests failed' }
& $Zig c++ -std=c++17 -O1 (Join-Path $testDir 'logic_test.cpp') -o (Join-Path $testDir 'logic_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logic test compilation failed' }
& (Join-Path $testDir 'logic_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logic tests failed' }
$jsonInclude = '-I' + (Join-Path $projectDir '.pio\libdeps\12KSG02LP1_v3\ArduinoJson\src')
& $Zig c++ -std=c++17 -O1 $jsonInclude (Join-Path $testDir 'config_json_test.cpp') -o (Join-Path $testDir 'config_json_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'JSON test compilation failed' }
& (Join-Path $testDir 'config_json_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'JSON tests failed' }
$lvgl = Join-Path $projectDir '.pio\libdeps\12KSG02LP1_v3\lvgl'
$sources = @(Get-ChildItem -LiteralPath (Join-Path $lvgl 'src') -Filter '*.c' -Recurse | ForEach-Object FullName)
$configPath = (Join-Path $testDir 'preview_lv_conf.h').Replace('\','/')
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
