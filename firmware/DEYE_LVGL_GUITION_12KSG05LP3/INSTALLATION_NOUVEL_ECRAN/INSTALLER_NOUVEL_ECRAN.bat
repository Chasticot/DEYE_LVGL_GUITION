@echo off
setlocal EnableExtensions DisableDelayedExpansion
chcp 65001 >nul
cd /d "%~dp0"
title Installation du firmware - nouvel ecran

set "ESPTOOL=%~dp0esptool.exe"
set "BOOTLOADER=%~dp0bootloader.bin"
set "PARTITIONS=%~dp0partitions.bin"
set "FIRMWARE=%~dp0firmware.bin"

echo.
echo ================================================
echo   Installation du firmware sur un nouvel ecran
echo ================================================
echo.

if not exist "%ESPTOOL%" goto :outil_manquant
if not exist "%BOOTLOADER%" goto :fichier_manquant
if not exist "%PARTITIONS%" goto :fichier_manquant
if not exist "%FIRMWARE%" goto :fichier_manquant

echo Branchez l'ecran au PC avec un cable USB de donnees.
echo.
echo Recherche automatique d'un port CH340...
set "PORT="
for /f "usebackq delims=" %%P in (`powershell -NoProfile -Command "try { $ports = @(Get-CimInstance Win32_PnPEntity -ErrorAction Stop | Where-Object { $_.Name -match 'CH340' -and $_.Name -match '\(COM\d+\)' }); if ($ports.Count -eq 1) { [regex]::Match($ports[0].Name, 'COM\d+').Value } } catch {}"`) do set "PORT=%%P"
if defined PORT goto :port_trouve

echo Aucun port CH340 unique n'a ete trouve.
echo Ports serie detectes :
powershell -NoProfile -Command "try { $ports = Get-CimInstance Win32_PnPEntity -ErrorAction Stop | Where-Object { $_.Name -match '\(COM\d+\)' } | Select-Object -ExpandProperty Name } catch { $ports = [System.IO.Ports.SerialPort]::GetPortNames() }; if ($ports) { $ports } else { Write-Host '  Aucun port serie detecte.' }"
echo.
set /p "PORT=Port COM de l'ecran (exemple COM5) : "
set "PORT=%PORT: =%"
if "%PORT%"=="" goto :port_invalide
if /I not "%PORT:~0,3%"=="COM" set "PORT=COM%PORT%"

:port_trouve
echo.
echo Port utilise : %PORT%
echo IMPORTANT : l'effacement complet supprime le Wi-Fi et tous les reglages locaux.
set /p "EFFACER=Effacer toute la memoire flash avant installation ? (O/N, defaut N) : "
if /I "%EFFACER%"=="O" goto :effacer_flash
if /I "%EFFACER%"=="OUI" goto :effacer_flash
goto :installer

:effacer_flash
echo.
echo Effacement complet de la memoire flash sur %PORT%...
"%ESPTOOL%" --chip esp32s3 --port "%PORT%" --baud 460800 --before default_reset erase_flash
if errorlevel 1 goto :echec

:installer
echo.
echo Installation en cours sur %PORT%...
echo Ne debranchez pas l'ecran pendant l'installation.
echo.
"%ESPTOOL%" --chip esp32s3 --port "%PORT%" --baud 460800 --before default_reset --after hard_reset write_flash -z 0x0 "%BOOTLOADER%" 0x8000 "%PARTITIONS%" 0x10000 "%FIRMWARE%"
if errorlevel 1 goto :echec

echo.
echo ================================================
echo   SUCCES : firmware installe.
echo   L'ecran redemarre automatiquement.
echo ================================================
goto :fin

:port_invalide
echo.
echo Aucun port valide n'a ete saisi. Relancez le programme et entrez COM suivi d'un numero.
goto :fin

:outil_manquant
echo.
echo ERREUR : esptool.exe est absent de ce dossier.
goto :fin

:fichier_manquant
echo.
echo ERREUR : un fichier .bin est absent de ce dossier.
goto :fin

:echec
echo.
echo ECHEC de connexion ou d'installation.
echo Verifiez le cable USB (il doit transporter les donnees), le port COM et l'alimentation.
echo Si necessaire, maintenez BOOT, appuyez brievement sur RESET, puis relachez BOOT et relancez ce programme.

:fin
echo.
pause
endlocal
