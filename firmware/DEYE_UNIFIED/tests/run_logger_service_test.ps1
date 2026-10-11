param([string]$Zig = 'zig')
$ErrorActionPreference = 'Stop'
$source = [IO.File]::ReadAllText((Join-Path $PSScriptRoot '..\deye_solarman.h'))
$start = $source.IndexOf('static uint8_t solarman_checksum(')
$end = $source.IndexOf('static uint16_t modbus_get_u16_be(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Production protocol functions not found.' }
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'logger_protocol_under_test.h'), $source.Substring($start, $end-$start))
$env:ZIG_GLOBAL_CACHE_DIR = Join-Path $env:TEMP 'deye-v3-435-zig-cache'
& $Zig c++ -std=c++17 -O1 -UNDEBUG ('-I' + (Join-Path $PSScriptRoot 'logger_compat')) (Join-Path $PSScriptRoot 'logger_service_test.cpp') -o (Join-Path $PSScriptRoot 'logger_service_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logger service test compilation failed.' }
& (Join-Path $PSScriptRoot 'logger_service_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logger service test failed.' }
$settings = [IO.File]::ReadAllText((Join-Path $PSScriptRoot '..\settings.h'))
$start = $settings.IndexOf('static bool settings_save_discovered_deye_host(')
$end = $settings.IndexOf('static bool settings_vetronic_host_valid(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Production logger persistence function not found.' }
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'logger_settings_under_test.h'), $settings.Substring($start, $end-$start))
& $Zig c++ -std=c++17 -O1 -UNDEBUG ('-I' + (Join-Path $PSScriptRoot 'logger_compat')) (Join-Path $PSScriptRoot 'logger_recovery_test.cpp') -o (Join-Path $PSScriptRoot 'logger_recovery_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logger recovery test compilation failed.' }
& (Join-Path $PSScriptRoot 'logger_recovery_test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Logger recovery test failed.' }
