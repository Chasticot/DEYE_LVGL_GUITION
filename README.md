# Deye Monitor — démarrage, utilisation et mises à jour

Deye Monitor affiche les mesures d’un onduleur DEYE sur un écran tactile GUITION ESP32-S3 480 × 480. Il fonctionne en local avec un logger compatible, sans Home Assistant ni compte cloud à renseigner dans l’écran.

## Commencer ici

![Le parcours de première utilisation en cinq étapes](docs/images/demarrage/parcours-demarrage.svg)

**La notice est illustrée :** des captures montrent où appuyer, et des schémas expliquent l’alimentation, le réseau, les mesures et les mises à jour. Commencer par le guide si c’est votre premier écran.

- **[Première mise en route et guide utilisateur](docs/MISE_A_JOUR_ECRAN_GITHUB.md)** : alimentation, Wi-Fi, modèle, logger, premières mesures.
- **[Questions fréquentes et dépannage](docs/FAQ_ECRAN.md)** : « DEYE injoignable », connexion directe sans box, IP, GEN, alimentation, encastrement, mises à jour.
- **[DL1000B-WIFI : activer Modbus TCP](docs/GUIDE_DL1000B_MODBUS_TCP.md)** : captures pas à pas et demande à Deye pour un firmware 1.7.2 minimum.
- **[Mémo de démarrage](docs/RECAP_UTILISATEURS_V3.md)** : les étapes à garder sous la main.
- **[Téléchargements et dernière version stable](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest)**.

La dernière version publiée est **4.8.6**, le **5 novembre 2026**. Elle ajoute les sources **Hoymiles** via OpenDTU, AhoyDTU ou DTU-Pro Modbus compatible, et les **micro-onduleurs Deye V118** en V5 ou Modbus TCP. **Sossen est suivi via un compteur Shelly de production dédié ; la lecture directe du micro-onduleur Sossen n’est pas intégrée.** [Notes et binaire 4.8.6](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.8.6).

Les sources supplémentaires apparaissent en **PV5 à PV8**, avec ajout au total facultatif après essai et confirmation. Les doublons entre sources Hoymiles sont détectés grâce aux numéros des micro-onduleurs. Les **tableaux de bord LCD et Web actuels restent conservés à l’identique**, ainsi que les six tuiles de réglages de la 4.8.5. [Guide des connexions et limites 4.8.6](docs/NOTES_4.8.6.md) · [Guide des sources solaires, de la microSD et des Sofar](docs/GUIDE_MICRO_SD_SOFAR.md).

Compilation et tests logiciels réussis, y compris lecteurs simulés et formulaires sur téléphone. Aucun essai sur écran, DTU, micro-onduleur ou compteur physique pendant cette validation. La compatibilité dépend du modèle, du protocole et du firmware ; elle n’est pas garantie pour toutes les variantes. Les fonctions précédentes — APsystems/Enphase, Sofar, historique et microSD, UTC/heure d’été et Time of Use Deye — restent disponibles.

## Prévision solaire en 4.8.0

Depuis la 4.8.5, ouvrir **Réglages > Énergie > Prévision solaire** (dans les anciennes versions : **Configuration > Prévision solaire**) ; renseigner la position et les panneaux puis activer la fonction. Sans compte ni clé API ; Internet est nécessaire pour recevoir les prévisions.

Toucher **PV auj. / PV dem.** ouvre les deux courbes. Choisir **Aujourd’hui**, **Demain** ou **Alternance** pour le bandeau ; l’alternance est réglable de **1 à 300 secondes**, avec **5 secondes par défaut**.

[Guide illustré de la prévision solaire](docs/GUIDE_PREVISION_SOLAIRE.md) · [Nouveautés et installation de la 4.8.0](docs/GUIDE_UNIFIE_4.8.0.md).

## Mettre un écran déjà installé à jour

1. Exporter ses réglages depuis la page Web de l’écran.
2. Avec Internet et une heure correcte, ouvrir **Réglages > Système > Mise à jour > Vérifier sur GitHub > Installer** depuis la 4.8.5 ; sur les versions précédentes, l’accès est dans **Configuration > Mise à jour**.
3. Sans ce menu, télécharger le fichier **`_OTA.bin`** de la release adaptée et l’envoyer depuis le formulaire OTA de **l’IP de l’écran**.
4. Garder l’alimentation jusqu’au redémarrage, puis vérifier version, modèle, logger et mesures.

Le fichier OTA ne remplace pas une première installation USB. Les archives automatiques **Source code** ne sont pas un programme d’installation. [Procédure complète et récupération USB](docs/MISE_A_JOUR_ECRAN_GITHUB.md#mettre-lécran-à-jour).

## Compatibilité et anciennes notices

Le modèle de l’onduleur **et** celui du logger comptent. LSW3/LSW5 et LSE disposent de retours de fonctionnement ; **le fonctionnement DL1000B avec notre écran est confirmé par un retour utilisateur**. Pour le DL1000B-WIFI, demander un firmware **1.7.2 minimum**, activer Modbus TCP dans **Link2** puis utiliser le port **502**. [Mode opératoire avec captures et demande à Deye](docs/GUIDE_DL1000B_MODBUS_TCP.md). [Vérifier son matériel](docs/MISE_A_JOUR_ECRAN_GITHUB.md#vérifier-la-compatibilité-avant-de-commencer).

Le [manuel détaillé 4.6.3](docs/GUIDE_UTILISATEUR_V3.md) et les [notes 4.7.0](docs/GUIDE_UNIFIE_4.7.0.md) restent disponibles pour les anciennes versions. Les PDF sont mis à jour pour la **4.8.0** : [guide complet illustré](docs/pdf/GUIDE_UTILISATEUR_V3.pdf) et [mémo de démarrage](docs/pdf/RECAP_UTILISATEURS_V3.pdf). Les noms de fichiers sont conservés pour les anciens liens.

Ce dépôt public distribue les **binaires OTA**, leurs empreintes SHA-256 et les guides. Les sources du firmware et leur historique restent privés. Les URL existantes des guides sont conservées pour les écrans déjà livrés.
