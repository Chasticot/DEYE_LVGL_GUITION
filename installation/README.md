# Sources de l'installateur Windows

Pour installer sans compiler, télécharger le **pack Windows complet** de la variante voulue dans les [Releases](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases). Ce dossier contient les sources du lanceur ; seul, il ne contient pas les binaires ni esptool et ne permet pas de flasher.

`INSTALLER.bat` lance `installer.ps1`. Les packs générés ajoutent `esptool.exe`, sa licence, les quatre binaires, la notice et le manifeste SHA-256. La détection d'un CH340 unique sélectionne son port automatiquement ; plusieurs CH340 ou une détection indisponible déclenchent le choix manuel.

Les tests de sélection utilisent des ports simulés et n'ouvrent aucune liaison série :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File installation/test_port_selection.ps1
```

Pour reconstruire les packs, voir le [guide PlatformIO](../docs/COMPILATION_PLATFORMIO.md#tests-et-création-des-packs).
