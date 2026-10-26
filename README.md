# Deye Monitor — démarrage, utilisation et mises à jour

Deye Monitor affiche les mesures d’un onduleur DEYE sur un écran tactile GUITION ESP32-S3 480 × 480. Il fonctionne en local avec un logger compatible, sans Home Assistant ni compte cloud à renseigner dans l’écran.

## Commencer ici

![Le parcours de première utilisation en cinq étapes](docs/images/demarrage/parcours-demarrage.svg)

**La notice est illustrée :** des captures montrent où appuyer, et des schémas expliquent l’alimentation, le réseau, les mesures et les mises à jour. Commencer par le guide si c’est votre premier écran.

- **[Première mise en route et guide utilisateur](docs/MISE_A_JOUR_ECRAN_GITHUB.md)** : alimentation, Wi-Fi, modèle, logger, premières mesures.
- **[Questions fréquentes et dépannage](docs/FAQ_ECRAN.md)** : « DEYE injoignable », connexion directe sans box, IP, GEN, alimentation, encastrement, mises à jour.
- **[Mémo de démarrage](docs/RECAP_UTILISATEURS_V3.md)** : les étapes à garder sous la main.
- **[Téléchargements et dernière version stable](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest)**.

La dernière version stable est **4.8.0**, publiée le **26 octobre 2026**. Elle ajoute la prévision solaire d’aujourd’hui et de demain, leurs courbes et le choix de la valeur du bandeau. [Notes et binaire 4.8.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.8.0).

## Prévision solaire en 4.8.0

Dans **Configuration > Prévision solaire**, renseigner la position et les panneaux puis activer la fonction. Sans compte ni clé API ; Internet est nécessaire pour recevoir les prévisions.

Toucher **PV auj. / PV dem.** ouvre les deux courbes. Choisir **Aujourd’hui**, **Demain** ou **Alternance** pour le bandeau ; l’alternance est réglable de **1 à 300 secondes**, avec **5 secondes par défaut**.

[Guide illustré de la prévision solaire](docs/GUIDE_PREVISION_SOLAIRE.md) · [Nouveautés et installation de la 4.8.0](docs/GUIDE_UNIFIE_4.8.0.md).

## Mettre un écran déjà installé à jour

1. Exporter ses réglages depuis la page Web de l’écran.
2. Avec Internet et une heure correcte, ouvrir **Configuration > Mise à jour > Vérifier sur GitHub > Installer**.
3. Sans ce menu, télécharger le fichier **`_OTA.bin`** de la release adaptée et l’envoyer depuis le formulaire OTA de **l’IP de l’écran**.
4. Garder l’alimentation jusqu’au redémarrage, puis vérifier version, modèle, logger et mesures.

Le fichier OTA ne remplace pas une première installation USB. Les archives automatiques **Source code** ne sont pas un programme d’installation. [Procédure complète et récupération USB](docs/MISE_A_JOUR_ECRAN_GITHUB.md#mettre-lécran-à-jour).

## Compatibilité et anciennes notices

Le modèle de l’onduleur **et** celui du logger comptent. LSW3/LSW5 et LSE disposent de retours de fonctionnement ; **DL1000B-WIFI reste à valider sur le matériel concerné**, même si un mode existe dans le logiciel. [Vérifier son matériel](docs/MISE_A_JOUR_ECRAN_GITHUB.md#vérifier-la-compatibilité-avant-de-commencer).

Le [manuel détaillé 4.6.3](docs/GUIDE_UTILISATEUR_V3.md) et les [notes 4.7.0](docs/GUIDE_UNIFIE_4.7.0.md) restent disponibles pour les anciennes versions. Les PDF historiques sont datés 4.6.3 ; pour une mise en route actuelle, utiliser les liens ci-dessus.

Ce dépôt public distribue les **binaires OTA**, leurs empreintes SHA-256 et les guides. Les sources du firmware et leur historique restent privés. Les URL existantes des guides sont conservées pour les écrans déjà livrés.
