# Validation 4.3.10 — GEN Daily et relais — 10 octobre 2026

- Compilation PlatformIO `deye_unified` depuis les sources du dépôt de publication : SUCCESS.
- Application OTA : 1 541 680 octets ; SHA-256 `bbea91fa8e1b9351c6a33024d3e4b34098eb5cf2385f140d6977610fa523a80a`.
- Douze exécutables C++ hôte, deux scripts Web et rendu LVGL : PASS.
- Assertions C++/LVGL activées explicitement avec `-UNDEBUG` ; Zig définit autrement NDEBUG avec `-O1`.
- Pack Windows : empreintes et manifest vérifiés ; refus des fichiers absents/corrompus testé sans port série.
- esptool 4.5.1 : image ESP32-S3, checksum et hash incorporé valides.

## Lecture et conservation des paramètres

Le test du lecteur auxiliaire réel vérifie les modes SMARTLOAD et GEN MO, la lecture optionnelle en SMARTLOAD, les coefficients, l’absence de synchronisation NTP, l’échec d’une lecture, le changement de jour et les profils sans GEN.

Les tests de migration NVS vérifient que les anciens paramètres PV4 et relais sont conservés, même avec des octets de remplissage non nuls. L’option GEN Daily reste désactivée à la migration puis persiste après sauvegarde et rechargement. Les tests JSON couvrent l’export/import de l’option, les types invalides et les anciens exports sans ce champ.

## Affichage

Le bandeau est contrôlé dans les thèmes clair et sombre, avec SMARTLOAD ON/OFF, GEN valide/indisponible et des valeurs de 0 à 999,9 kWh. Les mesures des libellés vérifient le maintien sur une ligne, l’absence de chevauchement et le respect des bords. Les températures conservent leur police ; le libellé SMARTLOAD/GEN utilise la police 14 avec l’option activée, 16 sinon. La précision d’une décimale est conservée quand nécessaire.

Le test du JavaScript Web de production vérifie que le relais disparaît quand sa règle est désactivée, affiche OFF/ON avec la règle active et disparaît après désactivation. Le libellé tactile est masqué dès la création et lors des actualisations selon la même option.

## Livrables et limites

Le firmware OTA, le pack Windows, le guide et leurs empreintes SHA-256 sont générés depuis cette version. Les tests existants des profils, sources PV/GEN, tarifs, WB01, veille/tactile et vérification OTA restent valides.

La validation sur l’écran physique reste à effectuer. Aucun appareil n’a été flashé ni aucune commande de recharge envoyée pour cette publication.

[Guide 4.3.10](GUIDE_UNIFIE_4.3.10.md) · [Release](RELEASE_v4.3.10.md) · [Validation précédente](VALIDATION_4.3.9.md)
