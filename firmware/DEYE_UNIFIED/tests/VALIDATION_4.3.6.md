# Validation 4.3.6 — GEN MO + PV — 3 octobre 2026

- Compilation des sources du dépôt avec `pio run`, environnement `deye_unified` : SUCCESS.
- RAM statique : 60 504 / 327 680 octets (18,5 %).
- Application : 1 538 593 / 1 966 080 octets (78,3 %).
- Binaire OTA : 1 538 960 octets.
- SHA-256 : `d0b1d25ab28860718582e668c6c4e8080c1adb0a150c26fcd5b3d2cc713f9b93`.
- Vérification de l'image ESP32-S3 avec esptool 4.5.1 : checksum et hash valides.

`run_checks.ps1 -SkipPreview` : huit exécutables hôte et script Web de production réussis.

Le nouveau test utilise les fonctions de production partagées par l'écran, le Web et l'historique. Il couvre le cumul de quatre PV plus GEN, les deux signes GEN après calibration, le masquage des PV, GEN seul, SmartLoad, l'option désactivée et un profil sans GEN. Il vérifie aussi l'addition du compteur journalier GEN au compteur PV, les sources manquantes, les profils à deux PV et l'estimation sur une heure avec GEN négatif, les trous de communication et le changement de jour.

Les tests existants de profils, migration/NVS, choix de borne, horaires, relais, veille, tarifs LoRa/WB01, codecs JSON et scripts Web ont tous réussi. Le test Web utilise un DOM et un transport simulés. La disposition des pages n'a pas changé ; l'affichage GEN MO utilise la puissance positive de production.

Le pack Windows est contrôlé par le test d'archive et de toutes les empreintes, le mode de vérification seul, le refus de fichiers absents/corrompus et la sélection de ports simulés.

Aucun flash, port série réel ni commande de charge n'a été exécuté. Vérifier sur l'installation la puissance GEN et les deux totaux avec GEN MO et le cumul activés. La passerelle WB01 conserve ses propres paramètres et calculs solaires.
