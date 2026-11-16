# Deye Monitor — démarrage, utilisation et mises à jour

**Notices des bornes et de la veille :** [V2C Trydan / Pro](docs/GUIDE_V2C_TRYDAN_BETA.md) · [Tesla Gen 3 Wi-Fi](docs/GUIDE_TESLA_GEN3_WIFI.md) · [Veille et réveil tactile](docs/GUIDE_VEILLE_HORAIRE.md).

> **Premières mesures :** [Démarrage rapide Wi-Fi + logger](docs/DEMARRAGE_RAPIDE.md) · [Fiche PDF à imprimer](docs/pdf/DEMARRAGE_RAPIDE.pdf) · [Pas à pas illustré](docs/DEMARRAGE_SIMPLE.md).

> **Documentation illustrée du 16 novembre 2026 :** [guide des menus](docs/GUIDE_UTILISATEUR_V3.md), [guide PDF](docs/pdf/GUIDE_UTILISATEUR_V3.pdf) et [mémo](docs/RECAP_UTILISATEURS_V3.md).

Deye Monitor affiche les mesures d’un onduleur DEYE sur un écran tactile GUITION ESP32-S3 480 × 480. Il fonctionne en local avec un logger compatible, sans Home Assistant ni compte cloud à renseigner dans l’écran.

<!-- galerie-sombre-debut -->
## Le projet en images — mode sombre

Interface **4.9.2** sur écran 480 × 480. Captures rendues depuis le firmware avec des données de démonstration ; les valeurs, adresses et états ne décrivent pas une installation réelle. Cliquer sur une image pour l’agrandir.

<p align="center"><a href="docs/images/presentation/dashboard-sombre.png"><img src="docs/images/presentation/dashboard-sombre.png" alt="Tableau de bord principal — mode sombre" width="480"></a><br><em>Tableau de bord principal</em></p>

<table>
<tr>
<td align="center"><a href="docs/images/presentation/reglages-sombre.png"><img src="docs/images/presentation/reglages-sombre.png" alt="Les six rubriques de réglages — mode sombre" width="250"></a><br><em>Les six rubriques de réglages</em></td>
<td align="center"><a href="docs/images/presentation/installation-sombre.png"><img src="docs/images/presentation/installation-sombre.png" alt="Installation et sources solaires — mode sombre" width="250"></a><br><em>Installation et sources solaires</em></td>
<td align="center"><a href="docs/images/presentation/pilotage-sombre.png"><img src="docs/images/presentation/pilotage-sombre.png" alt="Recharge, tarifs et batterie — mode sombre" width="250"></a><br><em>Recharge, tarifs et batterie</em></td>
</tr>
<tr>
<td align="center"><a href="docs/images/presentation/prevision-sombre.png"><img src="docs/images/presentation/prevision-sombre.png" alt="Prévisions solaires et courbes — mode sombre" width="250"></a><br><em>Prévisions solaires et courbes</em></td>
<td align="center"><a href="docs/images/presentation/expositions-sombre.png"><img src="docs/images/presentation/expositions-sombre.png" alt="Jusqu’à huit expositions solaires — mode sombre" width="250"></a><br><em>Jusqu’à huit expositions solaires</em></td>
<td align="center"><a href="docs/images/presentation/historique-sombre.png"><img src="docs/images/presentation/historique-sombre.png" alt="Historique et carte microSD — mode sombre" width="250"></a><br><em>Historique et carte microSD</em></td>
</tr>
<tr>
<td align="center"><a href="docs/images/presentation/tou-sombre.png"><img src="docs/images/presentation/tou-sombre.png" alt="Time of Use de l’onduleur — mode sombre" width="250"></a><br><em>Time of Use de l’onduleur</em></td>
<td align="center"><a href="docs/images/presentation/v2c-sombre.png"><img src="docs/images/presentation/v2c-sombre.png" alt="Recharge V2C Trydan / Pro — mode sombre" width="250"></a><br><em>Recharge V2C Trydan / Pro</em></td>
<td align="center"><a href="docs/images/presentation/tesla-sombre.png"><img src="docs/images/presentation/tesla-sombre.png" alt="Suivi Tesla Gen 3 Wi-Fi — mode sombre" width="250"></a><br><em>Suivi Tesla Gen 3 Wi-Fi</em></td>
</tr>
</table>
<!-- galerie-sombre-fin -->

## Commencer ici

![Le parcours de première utilisation en cinq étapes](docs/images/demarrage/parcours-demarrage.svg)

**La notice est illustrée :** des captures montrent où appuyer, et des schémas expliquent l’alimentation, le réseau, les mesures et les mises à jour. Commencer par le guide si c’est votre premier écran.

- **[Première mise en route et guide utilisateur](docs/MISE_A_JOUR_ECRAN_GITHUB.md)** : alimentation, Wi-Fi, modèle, logger, premières mesures.
- **[Questions fréquentes et dépannage](docs/FAQ_ECRAN.md)** : « DEYE injoignable », connexion directe sans box, IP, GEN, alimentation, encastrement, mises à jour.
- **[DL1000B-WIFI : activer Modbus TCP](docs/GUIDE_DL1000B_MODBUS_TCP.md)** : captures pas à pas et demande à Deye pour un firmware 1.7.2 minimum.
- **[Mémo de démarrage](docs/RECAP_UTILISATEURS_V3.md)** : les étapes à garder sous la main.
- **[Téléchargements et dernière version stable](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest)**.

La dernière version publiée est **4.9.2**, le **16 novembre 2026** : sauvegarde complétée pour PV5 à PV8 et Sofar, correction Time of Use et ressources Web compressées. Les menus et fonctions de la 4.9.1 sont conservés. [Notes 4.9.2](docs/NOTES_4.9.2.md) · [Nouveautés, installation et limites 4.9.2](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.9.2).

**Sauvegarde privée :** en 4.9.2, le JSON inclut aussi les clés et jetons des passerelles PV5 à PV8. Le conserver en lieu sûr et ne pas le partager publiquement. Les mots de passe Wi-Fi, identifiants Web et CSV de la carte SD restent exclus.

La gestion de la microSD (formatage FAT32 confirmé, retrait sécurisé et écritures groupées) et la recharge anticipée avec ou sans Tempo restent disponibles. [Guide de ces fonctions](docs/NOTES_4.8.9.md).

Sur l’écran : **Réglages > Énergie > Historique / carte SD** et **Réglages > Pilotage > Batterie / anticipation**. Sur le Web : **Énergie > Historique et carte SD** et **Pilotage > Time of Use · batterie**. Vérifier les horloges, le mode batterie SOC, le TOU actif et au moins une plage entièrement comprise dans les HC. Les notices Deye présentent un début à 00h00 ou 01h00 selon les modèles ; ce n’est pas une confirmation d’un bug universel.

Les **tableaux de bord LCD et Web restent conservés à l’identique**, ainsi que les six tuiles de réglages. La lecture directe Sossen Tuya 3.5 reste en essai : [procédure et limites 4.8.7](docs/NOTES_4.8.7.md). Les sources Hoymiles, Deye, APsystems, Enphase et Sofar, UTC/heure d’été et les autres fonctions précédentes restent disponibles. [Guide des sources solaires et de la microSD](docs/GUIDE_MICRO_SD_SOFAR.md).

Compilation et tests logiciels réussis. **Les essais physiques du formatage sur microSD et d’un cycle réel de recharge sur Deye restent à réaliser.** L’automatisme de batterie est désactivé par défaut ; vérifier son premier cycle et la restauration des consignes sur le LCD de l’onduleur.

## Prévision solaire en 4.9.2

Depuis la 4.8.5, ouvrir **Réglages > Énergie > Prévision solaire** (dans les anciennes versions : **Configuration > Prévision solaire**) ; renseigner la position et jusqu'à **huit expositions** (puissance, inclinaison et orientation par groupe), puis activer la fonction. Les courbes et totaux sont additionnés ; laisser les groupes inutilisés à 0 kWc. Sans compte ni clé API ; Internet est nécessaire pour recevoir les prévisions.

Toucher **PV auj. / PV dem.** ouvre les deux courbes. Choisir **Aujourd’hui**, **Demain** ou **Alternance** pour le bandeau ; l’alternance est réglable de **1 à 300 secondes**, avec **5 secondes par défaut**.

[Guide illustré de la prévision solaire](docs/GUIDE_PREVISION_SOLAIRE.md) · [Nouveautés et installation de la 4.8.0](docs/GUIDE_UNIFIE_4.8.0.md).

## Mettre un écran déjà installé à jour

1. Exporter ses réglages depuis la page Web de l’écran.
2. Avec Internet et une heure correcte, ouvrir **Réglages > Système > Mise à jour > Vérifier sur GitHub > Installer** depuis la 4.8.5 ; sur les versions précédentes, l’accès est dans **Configuration > Mise à jour**.
3. Sans ce menu, télécharger le fichier **`_OTA.bin`** de la release adaptée et l’envoyer depuis le formulaire OTA de **l’IP de l’écran**.
4. Garder l’alimentation jusqu’au redémarrage, puis vérifier version, modèle, logger et mesures.

Le fichier OTA ne remplace pas une première installation USB. Les archives automatiques **Source code** ne sont pas un programme d’installation. [Procédure complète et récupération USB](docs/MISE_A_JOUR_ECRAN_GITHUB.md#mettre-lécran-à-jour).

## Compatibilité et anciennes notices

Le modèle de l’onduleur **et** celui du logger comptent. LSW3/LSW5 et LSE disposent de retours de fonctionnement ; **le fonctionnement DL1000B avec notre écran est confirmé par un retour utilisateur**. Pour le DL1000B-WIFI, demander un firmware **1.7.2 minimum**, activer Modbus TCP sur le lien adapté (**Link1 ou Link2**) puis utiliser le port **502**. Un fonctionnement est rapporté sur Link1 : conserver ce choix si les mesures arrivent. [Mode opératoire avec captures et demande à Deye](docs/GUIDE_DL1000B_MODBUS_TCP.md). [Vérifier son matériel](docs/MISE_A_JOUR_ECRAN_GITHUB.md#vérifier-la-compatibilité-avant-de-commencer).

Le [guide illustré](docs/GUIDE_UTILISATEUR_V3.md) explique **40 parcours avec 75 captures commentées**, pour la **4.9.2**, avec des captures refaites depuis le code actif. Les [notes 4.7.0](docs/GUIDE_UNIFIE_4.7.0.md) restent disponibles pour les anciennes versions. Les PDF ont été actualisés le **16 novembre 2026** : [guide complet illustré](docs/pdf/GUIDE_UTILISATEUR_V3.pdf) et [mémo de démarrage](docs/pdf/RECAP_UTILISATEURS_V3.pdf). Les noms de fichiers sont conservés pour les anciens liens.

Ce dépôt public distribue les **binaires OTA**, leurs empreintes SHA-256 et les guides. Les sources du firmware et leur historique restent privés. Les URL existantes des guides sont conservées pour les écrans déjà livrés.
