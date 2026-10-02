# VS Code et PlatformIO

## Installer les outils

1. Installer [Visual Studio Code](https://code.visualstudio.com/).
2. Dans **Extensions**, installer **PlatformIO IDE**, publié par PlatformIO.
3. Télécharger les sources du dépôt et les extraire, ou cloner le dépôt.
4. Dans VS Code, choisir **Fichier > Ouvrir un dossier** et ouvrir la racine contenant `platformio.ini`, pas seulement `firmware/`.
5. Laisser PlatformIO installer les outils et les bibliothèques. Le premier lancement nécessite Internet.

## Choisir la variante

| Variante | Environnement | Sources |
| --- | --- | --- |
| V3 (par défaut) | `12KSG02LP1_v3` | `firmware/DEYE_V3/` |
| VEtronic | `deye_vetronic` | `firmware/DEYE_VETRONIC/` |

Dans l'icône PlatformIO, ouvrir **Project Tasks > environnement > General > Build**. **Build** compile seulement. **Upload** installe le firmware sur l'écran USB sélectionné. **Monitor** ouvre le moniteur série à 115200 bauds ; le fermer avant un téléversement ou l'utilisation du pack Windows.

## Commandes équivalentes

Dans un terminal PlatformIO, à la racine du dépôt :

```powershell
pio run -e 12KSG02LP1_v3
pio run -e deye_vetronic
```

Les applications compilées sont `.pio/build/12KSG02LP1_v3/firmware.bin` et `.pio/build/deye_vetronic/firmware.bin`. Pour installer par USB, en remplaçant COM5 par le port réel :

```powershell
pio run -e 12KSG02LP1_v3 -t upload --upload-port COM5
```

Pour VEtronic, remplacer le nom de l'environnement. Le téléversement écrit l'écran ; une simple compilation ne modifie aucun appareil.

## Configuration fixée par le projet

`espressif32@6.10.0`, framework Arduino ESP32 2.0.17, LVGL 8.4.0, GFX Library for Arduino 1.4.7 et ArduinoJson 6.21.5. PlatformIO télécharge les dépendances déclarées dans `platformio.ini` ([fonctionnement de lib_deps](https://docs.platformio.org/en/latest/projectconf/sections/env/options/library/lib_deps.html)). Il ne faut pas ajouter de bibliothèques manuellement dans `lib/`.

La carte de compilation est `esp32-s3-devkitc-1` avec PSRAM OPI et mémoire Arduino `qio_opi`. Le brochage du véritable écran se trouve dans `main.cpp` et `config.h`. Flash logique 4 Mo, partition `min_spiffs.csv`, deux emplacements OTA de 1 966 080 octets. Cette configuration utilise les quatre premiers Mo même si l'écran possède davantage de flash physique. Ne pas mélanger les binaires avec une autre table de partitions.

La V3 utilise C++17. Les options et l'inclusion de `lv_conf.h` sont fournies automatiquement ; `PREPARER_ARDUINO.bat` n'est pas nécessaire pour PlatformIO.

## Tests et création des packs

Après compilation de la V3, les tests hôtes utilisent Zig 0.13.0 :

```powershell
powershell -ExecutionPolicy Bypass -File firmware/DEYE_V3/tests/run_checks.ps1 -Zig C:/outils/zig/zig.exe
```

Pour assembler les deux packs Windows après compilation des deux environnements, fournir l'exécutable Windows esptool 4.5.1 (celui du pack publié convient) :

```powershell
python tools/build_installation_packs.py --esptool C:/outils/esptool.exe
python tools/test_installation_packs.py
powershell -ExecutionPolicy Bypass -File installation/test_port_selection.ps1
```

Les résultats sont dans `dist/`, exclu de Git. Le script d'assemblage vérifie la version présente dans chaque binaire, la taille et les partitions. Il ne compile pas et ne flashe aucun écran. L'outil esptool provient du [projet Espressif](https://github.com/espressif/esptool/tree/v4.5.1).
