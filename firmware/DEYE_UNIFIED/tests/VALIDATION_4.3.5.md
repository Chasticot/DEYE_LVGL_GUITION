# Validation du firmware unifié 4.3.5 — 3 octobre 2026

## Compilation

- Sources du dépôt GitHub : `pio run` depuis sa racine, environnement par défaut `deye_unified` : SUCCESS.
- Dépendances épinglées : espressif32 6.10.0, Arduino ESP32 2.0.17, LVGL 8.4.0, Arduino_GFX 1.4.7, ArduinoJson 6.21.5.
- RAM statique : 60 504 / 327 680 octets (18,5 %).
- Application : 1 538 545 / 1 966 080 octets (78,3 %).
- Les sources compilées comprennent les deux interfaces de recharge et le catalogue Deye général.
- Journaux de compilation conservés localement, exclus des sources publiées.

## Tests exécutés

`run_checks.ps1 -SkipPreview` exécuté depuis les sources du dépôt : sept exécutables hôte et script Web de production réussis.
L'aperçu LVGL fusionné des mêmes sources a été compilé et exécuté séparément avec succès lors de la préparation du firmware.

- Catalogue des 13 références, blocs, conversions, migrations GEN et AI-W5.1 P1, isolation des réglages et erreurs de sauvegarde.
- Combinaisons modèle/borne, sélection stable après sauvegarde jusqu'au redémarrage et maintien du choix enregistré sur un modèle incompatible.
- Ancienne activation VE ambiguë, choix invalide, erreur d'écriture NVS et reprise des registres natifs avec ancien déverrouillage ignoré.
- Blocage des écritures natives hors LoRa compatible, y compris lorsqu'une ancienne permission était activée.
- Horaires sur 1 440 minutes, matrice tarifaire de 128 cas, relais, veille et énergie GEN.
- Propriété tarifaire LoRa après commande Libre locale confirmée, pause/reprise exactes, annulation après changements externes, échec, désactivation et redémarrage.
- Codec JSON des réglages : types stricts, cohérence du backend, profil compatible, anciennes sauvegardes ambiguës refusées, champs facultatifs et données malformées.
- WB01 : limites manuelles, courant mesuré et puissance estimée, fraîcheur, SOC, retour à la borne, pause/reprise et réponse POST incertaine.
- Scripts Web réels avec DOM et transport simulés : tableaux de bord Aucune/LoRa/WB01, client désactivé, CSRF, aucun POST à l'ouverture et gestion des états hors ligne/occupé.
- Aperçu LVGL 480 × 480 : sélection des trois bornes, brouillons, rejet LoRa sur LP3, accès aux pages selon backend, préservation du choix lors des autres sauvegardes, limites des contrôles, SOC et états hors ligne/occupé.
- Images des options et de la page WB01 sur SG02/LP3 inspectées visuellement, ainsi que les thèmes sombre/clair et la saisie d'adresse.

## Livraison

`DEYE_V3_4.3.5_OTA.bin` est une copie vérifiée du binaire applicatif `.pio/build/deye_unified/firmware.bin` construit depuis ce dépôt.
Taille : 1 538 912 octets. SHA-256 : `58690e333503de6f9461b1b238ab187d46b26103fbf67fa14e2162fe8438fd22`.
L'image ESP32-S3, son intégrité et la présence de la version 4.3.5 ont été contrôlées avec esptool 4.5.1.
Le fichier `SHA256SUMS-4.3.5.txt` de la release fournit les empreintes des téléchargements.
Le pack Windows a passé les contrôles ZIP, tailles et SHA-256, le mode de vérification seul et le refus de fichiers corrompus ou absents. Les tests de sélection du port Windows ont réussi avec des données simulées, sans ouverture de port série.

Les avertissements du compilateur hôte Zig concernent sa bibliothèque libunwind et n'ont pas empêché les tests.
La RAM statique ne représente pas les allocations dynamiques, la pile HTTP ou les buffers d'affichage en fonctionnement.

## Vérifications matérielles restantes

Aucun téléversement, flashage ou ordre réel de charge n'a été exécuté. Vérifier sur l'écran et l'installation les mesures du profil, la migration, les redémarrages, les commandes de la borne choisie, les règles tarifaires et les pertes de liaison.
La passerelle WB01 garde sa propre configuration Deye : changer le profil de l'écran ne configure pas sa régulation solaire.
