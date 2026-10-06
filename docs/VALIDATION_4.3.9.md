# Validation 4.3.9 — réveil spontané en veille — 6 octobre 2026

- Compilation PlatformIO `deye_unified` depuis les sources du dépôt de publication : SUCCESS, dépendances épinglées.
- RAM statique : 60 520 / 327 680 octets (18,5 %).
- Application : 1 540 441 / 1 966 080 octets (78,4 %).
- OTA : 1 540 800 octets, SHA-256 `0a2953ac9474d471388db700bc909b1abaf287f6d73e78f6eef262cbdaeb94a6`.
- esptool 4.5.1 : image ESP32-S3, checksum et hash incorporé valides.

## Réveil sans toucher l’écran

Le retour utilisateur concerne la 4.3.7. Le défaut reste présent en 4.3.8 : `getLocalTime(..., 0)` peut échouer si `millis()` avance avant le premier contrôle de délai, même lorsque l’heure système est correcte. L’écran sort alors de veille ; la lecture suivante lui accorde la durée de réveil, soit 60 secondes par défaut.

`clock_local_time.h` remplace cette lecture par un instantané de l’horloge système et une conversion locale sans délai. La validation d’année et les conditions NTP des appelants sont conservées. La luminosité, les tarifs et les lectures journalières GEN utilisent également cette fonction.

Le test `clock_sleep_test.cpp` reproduit l’échec du délai zéro et la minute de réveil indue. Avec la nouvelle lecture, mille contrôles successifs restent en veille. Il vérifie aussi une sortie normale de plage, un réveil manuel, l’expiration de sa durée et le rejet d’une horloge invalide.

## Tactile et diagnostics

Le réveil demande au moins trois trames valides sur 80 ms, sans intervalle de plus de 120 ms entre trames. Un tap très bref peut nécessiter un appui légèrement plus long. Les commandes sur l’écran déjà allumé restent immédiates. Le geste de réveil reste absorbé.

Le pilote distingue une trame non prête d’un relâchement explicite, rejette plus de cinq contacts et les coordonnées hors écran, et refuse de confirmer une trame dont l’acquittement I2C échoue. Les traces `[DISPLAY]` indiquent la cause du réveil et le retour en veille.

`touch_wake_test.cpp` couvre les limites de coordonnées, les trames invalides, les contacts isolés, les lectures sans données, le relâchement, les erreurs, le réveil confirmé et sa durée, ainsi que le débordement de `millis()`.

## Contrôles des livrables

Les onze exécutables C++ et les deux scripts Web réussissent depuis les sources de publication. Les contrôles couvrent également les profils, les systèmes de recharge, GEN MO/PV5, les tarifs, l’import JSON, WB01, les indicateurs de recharge et la comparaison des versions GitHub. Aucune modification visuelle n’a été faite dans cette version ; les validations LVGL de la 4.3.8 restent historiques.

Le pack Windows réussit les contrôles ZIP et SHA-256, la vérification seule de l’installateur, et le refus d’un firmware corrompu ou d’un bootloader absent. Aucun port série ouvert, flash ni commande réelle de charge effectué.

Le défaut logiciel est reproduit, mais sa responsabilité dans le retour de cet utilisateur et le confort du filtre tactile restent à confirmer sur son écran. Après mise à jour, contrôler la veille prolongée sans interaction, le réveil par un bref appui et le retour en veille au délai configuré.

[Guide 4.3.9](GUIDE_UNIFIE_4.3.9.md) · [Release](RELEASE_v4.3.9.md) · [Validation précédente](VALIDATION_4.3.8.md)
