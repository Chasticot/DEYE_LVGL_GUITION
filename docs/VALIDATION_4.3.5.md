# Validation du firmware unifié 4.3.5 — 3 octobre 2026

Cette page concerne la version actuelle, compilée à partir de `firmware/DEYE_UNIFIED/` avec l'environnement `deye_unified`. Les anciennes sources V3 4.3.3 et Vetronic V3 4.3.4 restent historiques ; leur validation ne doit pas être confondue avec les résultats ci-dessous.

## Sources unifiées : compilation et tests logiciels

Les sources de travail 4.3.5 ont été compilées avec succès dans le projet PlatformIO principal et leur projet autonome. Les deux interfaces de recharge et le catalogue général Deye sont inclus.

- RAM statique : **60 504 / 327 680 octets**, soit 18,5 %.
- Application du projet principal de travail : **1 538 921 / 1 966 080 octets**, soit 78,3 %.
- Sept exécutables hôte et tests Web de production réussis.
- Aperçu LVGL fusionné compilé et exécuté avec succès, puis inspection visuelle des pages SG02/LP3 et WB01.

Ces chiffres décrivent les compilations du projet de travail. La compilation du dépôt de publication et les contrôles du pack sont distingués ci-dessous. La RAM statique n'inclut pas les allocations dynamiques, piles HTTP et buffers d'affichage.

## Périmètre des tests réussis

- Catalogue des 13 références, 12 profils sélectionnables, conversions, blocs Modbus et migrations GEN/AI-W5.1 P1.
- Isolation des réglages par modèle, erreurs de sauvegarde et maintien d'un choix mémorisé devenu incompatible.
- Compatibilité modèle/borne, ancienne activation VE ambiguë, choix explicite et stabilité du backend jusqu'au redémarrage.
- Reprise des adresses natives personnalisées sans reprise de l'ancien déverrouillage.
- Blocage des commandes natives hors LoRa compatible, y compris avec ancienne permission active.
- Horaires sur 1 440 minutes, matrice tarifaire de 128 cas, relais, veille et énergie GEN.
- Propriété tarifaire LoRa/WB01 après commande locale confirmée, pauses/reprises, annulation après modification externe, échec ou redémarrage.
- JSON de configuration : aller-retour, types stricts, cohérence modèle/borne, anciennes sauvegardes ambiguës refusées, champs facultatifs et entrées malformées.
- Protocole WB01 : limites manuelles, fraîcheur, courant mesuré, estimation de puissance, SOC, retour à la borne et réponse POST incertaine.
- Scripts Web réels avec DOM et transport simulés : tableaux de bord Aucune/LoRa/WB01, backend inactif, CSRF, absence de POST à l'ouverture et états hors ligne/occupé.
- LVGL 480 × 480 : trois choix de borne, brouillons, compatibilité SG02/LP3, routage et garde des pages, sauvegardes hors sélection, limites des contrôles, SOC et états hors ligne/occupé.
- Inspection visuelle des options, de la liste de bornes et de la page WB01, thèmes sombre/clair et saisie d'adresse.

Les avertissements de compilation hôte Zig relatifs à libunwind n'ont pas empêché la réussite des tests. Le compte rendu des sources figure aussi dans [VALIDATION_4.3.5.md](../firmware/DEYE_UNIFIED/tests/VALIDATION_4.3.5.md).

## Dépôt de publication et pack Windows

La compilation PlatformIO des sources publiées a réussi avec les dépendances déclarées, ainsi que les sept exécutables hôte et le script Web de production exécutés depuis cette structure.

- RAM statique du dépôt publié : **60 504 / 327 680 octets** (18,5 %).
- Application du dépôt publié : **1 538 545 / 1 966 080 octets** (78,3 %).
- Fichier binaire OTA : **1 538 912 octets**.
- SHA-256 du binaire OTA : `58690e333503de6f9461b1b238ab187d46b26103fbf67fa14e2162fe8438fd22`.
- Contrôle esptool de l'image ESP32-S3, checksum et hash incorporé : réussite.

Le dépôt utilise des dépendances fixées : ESP32 Arduino 2.0.17, LVGL 8.4.0, GFX Library for Arduino 1.4.7 et ArduinoJson 6.21.5. Les commandes reproductibles figurent dans le [guide PlatformIO](COMPILATION_PLATFORMIO.md).

Le pack comprend l'application, le bootloader, les partitions, le sélecteur OTA et esptool 4.5.1. Son manifeste et `SHA256SUMS-4.3.5.txt` donnent les empreintes des livrables. Le mode `-VerificationSeule` de l'installateur contrôle les fichiers sans accéder à un écran. Une empreinte ne constitue pas une signature numérique.

Le pack unifié a passé les contrôles ZIP, de toutes les tailles et empreintes, du mode de vérification seul et du refus de fichiers corrompus ou absents. Les tests de sélection du port Windows ont également réussi, avec des ports simulés. Aucun port série réel n'a été ouvert.

La procédure Arduino IDE est documentée ; les compilations Arduino historiques des variantes précédentes ne constituent pas une preuve de compilation Arduino du firmware unifié 4.3.5.

## Vérifications matérielles restantes

**Aucun téléversement, flashage ni ordre réel de charge n'a été exécuté pour valider cette version.** Les tests hôte utilisent des données et transports simulés.

Sur l'installation réelle, vérifier le profil et ses mesures au LCD, la migration, les sélections après redémarrage, les commandes de la borne choisie, les restrictions tarifaires et les pertes de liaison. Contrôler les allocations mémoire et le fonctionnement prolongé sur l'écran.

Avec WB01, vérifier courant, retour à la borne, régulation solaire et protection SOC. Sa passerelle garde sa propre configuration Deye : changer le modèle sur l'écran ne met pas à jour ses registres ni ses coefficients. L'accès au client WB01 avec tous les profils de l'écran ne valide pas toutes ces associations sur matériel.

Le retour utilisateur AI-W5.1 ESS P1 du 2 octobre 2026 concerne les registres intégrés au profil ; il ne couvre pas toutes les révisions AI-W5.1 ni P3, ni les nouvelles fonctions de recharge.

## Publications historiques

- [V3 4.3.3](RELEASE_v4.3.3.md) : publication initiale, préparation PlatformIO/Arduino, profils et packs Windows.
- [Vetronic V3 4.3.4](VALIDATION_VETRONIC_V3.md) : validation dédiée à l'ancienne édition SG02LP1/WB01.
- Les guides et PDF V3 historiques décrivent leurs versions d'origine. Pour le choix de borne et la migration actuelle, suivre le [guide unifié 4.3.5](GUIDE_UNIFIE_4.3.5.md).
