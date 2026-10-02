# Arduino IDE sous Windows

Cette procédure compile les mêmes sources que PlatformIO. Les packs de la release sont construits avec PlatformIO ; il n'est pas nécessaire de compiler pour les utiliser.

## Installer les dépendances

1. Installer [Arduino IDE](https://www.arduino.cc/en/software).
2. Dans **Fichier > Préférences > URL de gestionnaire de cartes supplémentaires**, ajouter `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
3. Dans le gestionnaire de cartes, rechercher **esp32 by Espressif Systems** et choisir exactement **2.0.17**. La série 3.x change des API utilisées par l'affichage.
4. Dans le gestionnaire de bibliothèques, installer exactement **lvgl 8.4.0**, **GFX Library for Arduino 1.4.7** et **ArduinoJson 6.21.5**.

La procédure d'ajout du support ESP32 est décrite dans la [documentation Espressif](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

## Préparer et ouvrir le sketch

1. Extraire le dépôt dans un dossier du PC.
2. À sa racine, double-cliquer sur **PREPARER_ARDUINO.bat**. Ce script crée un `build_opt.h` dans chaque variante pour que toutes les bibliothèques utilisent le bon `lv_conf.h`, sans modifier votre installation LVGL. Il active aussi C++17 pour la V3.
3. Ouvrir `firmware/DEYE_V3/DEYE_V3.ino` ou `firmware/DEYE_VETRONIC/DEYE_VETRONIC.ino`.

Le `.ino` est volontairement minimal : le programme se trouve dans `main.cpp`, compilé avec les fichiers du sketch. Garder tous les fichiers ensemble. **Si le dossier est déplacé ou renommé, relancer PREPARER_ARDUINO.bat** avant de compiler. Les fichiers `build_opt.h` générés contiennent les chemins de votre PC et sont exclus de Git.

## Réglages du menu Outils

| Option | Valeur |
| --- | --- |
| Board | ESP32S3 Dev Module |
| Upload Speed | 460800 (115200 en cas de problème) |
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
| Erase All Flash Before Sketch Upload | Disabled, sauf effacement complet volontaire |
| Port | Le port COM de votre écran |

Ce partitionnement correspond aux nouveaux packs USB. Il utilise 4 Mo de la mémoire, même sur un écran disposant de 16 Mo. Après un changement de partitionnement, utiliser l'USB et sauvegarder les réglages au préalable. Le seul binaire OTA ne remplace pas une table de partitions.

Cliquer sur **Vérifier** pour compiler puis, lorsque l'écran et son port sont identifiés, **Téléverser**. En cas d'échec de connexion, fermer le moniteur série, vérifier le câble de données et essayer la séquence BOOT/RESET décrite dans le guide Windows.

## Compiler avec Arduino CLI

Après préparation des fichiers et installation des dépendances :

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=4M,PartitionScheme=min_spiffs,PSRAM=opi,FlashMode=qio firmware/DEYE_V3
```

Remplacer le dossier par `firmware/DEYE_VETRONIC` pour l'autre variante. La compilation seule n'envoie rien au matériel.

## Dépannage

- `lv_conf.h` introuvable : relancer le BAT de préparation après extraction/déplacement du dépôt.
- Erreurs LVGL ou Arduino_GFX : contrôler les versions exactes des trois bibliothèques et du paquet ESP32.
- Manque d'espace applicatif : sélectionner **Minimal SPIFFS avec OTA**, pas la partition Arduino par défaut.
- Redémarrages ou écran noir : vérifier PSRAM OPI et la référence physique de l'écran. Une compilation réussie ne garantit pas un brochage compatible.

