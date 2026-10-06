# Validation 4.3.8 — indicateurs de recharge — 6 octobre 2026

- Compilation PlatformIO `deye_unified` : SUCCESS, dépendances épinglées du dépôt.
- RAM statique : 60 512 / 327 680 octets (18,5 %).
- Application : 1 540 053 / 1 966 080 octets (78,3 %).
- OTA : 1 540 416 octets, SHA-256 `b2c706a15765219f3b2110b09879f8b6eeefe581d267131145dec37cf6669cc6`.
- esptool 4.5.1 : image ESP32-S3, checksum et hash incorporé valides.

L'ancien tableau de bord Vetronic contenait un soleil dessiné et un indicateur de câble près de la voiture. Ces objets étaient absents de la version unifiée. Ils sont rétablis dans `ui_main.h` avec un composant partagé `ui_ev_badges.h`, sans chevauchement avec la puissance.

Le test `ev_dashboard_test.cpp` vérifie les états WB01 0/1/2 : débranché, attente et charge. La coche dépend du branchement, y compris à 0 A ; le soleil dépend du mode solaire lu et peut rester visible en attente de surplus. Une mesure indisponible ou un état invalide ne donne pas de coche verte. Un mode hors ligne ne garde pas le soleil. Les modes manuel, arrêt, autonome et inconnu masquent le soleil. Deye LoRa utilise seulement le mode R489 valide et récent ; aucune information de câble n'est déduite de ses registres. L'option Aucune masque les deux indicateurs.

`ui_preview.cpp` exécute le composant d'indicateurs de production avec LVGL 8.4.0. Les assertions vérifient les symboles, la couleur verte, les transitions branchement/hors ligne, le masquage, l'absence de chevauchement et les propriétés de clic. Les deux captures du composant, `ev-badges-solar-dark.bmp` et `ev-badges-solar-light.bmp`, ont été examinées. Elles ne constituent pas une capture complète du tableau de bord sur matériel.

La suite complète réussit : neuf exécutables C++, scripts Web et comparaison des versions GitHub, puis prévisualisation LVGL des pages unifiées. Les tests de GEN MO/PV5 confirment les watts et les kWh pour les deux signes, l'exclusion SmartLoad et le calcul journalier de secours. Le test des versions conserve les cas `4.3.4-vetronic-v3`, versions invalides et erreurs GitHub/réseau.

Le pack Windows réussit les contrôles ZIP, empreintes, vérification seule et refus d'un fichier absent ou corrompu. Aucun flash, accès au port série ou commande réelle de charge n'a été effectué.

Après installation, contrôler les états branché en attente, solaire en charge, débranché et perte de liaison avec la WB01. Le soleil représente le mode solaire confirmé, pas la présence d'une puissance de charge à chaque instant.

[Guide 4.3.8](GUIDE_UNIFIE_4.3.8.md) · [Release](RELEASE_v4.3.8.md)
