@echo off
setlocal
cd /d "%~dp0"
title Installation DEYE Monitor
if not exist "%~dp0installer.ps1" (
  echo ERREUR : installer.ps1 absent. Extraire tout le ZIP avant de lancer ce fichier.
  pause
  exit /b 1
)
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0installer.ps1"
set "RESULTAT=%ERRORLEVEL%"
echo.
pause
exit /b %RESULTAT%
