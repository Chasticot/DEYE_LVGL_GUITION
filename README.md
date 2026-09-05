# DEYE LVGL GUITION

Interface tactile autonome pour écran GUITION 480 x 480 basé sur ESP32-S3, destinée au suivi d'un onduleur Deye via logger Solarman.

![Écran DEYE LVGL GUITION](https://github.com/user-attachments/assets/73ff4867-a7e3-4e30-a7ba-2dfac98bcd35)

Le projet affiche localement les principales mesures de l'installation photovoltaïque et permet de consulter les informations de l'onduleur sans dépendre d'un ordinateur ou d'un serveur domotique.

## État du projet

La branche `main` contient le développement actuel avec l'intégration VE. Le reste de l'interface est opérationnel, mais les commandes VE doivent encore être confirmées sur l'onduleur avant d'être considérées comme définitives.

La base stable sans TEMPO ni VE est conservée dans le tag [`V1_base_LSW`](https://github.com/Chasticot/DEYE_LVGL_GUITION/tree/V1_base_LSW). Elle sert de référence pour les installations qui souhaitent uniquement le suivi LSW/LSE classique.

## Fonctions principales

- Affichage en temps réel de la production PV, de la batterie, de la consommation et du réseau.
- Indication de l'import et de l'export réseau avec distinction des puissances positives et négatives.
- Suivi de l'état de charge, des tensions, puissances et températures.
- Bilans journaliers de production, consommation, achat et vente d'énergie.
- Écran de configuration Wi-Fi avec scan des réseaux et indicateur de qualité.
- Configuration de l'adresse IP, du port, du mode de communication et du numéro de série du logger.
- Synchronisation NTP et réglage du fuseau horaire.
- Interface tactile LVGL fluide avec indicateurs de connexion Wi-Fi et Deye.
- Sauvegarde des paramètres dans la mémoire non volatile de l'ESP32.
- Communication Solarman V5 pour LSW et Modbus TCP pour LSE selon la configuration utilisée.
- Intégration VE en cours de validation dans `main`.

## Matériel

- Écran GUITION ESP32-S3 480 x 480, par exemple ESP32-4848S040.
- PSRAM OPI activée.
- Logger Solarman LSW3 ou LSE relié au même réseau que l'écran.
- Onduleur Deye compatible avec le profil de registres utilisé par le projet.

## Câblage par défaut

Les broches principales sont définies dans `config.h` :

| Fonction | GPIO |
| --- | ---: |
| GT911 SDA | 19 |
| GT911 SCL | 45 |
| Rétroéclairage | 38 |
| Adresse GT911 | `0x5D` |

Les broches du panneau RGB dépendent du modèle d'écran. Vérifiez toujours le brochage de votre carte avant le premier flash.

## Préparer Arduino IDE

1. Installer le support de cartes ESP32 depuis l'URL officielle Espressif :
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. Sélectionner `ESP32S3 Dev Module` ou le modèle correspondant à votre carte.
3. Installer `lvgl` version 8.4.0 et `Arduino_GFX` version 1.4.7.
4. Utiliser les réglages suivants :

| Réglage | Valeur recommandée |
| --- | --- |
| USB CDC On Boot | Enabled |
| CPU Frequency | 240 MHz (WiFi) |
| PSRAM | OPI PSRAM |
| Partition Scheme | Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS) |
| Upload Speed | 921600 |

## Configuration

Les paramètres par défaut se trouvent dans `config.h`. Pour éviter de publier des identifiants personnels, les valeurs Wi-Fi et le numéro de série peuvent rester génériques dans le dépôt public. Après le premier démarrage, configurez-les depuis l'écran de réglages ; ils sont ensuite conservés dans la mémoire de l'ESP32.

Ne partagez jamais un vrai mot de passe Wi-Fi dans un dépôt public.

## Compiler et flasher

Ouvrir le sketch correspondant à la branche utilisée :

- `DEYE_LVGL_UI_2x2_menu_avance_coef.ino` pour le tag `V1_base_LSW`.
- `DEYE_LVGL_UI_2x2_menu_avance_coef_LSE_LSW.ino` pour `main`.

Compiler puis téléverser avec les réglages ESP32-S3 ci-dessus. Un effacement complet de la flash n'est pas recommandé si les paramètres sauvegardés doivent être conservés.

Les fichiers `.bin` de flash seront publiés dans les releases GitHub après validation de la compilation de chaque version. Un flash complet utilise le firmware applicatif, le bootloader et le fichier de partitions correspondant à la même version.

## Organisation

```text
.
├── DEYE_LVGL_UI_2x2_menu_avance_coef_LSE_LSW.ino
├── config.h
├── app_data.h
├── deye_solarman.h
├── settings.h
├── wifi_manager.h
├── ui_main.h
├── ui_settings.h
├── ui_ve_deye.h
├── ve_deye.h
├── ve_modbus_codec.h
├── VE_INTEGRATION.md
└── pc_simulator/
```

## Documentation VE

Les détails techniques, les limites connues et les points restant à valider sont regroupés dans [`VE_INTEGRATION.md`](VE_INTEGRATION.md).

## Remerciements

Le projet s'appuie notamment sur LVGL, Arduino_GFX, les travaux pySolarmanV5 et les échanges de la communauté Jeedom et Home Assistant autour du protocole Solarman V5.

## Licence

Ce projet est distribué sous licence MIT. Utilisez-le à vos propres risques.
