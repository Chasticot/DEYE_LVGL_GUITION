# DEYE LVGL GUITION — LSW amélioré

Firmware autonome pour écran GUITION ESP32-S3 480 × 480, destiné au suivi d’un onduleur Deye via logger Solarman LSW.

Cette version propose une interface tactile LVGL, le suivi photovoltaïque et batterie, l’intégration VE/TEMPO, une configuration complète par page Web et des mises à jour OTA. Les réglages sont conservés dans la mémoire de l’écran.

## Ce que fait le projet

- Affiche la production solaire, la batterie, la consommation, le réseau, l’import/export et les bilans d’énergie.
- Communique avec l’onduleur Deye via le logger Solarman LSW.
- Intègre les informations VE et la couleur Tempo.
- Permet de régler l’écran, le Wi-Fi, le réseau DHCP/IP fixe, le logger, l’onduleur, le NTP et les registres personnalisés.
- Fournit un tableau de bord Web, un historique, un diagnostic et l’export/import de configuration JSON.
- Permet de mettre à jour le firmware par OTA sans câble USB après la première installation.

## Installation simple — public non technique

Cette méthode ne demande pas Arduino IDE.

1. Téléchargez le dépôt avec **Code → Download ZIP**, puis décompressez-le.
2. Ouvrez le dossier `INSTALLATION_NOUVEL_ECRAN`.
3. Branchez l’écran au PC avec un câble USB qui transporte les données.
4. Double-cliquez sur `INSTALLER_NOUVEL_ECRAN.bat`.
5. Le programme détecte automatiquement le port de l’écran. Si nécessaire, saisissez le numéro COM affiché par Windows.
6. Pour un écran neuf, répondez `O` à la question d’effacement complet. Pour conserver une configuration existante, répondez `N`.
7. Attendez le message **SUCCES** et le redémarrage de l’écran.

Le dossier contient déjà `esptool.exe`, le firmware, le bootloader et la table de partitions : aucune installation supplémentaire n’est nécessaire. En cas d’échec, utilisez un câble USB de données et, si votre carte possède ces boutons, maintenez `BOOT`, appuyez brièvement sur `RESET`, relâchez `BOOT`, puis relancez le BAT.

Les détails sont disponibles dans [`INSTALLATION_NOUVEL_ECRAN/LISEZ_MOI.txt`](INSTALLATION_NOUVEL_ECRAN/LISEZ_MOI.txt).

## Configuration par page Web

Après le démarrage et la connexion au réseau, ouvrez `http://ADRESSE_IP_DE_L_ECRAN/` depuis un appareil connecté au même réseau. La page permet de configurer l’écran sans recompiler : Wi-Fi, réseau, logger, onduleur, NTP, Tempo/VE, affichage et registres.

## Mise à jour OTA

Pour faire une mise à jour OTA, il faut prendre le fichier firmware applicatif `.ino.bin` de la release. Dans la page Web, ouvrez **Mise à jour OTA**, sélectionnez ce `.bin`, puis lancez l’installation.

N’utilisez pas le `bootloader.bin`, le fichier `partitions.bin` ni une image fusionnée pour une mise à jour OTA. Ces fichiers servent uniquement à l’installation USB complète. Les paramètres enregistrés sont conservés après l’OTA.

## Installation et développement avec Arduino IDE

Cette méthode s’adresse aux personnes qui souhaitent compiler, modifier ou développer le projet.

1. Installez le support de cartes ESP32 depuis `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`.
2. Ouvrez `DEYE_LVGL_UI_2x2_menu_avance_coef_LSW_ameliore.ino` dans Arduino IDE.
3. Installez `lvgl` 8.4.0 et `Arduino_GFX` 1.4.7.
4. Sélectionnez une carte `ESP32S3 Dev Module` correspondant à votre écran.
5. Utilisez : USB CDC On Boot = Enabled, CPU = 240 MHz (WiFi), PSRAM = OPI PSRAM, Partition Scheme = Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS), Upload Speed = 921600.
6. Compilez puis téléversez par USB.

Le partitionnement OTA est indispensable pour utiliser ensuite la mise à jour depuis la page Web. Un effacement complet de la flash supprime les paramètres Wi-Fi et les réglages enregistrés.

## Matériel et documentation

- Écran GUITION ESP32-S3 480 × 480, par exemple ESP32-4848S040.
- PSRAM OPI, logger Solarman LSW3 et onduleur Deye compatible.
- Broches GT911 par défaut : SDA GPIO 19, SCL GPIO 45, rétroéclairage GPIO 38, adresse `0x5D`.
- [`WEB_SERVER.md`](WEB_SERVER.md) : configuration Web, authentification, réseau et OTA.
- [`VE_INTEGRATION.md`](VE_INTEGRATION.md) : intégration VE et limites connues.
- [`INSTALLATION_NOUVEL_ECRAN/`](INSTALLATION_NOUVEL_ECRAN/) : installation USB autonome.

Vérifiez toujours le brochage exact de votre écran avant tout flash. Projet distribué sous licence MIT.
