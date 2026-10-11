# Validation 4.4.1 — consigne de recherche du logger — 11 octobre 2026

- Seules la consigne tactile, la version et la documentation changent par rapport à la 4.4.0. La logique de découverte et de récupération automatique est conservée.
- Compilation PlatformIO des sources exactes de publication, environnement `deye_unified` : réussie.
- Binaire OTA : 1558240 octets ; SHA-256 `9040e687be554c8518f7036e5c2c4611309deb4cbea2dadf44aec0559e7ec861`.
- Page tactile de production rendue dans LVGL, thèmes clair/sombre : consigne sur deux lignes sans chevauchement avec les diagnostics ou les boutons. Les autres états de découverte restent vérifiés.
- Comparaison de versions OTA : tests existants réussis.
- Pack Windows : ZIP, manifest et empreintes vérifiés ; test du contrôle sans port série et du refus des fichiers absents/corrompus réussi.
- esptool 4.5.1 : checksum et hash incorporé du binaire ESP32-S3 valides.

## Écran de l’utilisateur

La correction du texte a été compilée puis flashée sur l’écran en COM16 avant la publication : écriture et empreinte vérifiées, paramètres conservés, redémarrage matériel effectué. Ce flash utilisait encore le numéro 4.4.0 ; les fichiers de cette release sont recompilés en 4.4.1. L’affichage a été vérifié par rendu LVGL ; aucune capture de l’écran physique n’a été reçue.

## Portée et validation précédente

Les tests complets des fonctions communes, du service réseau et de la sauvegarde automatique restent ceux de [4.4.0](VALIDATION_4.4.0.md), car cette correction ne les modifie pas. La découverte manuelle précédente avait été confirmée sur le logger par l’utilisateur. Le scénario complet changement DHCP, sauvegarde automatique et redémarrage reste à vérifier sur matériel.

[Guide 4.4.1](GUIDE_UNIFIE_4.4.1.md) · [Release](RELEASE_v4.4.1.md)
