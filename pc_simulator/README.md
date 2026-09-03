# Simulateur PC LVGL

Ce dossier lance l'interface LVGL du projet dans une fenêtre Windows native de 480 x 480 pixels. Il utilise LVGL 8.4.0, les écrans `ui_main.h`, `ui_settings.h` et `ui_registres_perso.h` du firmware, ainsi que la souris comme écran tactile.

Les périphériques ESP32 (Wi-Fi, Preferences, NTP et logger Solarman) sont remplacés uniquement dans le simulateur. Les valeurs initiales reprennent la capture de référence : 6 569 W PV, batterie à 47 %, consommation à 6 472 W et réseau à 0 W.

## Lancer l'émulateur déjà compilé

Double-cliquer sur :

`build\\Release\\deye_lvgl_pc_sim.exe`

La fenêtre s'appelle **TFT Simulator**. Cliquer sur `CFG` ouvre les écrans de configuration ; la souris se comporte comme le tactile.

## Recompiler après une modification de l'interface

Depuis ce dossier, configurer puis compiler le projet CMake avec Visual Studio Build Tools. La configuration télécharge LVGL 8.4.0 et SDL2 lors du premier lancement.

```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release --target deye_lvgl_pc_sim
```

Ce simulateur vérifie fidèlement le rendu et la navigation LVGL. Il ne remplace pas un test matériel : les échanges Wi-Fi/Solarman, le contrôleur tactile GT911 et le pilote RGB ESP32 restent propres à l'écran physique.
