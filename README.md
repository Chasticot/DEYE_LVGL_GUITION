DEYE LVGL Monitor – Interface tactile pour onduleur Deye (Solarman V5)

<img width="1536" height="2048" alt="image" src="https://github.com/user-attachments/assets/73ff4867-a7e3-4e30-a7ba-2dfac98bcd35" />


Interface de monitoring tactile pour onduleurs Deye hybrides (SG02LP1).
Affiche en temps réel les données de production PV, batterie, consommation et réseau sur un écran 480×480.
Communication via le protocole Solarman V5 – compatible avec les data loggers LSW3.

https://docs/dashboard.png
✨ Fonctionnalités

    📊 Affichage en temps réel :

        Production PV (PV1, PV2, PV3)

        État de la batterie (SOC, tension, puissance, température)

        Consommation globale (LOAD + UPS)

        Puissance réseau (IMPORT / EXPORT) avec signe négatif en cas d’injection

        SmartLoad ON/OFF

        Températures DC/AC/BAT

    📈 Données journalières :

        Production PV du jour (kWh)

        Consommation du jour (kWh)

        Achat et vente réseau (kWh)

    ⚙️ Configuration sans fil :

        WiFi (scan des réseaux avec affichage de la qualité en %)

        NTP (fuseau horaire, serveurs)

        Deye (adresse IP, numéro de série du logger)

    🖥️ Interface tactile :

        Écran 480×480 avec rotation 90°

        Navigation fluide grâce au multithread (FreeRTOS)

        LEDs d’état WiFi et Deye

    🔌 Protocole Solarman V5 :

        Lecture Modbus RTU encapsulée

        Timeouts longs (15s) inspirés du plugin Jeedom

        Reconnexion automatique à chaque lecture

🧰 Matériel requis

    Ecran GUITION type ESP32-4848S040
    Le lien de celui que j'ai acheté en 2025 => https://fr.aliexpress.com/item/1005006622746590.html
    Data logger Solarman LSW3 connecté à l’onduleur Deye (même réseau WiFi)

🛠️ Configuration de l’environnement Arduino IDE
1. Installer le support ESP32

    Ouvrez Arduino IDE → Fichier → Préférences.

    Dans « URL de gestionnaire de cartes supplémentaires », ajoutez :
    text

    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

    Ouvrez Outils → Type de carte → Gestionnaire de cartes….

    Recherchez esp32 et installez la version 2.0.17.

2. Choisir la carte et ses paramètres

Sélectionnez la carte adaptée à votre matériel, par exemple :

    Outils → Type de carte → ESP32S3 Dev Module (ou le modèle exact de votre carte).

Paramètres recommandés (pour ESP32-S3) :
Paramètre	Valeur
USB CDC On Boot	Enabled
CPU Frequency	240 MHz (WiFi)
Flash Size	16 MB (ou selon votre carte)
Partition Scheme	Huge App (3MB No OTA/1MB SPIFFS)
PSRAM	OPI PSRAM (si présente)
Upload Speed	921600

    ⚠️ Le PSRAM est obligatoire pour le buffer LVGL. Activez-le dans les paramètres.

3. Installer les bibliothèques requises

Via le Gestionnaire de bibliothèques (croquis → Inclure une bibliothèque → Gérer les bibliothèques…) :
Bibliothèque	Version	Utilisation
lvgl	8.4.0	Interface graphique
Arduino_GFX	1.4.7	Driver écran RGB/SPI
WiFi	(incluse)	Connexion réseau
Wire	(incluse)	I2C pour le tactile
Preferences	(incluse)	Stockage des paramètres
🔌 Schéma de connexion (pins)

Les broches utilisées sont définies dans config.h. Adaptez-les à votre carte :
Composant	Pins ESP32-S3
RGB Panel	18, 17, 16, 21, 11, 12, 13, 14, 0, 8, 20, 3, 46, 9, 10, 4, 5, 6, 7, 15
GT911 SDA	19
GT911 SCL	45
Rétroéclairage	38
SPI (optionnel)	39, 48, 47
📦 Installation et premier lancement

    Clonez le dépôt :
    bash

    git clone 

    Ouvrez le fichier DEYE_LVGL_UI_2x2.ino dans Arduino IDE.

    Vérifiez les paramètres par défaut dans config.h :

        WiFi SSID / password

        Adresse IP du logger Deye

        Numéro de série du logger

    Compilez et téléversez le code sur l’ESP32-S3.

    À la première mise sous tension, l’écran affichera des données simulées. Utilisez le bouton CFG pour paramétrer votre réseau WiFi et votre onduleur.

📁 Structure du projet

     DEYE_LVGL_UI_2x2/
     ├── DEYE_LVGL_UI_2x2.ino       # Point d’entrée
     ├── config.h                   # Pins, WiFi, Deye, NTP
     ├── app_data.h                 # Structure des données
     ├── settings.h                 # Gestion des préférences (Preferences)
     ├── wifi_manager.h             # WiFi
     ├── ntp_manager.h              # NTP
     ├── touch_gt911.h              # Driver GT911
     ├── deye_solarman.h            # Protocole Solarman V5 (multithread)
     ├── ui_main.h                  # Interface principale (LVGL)
     └── ui_settings.h              # Écrans de configuration

⚙️ Configuration sans fil

L’interface de configuration vous permet de modifier :

    WiFi : scan des réseaux avec affichage de la qualité en %.

    NTP : fuseau horaire, serveurs primaire et secondaire.

    Deye : adresse IP du data logger et numéro de série (visible sur l’étiquette).

Les paramètres sont sauvegardés dans la mémoire flash (Preferences) et persistent après redémarrage.
🧵 Multithreading et fluidité

Le projet utilise FreeRTOS pour déporter les lectures Solarman sur le cœur 1 de l’ESP32-S3.
L’interface LVGL tourne sur le cœur 0, ce qui garantit une navigation fluide même en cas de timeouts réseau.

    Mutex : protège les données partagées entre les cœurs.

    Flag ui_active : suspend les lectures pendant la navigation dans les menus.

🔧 Dépannage
Problème	Solution
Écran noir / rotation inversée	Vérifier gfx->setRotation(1) dans setup()
Toucher non réactif	Vérifier les broches I2C du GT911
Connexion Deye échoue	Vérifier l’adresse IP et le numéro de série dans la configuration
Lectures échouent souvent	Vérifier que le logger est sur le même réseau et que le port 8899 est ouvert
Compilation – PSRAM manquant	Activer PSRAM dans les paramètres de la carte
🙏 Remerciements

    LVGL – bibliothèque graphique

    Arduino_GFX – driver écran

    pySolarmanV5 – inspiration pour le protocole

    La communauté Jeedom et Home Assistant pour les échanges sur Solarman V5

📄 Licence

Ce projet est distribué sous licence MIT. Utilisez-le à vos propres risques.

⭐ Si ce projet vous est utile, n’hésitez pas à mettre une étoile sur GitHub !
