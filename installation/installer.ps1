param([switch]$VerificationSeule)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

function Select-PortEcran {
    param([string[]]$Ports, [string[]]$DeviceNames)
    $ch340Ports = @(
        foreach ($name in $DeviceNames) {
            if ($name -match 'CH340' -and $name -match '\((COM[0-9]+)\)') {
                $candidate = $Matches[1].ToUpperInvariant()
                if ($Ports -contains $candidate) { $candidate }
            }
        }
    ) | Sort-Object -Unique
    $ch340Ports = @($ch340Ports)
    if ($ch340Ports.Count -eq 1) {
        Write-Host "CH340 détecté : port $($ch340Ports[0]) sélectionné automatiquement." -ForegroundColor Green
        return $ch340Ports[0]
    }
    if ($ch340Ports.Count -gt 1) {
        Write-Host 'Plusieurs CH340 détectés : choisir le port de votre écran.'
    } else {
        Write-Host 'Aucun CH340 identifié : choisir le port manuellement.'
    }
    $selected = (Read-Host 'Port de votre écran (exemple COM5)').Trim().ToUpperInvariant()
    if ($selected -notmatch '^COM[0-9]+$' -or $Ports -notcontains $selected) {
        throw 'Port invalide ou absent. Aucun flash effectué.'
    }
    return $selected
}

try {
    Set-Location -LiteralPath $PSScriptRoot
    $manifest = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'manifest.json') -Raw | ConvertFrom-Json
    Write-Host "DEYE Monitor - $($manifest.variant) - version $($manifest.version)"
    Write-Host 'Vérification des fichiers...'
    $required = @('esptool.exe', 'bootloader.bin', 'partitions.bin', 'boot_app0.bin', 'firmware.bin', 'installer.ps1', 'INSTALLER.bat')
    foreach ($name in $required) {
        $path = Join-Path $PSScriptRoot $name
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Fichier absent : $name. Extraire à nouveau le ZIP complet." }
        $entry = @($manifest.files | Where-Object { $_.name -eq $name })
        if ($entry.Count -ne 1) { throw "Empreinte absente ou ambiguë : $name." }
        $hasher = [System.Security.Cryptography.SHA256]::Create()
        $stream = [System.IO.File]::OpenRead($path)
        try { $hash = [BitConverter]::ToString($hasher.ComputeHash($stream)).Replace('-', '') }
        finally { $stream.Dispose(); $hasher.Dispose() }
        if ($hash -ne $entry[0].sha256) {
            throw "Fichier modifié ou endommagé : $name. Extraire à nouveau le ZIP complet."
        }
    }
    Write-Host 'Fichiers vérifiés.' -ForegroundColor Green
    if ($VerificationSeule) { exit 0 }

    Write-Host ''
    Write-Host 'Brancher uniquement l''écran à installer, avec un câble USB de données.'
    Write-Host 'Fermer Arduino IDE, le moniteur série et tout logiciel utilisant le port COM.'
    Write-Host 'Ports détectés :'
    $ports = @([System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object)
    if ($ports.Count -eq 0) { throw 'Aucun port COM détecté. Vérifier le câble USB et le pilote, puis relancer.' }
    $deviceNames = @()
    try {
        $deviceNames = @(Get-CimInstance Win32_PnPEntity -ErrorAction Stop |
            Where-Object { $_.Name -match '\(COM[0-9]+\)' } |
            ForEach-Object { $_.Name })
        $deviceNames | ForEach-Object { Write-Host "  $_" }
    } catch { Write-Host 'Description des ports indisponible.' }
    Write-Host ($ports -join ', ')
    $port = Select-PortEcran -Ports $ports -DeviceNames $deviceNames

    $speed = (Read-Host 'Vitesse : Entrée pour 460800, ou saisir 115200 si la connexion est instable').Trim()
    if ($speed -eq '') { $speed = '460800' }
    if ($speed -notin @('460800', '115200')) { throw 'Vitesse invalide. Aucun flash effectué.' }
    Write-Host ''
    Write-Host 'Effacer la mémoire supprime le Wi-Fi, les réglages, les tarifs et les données enregistrées.'
    Write-Host 'Pour un écran neuf ou un changement de partitionnement : choisir O.'
    if ($manifest.variant -eq 'UNIFIED') {
        Write-Host 'Depuis V3/Vetronic V3 avec le même partitionnement, la mise à jour 4.3.5 permet de conserver les réglages avec N.'
        Write-Host 'Après migration, sélectionner explicitement la borne dans les réglages de l''écran.'
    } else {
        Write-Host 'Pour un changement entre anciennes variantes : choisir O.'
    }
    Write-Host 'Pour conserver les réglages : choisir N (compatibilité non garantie entre anciennes versions).'
    $erase = (Read-Host 'Effacer complètement avant installation ? O/N [N]').Trim().ToUpperInvariant()
    if ($erase -eq '') { $erase = 'N' }
    if ($erase -notin @('O', 'N')) { throw 'Réponse invalide. Aucun flash effectué.' }
    Write-Host "Installation : $($manifest.variant) / $($manifest.version) / $port / $speed bauds / effacement : $erase"
    if ((Read-Host 'Saisir INSTALLER pour démarrer, ou Entrée pour annuler').Trim() -cne 'INSTALLER') {
        Write-Host 'Installation annulée. Aucun flash effectué.'
        exit 0
    }
    $exe = Join-Path $PSScriptRoot 'esptool.exe'
    $common = @('--chip', 'esp32s3', '--port', $port, '--baud', $speed, '--before', 'default_reset', '--after', 'hard_reset')
    if ($erase -eq 'O') {
        & $exe @common erase_flash
        if ($LASTEXITCODE -ne 0) { throw 'Échec de l''effacement. Installation interrompue.' }
    }
    & $exe @common write_flash -z --flash_size 4MB 0x0 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin
    if ($LASTEXITCODE -ne 0) { throw 'Échec du flash. Ne pas considérer l''installation comme terminée.' }
    Write-Host ''
    Write-Host 'Installation terminée : écriture et vérification réussies par esptool.' -ForegroundColor Green
    Write-Host 'L''écran redémarre. Au besoin, appuyer sur RESET ou débrancher/rebrancher son alimentation.'
    Write-Host 'Configurer ensuite le Wi-Fi et le logger depuis les menus de l''écran.'
    exit 0
} catch {
    Write-Host ''
    Write-Host "ERREUR : $($_.Exception.Message)" -ForegroundColor Red
    Write-Host 'En cas de problème de connexion : vérifier le port et le câble, puis essayer à 115200 bauds.'
    Write-Host 'Si nécessaire : maintenir BOOT, appuyer puis relâcher RESET, relâcher BOOT et relancer.'
    Write-Host 'Si le port COM change après cette manipulation, sélectionner le nouveau port.'
    exit 1
}
