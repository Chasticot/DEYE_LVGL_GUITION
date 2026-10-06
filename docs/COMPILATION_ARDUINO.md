# Compiler le firmware unifié 4.3.9 avec Arduino IDE

Cette procédure utilise les sources de `firmware/DEYE_UNIFIED/`. Le pack de la release est construit avec PlatformIO et s'installe sans Arduino IDE. La compilation Arduino 4.3.9 doit être distinguée de la validation PlatformIO consignée dans [VALIDATION.md](VALIDATION.md).

## Installer les dépendances

1. Installer [Arduino IDE](https://www.arduino.cc/en/software).
2. Dans **Fichier > Préférences > URL de gestionnaire de cartes supplémentaires**, ajouter `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
3. Dans le gestionnaire de cartes, installer **esp32 by Espressif Systems 2.0.17**.
4. Installer exactement **lvgl 8.4.0**, **GFX Library for Arduino 1.4.7** et **ArduinoJson 6.21.5** depuis le gestionnaire de bibliothèques.

La version 3.x du paquet ESP32 change des API utilisées par l'affichage. Garder les versions ci-dessus pour reproduire cette configuration.

## Préparer le sketch

1. Extraire ou cloner les sources du dépôt sur le PC.
2. À sa racine, lancer **PREPARER_ARDUINO.bat**.
3. Ouvrir **`firmware/DEYE_UNIFIED/DEYE_UNIFIED.ino`**.

Le BAT génère `build_opt.h` avec les chemins locaux vers `lv_conf.h` et active C++17. Il ne modifie pas l'installation de la bibliothèque LVGL. Relancer le BAT après tout déplacement ou renommage du dépôt ; les chemins générés sont propres au PC et exclus de Git.

Le `.ino` fournit l'entrée du sketch ; le programme se trouve dans `main.cpp` et les fichiers voisins. Conserver tout le dossier ensemble. Le firmware unifié contient tous les modèles disponibles et les trois choix de recharge, sélectionnés dans les réglages après installation.

## Réglages du menu Outils

| Option | Valeur |
| --- | --- |
| Board | ESP32S3 Dev Module |
| Upload Speed | 460800 ; 115200 si nécessaire |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | Enabled |
| USB Firmware MSC On Boot | Disabled |
| USB DFU On Boot | Disabled |
| Upload Mode | UART0 / Hardware CDC |
| CPU Frequency | 240 MHz |
| Flash Mode | QIO 80 MHz |
| Flash Size | 4 MB |
| Partition Scheme | Minimal SPIFFS (1.9 MB APP with OTA/190 KB SPIFFS) |
| PSRAM | OPI PSRAM |
| Core Debug Level | None |
| Arduino Runs On / Events Run On | Core 1 |
| Erase All Flash Before Sketch Upload | Disabled, sauf effacement complet souhaité |
| Port | Port COM de l'écran |

Ce partitionnement utilise les quatre premiers Mo même sur un écran avec davantage de flash physique. Sauvegarder les réglages avant toute installation ; un effacement complet les supprime. Le fichier OTA seul ne remplace pas une table de partitions.

**Vérifier** compile le sketch. **Téléverser** écrit l'écran connecté : identifier son port et fermer les autres applications série avant cette action. Pour une installation sans compilation, utiliser le [pack Windows](INSTALLATION_WINDOWS.md).

## Arduino CLI

Après installation des mêmes dépendances et préparation des fichiers :

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=4M,PartitionScheme=min_spiffs,PSRAM=opi,FlashMode=qio firmware/DEYE_UNIFIED
```

La compilation seule ne modifie aucun matériel.

## Dépannage et anciennes versions

- `lv_conf.h` introuvable : relancer le BAT après extraction ou déplacement.
- Erreurs LVGL ou Arduino_GFX : vérifier les versions exactes des dépendances.
- Application trop grande : sélectionner Minimal SPIFFS avec OTA.
- Écran noir ou redémarrages : vérifier la PSRAM OPI, le brochage et la référence physique de l'écran.

Les sketches `firmware/DEYE_V3/DEYE_V3.ino` et `firmware/DEYE_VETRONIC_V3/DEYE_VETRONIC_V3.ino` restent disponibles pour les versions historiques 4.3.3 et 4.3.4. La version actuelle se compile dans DEYE_UNIFIED. [Guide 4.3.9](GUIDE_UNIFIE_4.3.9.md).
