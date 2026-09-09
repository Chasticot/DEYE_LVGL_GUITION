param([string]$Compiler = 'C:/Users/lagie/AppData/Local/Arduino15/packages/esp32/tools/xtensa-esp32s3-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32s3-elf-g++.exe')
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$testBuild = Join-Path $project 'build_network_tests'
New-Item -ItemType Directory -Path $testBuild -Force | Out-Null
# Add only constexpr qualifiers to the actual pure validation functions so GCC
# evaluates the production algorithms in static_assert, without an ESP32 board.
$header = (Get-Content -LiteralPath (Join-Path $project 'network_config.h') -Raw).Replace('static bool ', 'static constexpr bool ')
Set-Content -LiteralPath (Join-Path $testBuild 'network_config_testable.h') -Value $header -Encoding UTF8
& $Compiler -std=c++14 -fsyntax-only -I $testBuild (Join-Path $PSScriptRoot 'network_config_test.cpp')
if ($LASTEXITCODE -ne 0) { throw 'Echec des tests reseau.' }
Write-Output 'PASS : validation IPv4, DHCP, masques, passerelle et DNS.'
