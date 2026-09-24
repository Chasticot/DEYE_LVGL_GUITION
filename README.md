# DEYE LVGL GUITION

Firmware pour écran tactile GUITION ESP32-S3 480 × 480, destiné au suivi et au pilotage d'installations équipées d'un onduleur Deye.

Le dépôt contient plusieurs variantes. Il est important de choisir le dossier correspondant exactement au modèle d'onduleur ou à l'intégration utilisée.

## Choisir la bonne variante

| Variante | Utilisation |
| --- | --- |
| [`DEYE_LVGL_GUITION_12KSG02LP1`](firmware/DEYE_LVGL_GUITION_12KSG02LP1/) | Onduleur Deye 12K SG02 LP1 |
| [`DEYE_LVGL_GUITION_12KSG05LP3`](firmware/DEYE_LVGL_GUITION_12KSG05LP3/) | Onduleur Deye 12K SG05 LP3 |
| [`DEYE_LVGL_GUITION_25KSG01HP3`](firmware/DEYE_LVGL_GUITION_25KSG01HP3/) | Onduleur Deye 25K SG01 HP3 |
| [`DEYE_LVGL_GUITION_VETRONIC`](firmware/DEYE_LVGL_GUITION_VETRONIC/) | Version intégrant la gestion VETRONIC |

Ne flashez pas une variante prévue pour un autre modèle d'onduleur.

## Installation simple — recommandée aux débutants

Cette méthode ne nécessite ni Arduino IDE ni compilation.

1. Cliquez sur **Code**, puis **Download ZIP** sur cette page GitHub.
2. Décompressez le fichier téléchargé.
3. Ouvrez `firmware`, puis le dossier correspondant à votre onduleur.
4. Ouvrez le dossier `INSTALLATION_NOUVEL_ECRAN`.
5. Branchez l'écran au PC avec un câble USB qui transporte les données.
6. Double-cliquez sur `INSTALLER_NOUVEL_ECRAN.bat`.
7. Suivez les indications affichées et attendez le message `SUCCES`.

Le dossier contient déjà le programme d'installation, le firmware, le bootloader et la table de partitions. Aucun logiciel supplémentaire n'est nécessaire sous Windows.

Pour un écran neuf, acceptez l'effacement complet de la mémoire. Sur un écran déjà configuré, refusez l'effacement pour conserver le Wi-Fi et les réglages locaux.

En cas d'échec, vérifiez que le câble USB transmet bien les données. Si la carte possède des boutons `BOOT` et `RESET`, maintenez `BOOT`, appuyez brièvement sur `RESET`, relâchez `BOOT`, puis relancez l'installateur.

## Installation et développement avec Arduino IDE

Cette méthode est destinée aux personnes qui souhaitent modifier ou recompiler le firmware.

1. Ouvrez le dossier de la variante voulue dans `firmware/`.
2. Ouvrez le fichier `.ino` portant exactement le même nom que ce dossier.
3. Installez le support ESP32 pour Arduino ainsi que les bibliothèques `lvgl` 8.4.0 et `Arduino_GFX` 1.4.7.
4. Installez la version **2.0.17** du paquet `esp32 by Espressif Systems`.
5. Sélectionnez la carte `ESP32S3 Dev Module`.
6. Appliquez les réglages du menu **Outils** indiqués ci-dessous.
7. Compilez puis téléversez par USB.

### Configuration du menu Outils (ESP32 2.0.17)

| Option | Valeur |
| --- | --- |
| Board | `ESP32S3 Dev Module` |
| Upload Speed | `921600` |
| USB Mode | `Hardware CDC and JTAG` |
| USB CDC On Boot | `Enabled` |
| USB Firmware MSC On Boot | `Disabled` |
| USB DFU On Boot | `Disabled` |
| Upload Mode | `UART0 / Hardware CDC` |
| CPU Frequency | `240MHz (WiFi)` |
| Flash Mode | `QIO 80MHz` |
| Flash Size | `16MB (128Mb)` |
| Partition Scheme | `16M Flash (3MB APP/9.9MB FATFS)` |
| Core Debug Level | `None` |
| PSRAM | `OPI PSRAM` |
| Arduino Runs On | `Core 1` |
| Events Run On | `Core 1` |
| Erase All Flash Before Sketch Upload | `Disabled` |
| JTAG Adapter | `Disabled` |

La flash de 16 MB et la PSRAM de 8 MB utilisent deux interfaces distinctes : conservez **QIO 80 MHz pour la flash** et **OPI PSRAM pour la PSRAM**. Le schéma de partition ci-dessus fournit deux emplacements applicatifs de 3 MB et reste donc compatible avec les mises à jour OTA.

Après un changement de schéma de partition, effectuez le premier téléversement par USB. Laissez l'effacement complet désactivé si vous souhaitez conserver les paramètres Wi-Fi et les autres préférences déjà enregistrées. Si le téléversement échoue à `921600`, réduisez uniquement `Upload Speed` à `460800` ou `115200`.

## Fonctions principales

- Interface tactile LVGL pour écran 480 × 480.
- Lecture des informations de l'onduleur Deye par le réseau.
- Affichage solaire, batterie, consommation, réseau et bilans d'énergie.
- Configuration Wi-Fi, réseau, NTP et onduleur depuis l'écran ou l'interface Web.
- Intégration des informations VE et Tempo selon la variante.
- Mise à jour OTA du firmware depuis l'interface Web.
- Intégration VETRONIC dans la variante dédiée.

## Matériel

- Écran GUITION ESP32-S3 480 × 480, par exemple ESP32-4848S040.
- PSRAM OPI.
- Onduleur Deye compatible avec la variante choisie.
- Logger ou interface réseau compatible avec le firmware.

Vérifiez toujours le modèle de l'onduleur et le brochage exact de l'écran avant le flash.

## Comparaison des registres

Consultez le [tableau comparatif des registres par variante](REGISTRES_PAR_VARIANTE.md). Les adresses qui restent à confirmer y sont indiquées en italique.

## Licence

Projet distribué sous licence MIT. Consultez le fichier [`LICENSE`](LICENSE).
