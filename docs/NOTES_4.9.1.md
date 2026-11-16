# Deye Monitor 4.9.1 — menus 4.8.x rétablis

Cette correction rétablit l’interface complète de la **4.8.12** et conserve les nouveautés de la 4.9.0.

## Menus et fonctions retrouvés

Les réglages retrouvent leurs **six tuiles**, en clair et en sombre : **Installation**, **Énergie**, **Pilotage**, **Réseau / Heure**, **Écran**, **Système**. La page Web reprend les mêmes rubriques et ses formulaires séparés, adaptés au téléphone.

Les pages associées sont à nouveau disponibles : production PV1 à PV8, passerelles solaires, historique et carte SD, famille Sofar en lecture seule, fuseaux UTC et heure d’été, Time of Use et anticipation batterie. Les liens de retour respectent la rubrique d’origine. Le démarrage guidé Wi-Fi/logger reste accessible dans Installation.

## Ajouts récents conservés

- **V2C Trydan / Trydan Pro** : pilotage HTTP local, réglages disponibles selon le firmware de la borne, mesures et diagnostic, avec relecture des commandes.
- **Tesla Wall Connector Gen 3 Wi-Fi** : suivi local uniquement. Les commandes de charge et les tarifs restent dans l’application Tesla.
- **Recherche automatique des IP V2C/Tesla**, avec identité et mesures vérifiées avant enregistrement.
- **Veille LCD et rétroéclairage sur un ou deux créneaux**, réveil tactile temporaire ; le premier toucher ne déclenche pas de commande.
- **Rétroéclairage fixe tout-ou-rien, sans PWM ni réglage de luminosité** ; thèmes clair/sombre manuels ou horaires.
- **Prévision solaire jusqu’à huit expositions**.
- **Recherche automatique de mises à jour facultative**, désactivée par défaut. Coupée : aucune flèche ; vérification et installation manuelles toujours disponibles.

Les ressources de navigation Web sont compressées pour conserver toutes ces fonctions dans les partitions OTA existantes.

## Installer

1. Exporter la configuration et conserver le précédent fichier OTA.
2. Télécharger **`DEYE_V3_4.9.1_OTA.bin`** et, pour vérifier l’empreinte, **`SHA256SUMS-4.9.1.txt`** dans cette release.
3. Sur l’écran : **Réglages → Système → Mise à jour**. L’installation manuelle du `.bin` reste également possible depuis la page Web.
4. Après redémarrage, vérifier **4.9.1**, les six tuiles, les mesures et les réglages de la borne. Les configurations existantes sont conservées ; un ancien réglage de veille conserve son premier créneau.

La 4.9.1 est une mise à jour distincte : les écrans en 4.9.0 peuvent la détecter. Le firmware de la 4.9.0 reste inchangé.

**OTA :** 1 960 176 octets. **SHA-256 :** `9501fbb108d3cef37a2294d013027cf1c25f7f146e91487f5f5f4f8c6ce45681`.

## Guides

- [Guide utilisateur illustré](GUIDE_UTILISATEUR_V3.md) et [mémo](RECAP_UTILISATEURS_V3.md).
- [V2C Trydan / Pro](GUIDE_V2C_TRYDAN_BETA.md).
- [Tesla Gen 3 Wi-Fi](GUIDE_TESLA_GEN3_WIFI.md).
- [Veille et réveil tactile](GUIDE_VEILLE_HORAIRE.md).
- [Première utilisation et mise à jour](MISE_A_JOUR_ECRAN_GITHUB.md).

## Vérifications et essais utilisateurs

Compilation ESP32-S3, contrôles logiciels des bornes et de la veille, rendus LVGL et parcours Web locaux. Les nouvelles intégrations et la mise en veille restent à confirmer sur les bornes et révisions matérielles des utilisateurs testeurs. Aucun essai sur matériel réel n’a été effectué pendant cette correction.

Seuls le firmware OTA, son empreinte et les guides sont publics. Les sources et leur historique restent privés.
