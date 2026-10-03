# Validation du firmware unifié 4.3.6 — 3 octobre 2026

Le correctif GEN MO/PV5 est compilé depuis `firmware/DEYE_UNIFIED/`, environnement PlatformIO `deye_unified`. Les sources conservent le choix du modèle Deye et de la borne de la 4.3.5.

## Compilation et livrables

- PlatformIO : SUCCESS avec espressif32 6.10.0, Arduino ESP32 2.0.17, LVGL 8.4.0, Arduino_GFX 1.4.7 et ArduinoJson 6.21.5.
- RAM statique : 60 504 / 327 680 octets (18,5 %).
- Application : 1 538 593 / 1 966 080 octets (78,3 %).
- Binaire OTA : 1 538 960 octets, compatible avec la taille de la partition applicative.
- SHA-256 OTA : `d0b1d25ab28860718582e668c6c4e8080c1adb0a150c26fcd5b3d2cc713f9b93`.
- Image ESP32-S3 vérifiée avec esptool 4.5.1 : checksum et hash incorporé valides.

La RAM statique ne couvre pas toutes les allocations en fonctionnement. La procédure Arduino IDE reste documentée ; le build publié utilise PlatformIO.

## Tests exécutés

Huit exécutables hôte et les scripts Web de production ont réussi via `run_checks.ps1 -SkipPreview`.

Le test GEN MO/PV5 vérifie le cumul de quatre PV plus GEN, les deux signes après calibration, les coefficients des profils Deye et une calibration personnalisée. Il couvre PV masqués, GEN seul, SmartLoad, le cumul désactivé et les profils sans GEN.

Les deux totaux sont vérifiés : puissance instantanée et kWh du jour avec compteur GEN. L'estimation sans compteur journalier est vérifiée sur une heure avec une mesure GEN négative, sans intégration des trous de communication et avec remise à zéro au changement de jour. Les données journalières indisponibles restent indisponibles.

Les autres tests couvrent les profils, les sauvegardes/migrations NVS, le choix de borne, les horaires, relais et veille, les tarifs LoRa/WB01, les codecs JSON et le Web avec DOM et transport simulés. Le détail figure dans [le compte rendu des tests](../firmware/DEYE_UNIFIED/tests/VALIDATION_4.3.6.md).

Le pack Windows a passé les contrôles ZIP, tailles et empreintes, le mode de vérification seul et le refus de fichiers absents ou corrompus. La sélection de ports Windows a été testée avec des ports simulés. Aucun port série réel n'a été ouvert.

## Vérifications matérielles restantes

Aucun flash ni commande de charge réelle n'a été exécuté. Comparer la puissance GEN, la somme PV + GEN et leurs kWh du jour à l'installation réelle, avec **GEN MO** et **Cumuler GEN MO + PV** activés.

Le calcul partagé alimente l'écran, le Web et l'historique. La passerelle WB01 possède sa propre régulation solaire et sa propre configuration Deye ; ce correctif concerne le firmware de l'écran.

[Guide 4.3.6](GUIDE_UNIFIE_4.3.6.md) · [Notes de release](RELEASE_v4.3.6.md) · [Validation historique 4.3.5](VALIDATION_4.3.5.md)
