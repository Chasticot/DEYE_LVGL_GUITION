DEYE LVGL Monitor - Interface tactile pour onduleur Deye

https://img.shields.io/badge/platform-ESP32--S3-blue
https://img.shields.io/badge/LVGL-8.4.0-orange
https://img.shields.io/badge/license-MIT-green

Interface de monitoring tactile pour onduleurs Deye hybrides (SG02LP1) avec affichage des données en temps réel sur écran 480x480. Projet basé sur ESP32-S3 avec LVGL et protocole Solarman V5.

https://docs/dashboard.png
✨ Fonctionnalités

    📊 Affichage en temps réel des données de l'onduleur

        Production PV (PV1, PV2, PV3)

        État de la batterie (SOC, tension, puissance, température)

        Consommation (LOAD + UPS)

        Puissance réseau (IMPORT/EXPORT)

        SmartLoad ON/OFF

        Températures DC/AC

    ⚙️ Configuration sans fil

        Paramètres WiFi

        Configuration NTP (fuseau horaire, serveurs)

        Configuration Deye (IP, numéro de série)

    🖥️ Interface tactile

        Écran 480x480

        Navigation fluide entre pages

        LEDs d'état (WiFi, Deye)

        Horloge NTP

🚀 Démarrage rapide
Prérequis

    Arduino IDE (ou PlatformIO)

    Carte ESP32-S3 avec support RGB Panel

    Écran LCD 480x480 (ST7701)

    Touch GT911 (I2C)

Installation

    Clonez le dépôt :

bash


    Ouvrez le fichier DEYE_LVGL_UI_2x2.ino dans Arduino IDE

    Installez les bibliothèques nécessaires :

        lvgl v8.4.0

        Arduino_GFX_Library

        WiFi, Wire, Preferences (incluses avec ESP32)

    Configurez vos paramètres dans config.h ou via l'interface de configuration

    Compilez et téléversez sur votre ESP32-S3

📁 Structure du projet


    DEYE_LVGL_UI_2x2/
    ├── DEYE_LVGL_UI_2x2.ino    # Point d'entrée principal
    ├── config.h                 # Configuration matérielle
    ├── app_data.h               # Structure des données
    ├── settings.h               # Gestion des préférences
    ├── wifi_manager.h           # Gestion WiFi
    ├── ntp_manager.h            # Gestion NTP
    ├── touch_gt911.h            # Driver tactile GT911
    ├── deye_solarman.h          # Protocole Solarman V5
    ├── ui_main.h                # Interface principale
    └── ui_settings.h            # Interface de configuration

🔧 Configuration matérielle

    Connexions principales
    Composant	Pins ESP32-S3
    RGB Panel	18, 17, 16, 21, 11, 12, 13, 14, 0, 8, 20, 3, 46, 9, 10, 4, 5, 6, 7, 15
    GT911 SDA	19
    GT911 SCL	45
    Backlight	38
    SPI (optionnel)	39, 48, 47
    
 📊 Données affichées
 
Page principale

    Bloc	Informations
    Header	Heure, Date, WiFi, LED Deye
    PV Production	PV1/PV2/PV3 (W), Production journalière (kWh)
    Batterie	SOC (%), Tension (V), Puissance (W), Température
    Consommation	LOAD (W), UPS (W)
    Réseau	Puissance (W), ON GRID / OFF GRID
    Footer	SmartLoad, Températures DC/AC/BAT
    
Page de configuration

    WiFi : Scan des réseaux, mot de passe

    NTP : Fuseau horaire, serveurs

    Deye : Adresse IP, numéro de série du logger

🔌 Protocole Solarman V5

Le projet implémente le protocole Solarman V5 pour communiquer avec les onduleurs Deye :

    Lecture Modbus RTU encapsulée dans Solarman V5

    Registres supportés :

        PV1/PV2/PV3 Power (186-188)

        Battery SOC, Voltage, Power (183-184, 190)

        Grid Power (169), Load Power (175)

        DC/AC Températures (90-91)

        Grid Status (194)

        SmartLoad (195)

🛠️ Personnalisation
Modification des registres

Dans deye_solarman.h, vous pouvez ajouter ou modifier les registres lus :
cpp

// Ajout d'un nouveau registre dans decode_main_block()
next.nouveau_registre = modbus_get_u16_be(rtu, INDEX);

Ajout de données à l'interface

    Ajoutez la variable dans app_data.h

    Lisez la valeur dans deye_solarman.h

    Affichez-la dans ui_main.h

📝 Configuration initiale
Sur l'ESP32

    Connectez-vous au WiFi via l'interface de configuration

    Configurez l'adresse IP de votre onduleur Deye

    Saisissez le numéro de série du logger (visible sur l'étiquette)

Sur l'onduleur

    Assurez-vous que l'onduleur est sur le même réseau que l'ESP32

    Vérifiez que le port 8899 est ouvert

🤝 Contribution

Les contributions sont les bienvenues ! Pour contribuer :

    Forkez le projet

    Créez votre branche (git checkout -b feature/AmazingFeature)

    Committez vos changements (git commit -m 'Add some AmazingFeature')

    Pushez vers la branche (git push origin feature/AmazingFeature)

    Ouvrez une Pull Request

📄 Licence

Ce projet est sous licence MIT - voir le fichier LICENSE pour plus de détails.
⚠️ Avertissement

Ce projet est fourni à des fins éducatives et de monitoring personnel. Utilisez-le à vos propres risques. L'auteur n'est pas responsable des dommages causés à votre onduleur ou à votre installation.
🙏 Remerciements

    LVGL pour la bibliothèque graphique

    Arduino_GFX pour le driver display

    La communauté Deye pour les informations sur le protocole Solarman V5

