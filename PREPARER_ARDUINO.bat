@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\prepare_arduino.ps1"
set "RESULTAT=%ERRORLEVEL%"
pause
exit /b %RESULTAT%
