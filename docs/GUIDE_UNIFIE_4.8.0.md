# Deye Monitor 4.8.0 — prévision solaire

**Publication du 26 octobre 2026.** [Guide utilisateur illustré](MISE_A_JOUR_ECRAN_GITHUB.md) · [Configurer la prévision solaire](GUIDE_PREVISION_SOLAIRE.md).

## Ce qui change

La 4.8.0 ajoute une prévision solaire facultative, avec les courbes d’aujourd’hui et de demain. Le service public Forecast.Solar fonctionne sans compte ni clé API ; Internet et une date correcte sont nécessaires.

- Ouvrir **Configuration > Prévision solaire**, renseigner la position et les panneaux, puis activer et sauvegarder.
- Toucher **PV auj. / PV dem.**, entre l’heure et les paramètres, pour ouvrir les deux courbes.
- Choisir **Aujourd’hui**, **Demain** ou **Alternance** pour le bandeau.
- L’alternance utilise **5 secondes par défaut**, réglables de **1 à 300 secondes** dans **Réglages > Bandeau principal > Alternance (s)**. Le choix est conservé après redémarrage et OTA.

![Les deux courbes et les trois choix du bandeau.](images/solaire/courbes-alternance.png)

Les valeurs de l’image sont illustratives. Les courbes indiquent la puissance prévue en **kW** ; les totaux en **kWh** couvrent la journée entière, pas uniquement les heures restantes. Les deux courbes restent visibles quel que soit le choix du bandeau.

## Réglages et connexion

La fonction reste masquée tant qu’elle n’est pas activée. Les cinq paramètres décrivent un groupe de panneaux de même orientation et inclinaison. L’activation transmet leur position et leurs caractéristiques à Forecast.Solar.

La prévision est reçue au plus une fois par heure. L’alternance du bandeau ne lance pas de nouvelle demande Internet. Les données trop anciennes sont retirées ; une courbe absente n’est pas remplacée par une production nulle.

Un AP de logger sans accès Internet permet toujours les mesures locales, mais ne suffit pas à recevoir la prévision. Les réglages du DEYE, du réseau et de la recharge restent conservés lors d’une OTA compatible. Les options solaires font aussi partie de l’export/import JSON.

## Installer sur un écran déjà équipé

1. Exporter la configuration JSON depuis la page Web de l’écran.
2. Ouvrir **Configuration > Mise à jour > Vérifier sur GitHub > Installer**, avec Internet et une heure correcte.
3. Ou envoyer **DEYE_V3_4.8.0_OTA.bin** dans le formulaire OTA de la page Web de **l’écran**.
4. Conserver l’alimentation jusqu’au redémarrage ; vérifier la version, le modèle, la liaison au logger et les mesures.
5. Activer la prévision seulement si souhaité, puis contrôler les paramètres des panneaux.

- [Binaire OTA 4.8.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.8.0/DEYE_V3_4.8.0_OTA.bin).
- [Empreinte SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.8.0/SHA256SUMS-4.8.0.txt).
- [Procédure complète et récupération USB](MISE_A_JOUR_ECRAN_GITHUB.md#mettre-lécran-à-jour).

Ce fichier est l’application OTA de l’écran, pas une première installation USB ni un firmware pour le logger ou une passerelle WB01. Les archives automatiques **Source code** de ce dépôt contiennent uniquement la documentation publique.

## Vérifications

Compilation ESP32-S3, tests de régression et contrôles visuels en thèmes clair et sombre réussis. L’appel réel au service et sa connexion HTTPS ont été vérifiés. La précision de la prévision, le tactile et le cycle OTA restent à confirmer sur l’écran et l’installation réels. Les ombres locales et la météo peuvent créer un écart avec la production mesurée.

La 4.8.0 conserve la base 4.7.2, notamment ses améliorations de reconnexion Wi-Fi. Les profils expérimentaux et les matériels signalés à valider gardent leurs limites : [FAQ et compatibilité](FAQ_ECRAN.md).
