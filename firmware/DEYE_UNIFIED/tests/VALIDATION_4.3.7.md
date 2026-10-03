# Validation 4.3.7 — vérification des versions GitHub — 3 octobre 2026

- Compilation PlatformIO `deye_unified` : SUCCESS avec les dépendances épinglées du dépôt.
- RAM statique : 60 504 / 327 680 octets (18,5 %).
- Application : 1 538 897 / 1 966 080 octets (78,3 %).
- OTA : 1 539 264 octets, SHA-256 `13a0bc09173f9deac2a25b74b6abebbfc37612e05f68c95dda42b62708509ab0`.
- Image ESP32-S3 vérifiée avec esptool 4.5.1 : checksum et hash incorporé valides.

Le test `ota_version_test.cjs` extrait et exécute le JavaScript réel inclus dans `web_server.h`. Avant correction, `4.3.4-vetronic-v3` reproduisait l'erreur de lecture d'une valeur `null` pendant la comparaison. Après correction, ses quatorze cas réussissent : suffixe historique, préfixe v, métadonnées, version identique ou supérieure, comparaison numérique 9/10, version inconnue, tag invalide, réponse GitHub vide, HTTP 404/403 et erreur réseau. Toutes les requêtes sont des lectures GET ; aucune installation n'est déclenchée.

Les tests Web de production existants passent également : tableaux de bord et bornes, CSRF, absence de POST à l'ouverture, mesures et états hors ligne, SOC et tarifs. Les fonctions C++ et le cumul GEN MO/PV5 de la 4.3.6 sont conservés ; leur validation est décrite dans [VALIDATION_4.3.6.md](../../../docs/VALIDATION_4.3.6.md).

Le pack Windows est vérifié par les contrôles ZIP, tailles et empreintes, le mode de vérification seul et le refus de fichiers absents ou corrompus. Aucun flash ni commande réelle de charge n'a été effectué.

Pour un appareil sous 4.3.4 Vetronic V3, télécharger le fichier OTA manuellement : son ancien bouton contient toujours le code précédent jusqu'à la mise à jour. Après migration initiale vers le firmware unifié, sélectionner explicitement la borne.

[Guide 4.3.7](../../../docs/GUIDE_UNIFIE_4.3.7.md) · [Release](../../../docs/RELEASE_v4.3.7.md)
