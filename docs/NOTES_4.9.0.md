# Deye Monitor 4.9.0 — 16 novembre 2026

Firmware de l'écran GUITION ESP32-S3 480 × 480 compatible avec le projet.

## Nouveautés

- **V2C Trydan / Trydan Pro** : pilotage HTTP local, courant proposé, pause/reprise, modes solaires, réglages exposés par la borne, mesures et diagnostic. Les commandes sont suivies d'une relecture ; la borne conserve ses protections et sa régulation.
- **Tesla Wall Connector Gen 3 Wi-Fi** : suivi local de l'état, du véhicule branché, de la puissance estimée, des énergies, des phases, températures et alertes. **Suivi uniquement** : les commandes et tarifs restent dans l'application Tesla.
- **Recherche automatique des IP V2C/Tesla** : identification et mesures vérifiées avant enregistrement. Une borne connue est retrouvée après changement DHCP ; plusieurs réponses ou une identité différente demandent un choix explicite.
- **Veille de l'affichage sur un ou deux créneaux**, y compris à cheval sur minuit : LCD et rétroéclairage éteints, tactile actif pour un réveil temporaire. Le premier toucher n'actionne aucun bouton. Les lectures et l'accès Web continuent.
- **Rétroéclairage tout-ou-rien sans PWM ni variation**, pour écarter les problèmes de variation constatés selon les révisions matérielles. Thèmes clair/sombre manuels ou horaires conservés.
- **Prévision solaire jusqu'à huit expositions** et conservation des anciens réglages.
- **Recherche automatique de mises à jour facultative**, désactivée par défaut. Option coupée : aucune recherche automatique et aucune flèche ; vérification et installation manuelles toujours disponibles.

## Installer

1. Exporter la configuration de l'écran et conserver son précédent fichier OTA.
2. Télécharger **`DEYE_V3_4.9.0_OTA.bin`** dans les fichiers de cette release. Le fichier `SHA256SUMS-4.9.0.txt` permet d'en vérifier l'empreinte.
3. Installer le `.bin` depuis la page Web de mise à jour de l'écran, ou utiliser son parcours de mise à jour GitHub. Ne pas envoyer un ZIP, un bootloader ou ce firmware à la borne de charge.
4. Après redémarrage, vérifier le modèle, l'heure, les mesures et les options de recharge. Les anciens réglages sont conservés ; le deuxième créneau de veille reste désactivé s'il n'existait pas.

**Taille OTA :** 1 784 096 octets. **SHA-256 :** `8bdb9e830b6ee5e207012c22bb3f00939f2644c9b163c67aa1d07ef022da4b31`.

## Notices

- [Première utilisation et mise à jour de l'écran](MISE_A_JOUR_ECRAN_GITHUB.md).
- [V2C Trydan / Trydan Pro : connexion, commandes et essais](GUIDE_V2C_TRYDAN_BETA.md).
- [Tesla Gen 3 Wi-Fi : connexion et suivi](GUIDE_TESLA_GEN3_WIFI.md).
- [Veille LCD/rétroéclairage et réveil tactile](GUIDE_VEILLE_HORAIRE.md).
- [Guide utilisateur illustré](GUIDE_UTILISATEUR_V3.md) et [mémo](RECAP_UTILISATEURS_V3.md), complétés pour la 4.9.0. Les anciens PDF et certaines captures restent identifiés comme historiques.

## Validation

Compilation ESP32-S3 et tests logiciels réussis : protocoles, erreurs de communication, découverte de bornes, persistance, horaires, commandes d'affichage simulées et interfaces tactiles/Web. **Les nouvelles intégrations et la veille restent à confirmer sur les différentes bornes et révisions matérielles par les utilisateurs testeurs.** Une consigne relue ne prouve pas que le véhicule charge ; vérifier les mesures réelles lors du premier essai.

La publication contient le firmware OTA et son empreinte ; les notices sont accessibles depuis cette page et le dépôt public. Les sources et leur historique restent dans un dépôt privé.
