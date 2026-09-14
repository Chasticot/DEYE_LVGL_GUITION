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
4. Sélectionnez une carte ESP32-S3 correspondant à l'écran.
5. Utilisez notamment `USB CDC On Boot = Enabled`, `PSRAM = OPI PSRAM` et une table de partitions compatible OTA.
6. Compilez puis téléversez par USB.

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

## Licence

Projet distribué sous licence MIT. Consultez le fichier [`LICENSE`](LICENSE).
