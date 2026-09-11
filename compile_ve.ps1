param(
  [string]$ArduinoCli = 'C:/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe'
)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $ArduinoCli)) { throw 'Indiquer le chemin arduino-cli avec -ArduinoCli.' }
Push-Location -LiteralPath $PSScriptRoot
try {
  & $ArduinoCli compile --fqbn 'esp32:esp32:esp32s3:CDCOnBoot=cdc,PartitionScheme=min_spiffs' --build-property 'build.psram_type=opi' --build-path (Join-Path $PSScriptRoot 'build_ve_check') $PSScriptRoot
  if ($LASTEXITCODE -ne 0) { throw "Compilation Arduino echouee (code $LASTEXITCODE)." }
} finally {
  Pop-Location
}
