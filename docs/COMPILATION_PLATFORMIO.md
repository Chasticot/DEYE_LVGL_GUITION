# Compiler le firmware unifié 4.3.7 avec PlatformIO

## Installer et ouvrir le projet

1. Installer [Visual Studio Code](https://code.visualstudio.com/) et l'extension **PlatformIO IDE**.
2. Cloner le dépôt ou extraire son ZIP de sources.
3. Ouvrir la racine contenant `platformio.ini`, plutôt que seulement `firmware/`.
4. Laisser PlatformIO installer les outils et bibliothèques. Le premier lancement nécessite Internet.

L'environnement actuel est **`deye_unified`**, compilé par défaut à partir de **`firmware/DEYE_UNIFIED/`**. Le choix du modèle Deye et de la borne se fait dans le firmware : aucune compilation distincte par équipement n'est nécessaire.

## Compiler, installer et surveiller

Dans **Project Tasks > deye_unified > General**, **Build** compile, **Upload** écrit l'écran USB et **Monitor** ouvre le port série à 115200 bauds.

À la racine du dépôt :

```powershell
pio run
# Équivalent explicite :
pio run -e deye_unified
```

Le binaire OTA produit est `.pio/build/deye_unified/firmware.bin`. Il est destiné au formulaire OTA de l'écran avec un partitionnement compatible. La passerelle WB01 conserve son propre firmware.

Pour installer par USB, après identification du port réel et fermeture des moniteurs série :

```powershell
pio run -e deye_unified -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

Remplacer COM5 par le port de l'écran. Une compilation seule n'écrit aucun appareil.

## Configuration fournie

- `espressif32@6.10.0`, framework Arduino ESP32 2.0.17.
- LVGL 8.4.0, GFX Library for Arduino 1.4.7 et ArduinoJson 6.21.5.
- C++17, inclusion de `lv_conf.h` et options USB fournies automatiquement.
- Carte de compilation `esp32-s3-devkitc-1`, PSRAM OPI, mémoire Arduino `qio_opi`.
- Flash logique 4 Mo, partition `min_spiffs.csv`, deux emplacements applicatifs OTA de 1 966 080 octets.

Les dépendances déclarées par `lib_deps` sont téléchargées par PlatformIO ; il n'est pas nécessaire de copier manuellement des bibliothèques dans `lib/`. Le brochage réel de l'écran est dans `main.cpp` et `config.h`. Ne pas mélanger les binaires et une autre table de partitions. `PREPARER_ARDUINO.bat` concerne Arduino IDE, pas PlatformIO.

## Tests et pack Windows

Les tests hôte utilisent Zig 0.13.0 ; les tests Web nécessitent Node.js. Depuis la racine :

```powershell
powershell -ExecutionPolicy Bypass -File firmware/DEYE_UNIFIED/tests/run_checks.ps1 -Zig C:/outils/zig/zig.exe
node firmware/DEYE_UNIFIED/tests/web_test.cjs
```

L'aperçu LVGL compile les deux interfaces de recharge et vérifie les choix SG02/LP3, les brouillons, les contrôles et les états hors ligne. Les scripts Web testent le code réel avec un DOM et un transport simulés. Ces tests ne commandent aucun appareil.

Après compilation, pour assembler uniquement le pack actuel avec un exécutable Windows esptool 4.5.1 :

```powershell
python tools/build_installation_packs.py --variant UNIFIED --esptool C:/outils/esptool.exe
python tools/test_installation_packs.py
powershell -ExecutionPolicy Bypass -File installation/test_port_selection.ps1
```

Les résultats sont dans `dist/`, exclu de Git. L'assemblage vérifie les fichiers et les partitions ; il ne compile pas et ne flashe aucun écran. Le pack publié dispense l'utilisateur final de ces outils.

## Sources historiques

| Version | Environnement | Sources |
| --- | --- | --- |
| V3 4.3.3 | `12KSG02LP1_v3` | `firmware/DEYE_V3/` |
| Vetronic V3 4.3.4 | `vetronic_v3` | `firmware/DEYE_VETRONIC_V3/` |

Ces environnements servent à reproduire les anciennes versions. Le projet courant et les téléchargements recommandés utilisent 4.3.7. [Validation et limites](VALIDATION.md).
