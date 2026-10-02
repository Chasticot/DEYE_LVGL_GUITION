$ErrorActionPreference = 'Stop'
try {
    $root = Split-Path -Parent $PSScriptRoot
    foreach ($name in @('DEYE_V3', 'DEYE_VETRONIC')) {
        $sketch = (Join-Path $root "firmware/$name").Replace('\', '/')
        $options = '-I"' + $sketch + '" -include "' + $sketch + '/lv_conf.h"'
        if ($name -eq 'DEYE_V3') { $options += ' -std=gnu++17' }
        [System.IO.File]::WriteAllText((Join-Path $sketch 'build_opt.h'), $options + "`n", (New-Object System.Text.UTF8Encoding($false)))
        Write-Host "Configuration Arduino preparee : $name"
    }
    Write-Host 'Ouvrir ensuite le .ino de la variante voulue. Relancer ce BAT si le dossier est deplace.'
    exit 0
} catch { Write-Host $_.Exception.Message; exit 1 }
