# Firmware unifié 4.3.5 — Deye multi-onduleurs + WB01

Un seul firmware pour l'écran GUITION ESP32-S3 480 × 480 : le modèle Deye et la borne se choisissent dans les réglages de l'écran ou du Web.

- 12 profils Deye sélectionnables ; SG06LP1 reste indisponible faute de cartographie complète.
- Recharge **Aucune**, **Deye LoRa** ou **VE TRONIC WB01**.
- Deye LoRa réservé au **SUN-12K-SG02LP1-EU-AM2** ; client WB01 accessible avec tous les profils sélectionnables de l'écran.
- Menus, communications et tarifs adaptés à la borne sélectionnée.
- WB01 : Arrêt, Charge immédiate, Solaire, Rendre la main à la borne, courant jusqu'à 32 A selon le plafond de la passerelle et protection SOC compatible.
- Fonctions V3 conservées : mesures, historique, PV/GEN, relais, veille, tarifs, configuration Web, exports JSON et OTA.

## Télécharger

- [Pack USB complet Windows — DEYE_V3_4.3.5_Installation_Windows.zip](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_Installation_Windows.zip) : extraire tout le ZIP puis lancer INSTALLER.bat ; aucun Arduino IDE ni Python nécessaire.
- [Application OTA — DEYE_V3_4.3.5_OTA.bin](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_OTA.bin) : envoyer uniquement ce fichier depuis le formulaire OTA d'un écran au partitionnement compatible.
- [Guide unifié — GUIDE_UNIFIE_4.3.5.md](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/GUIDE_UNIFIE_4.3.5.md).
- [Empreintes — SHA256SUMS-4.3.5.txt](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/SHA256SUMS-4.3.5.txt).

Les binaires sont destinés à l'écran du projet, avec PSRAM OPI et partition Minimal SPIFFS 4 Mo avec OTA. Ils ne mettent pas à jour la passerelle WB01.

## Migration

Sauvegarder la configuration avant installation. Vérifier le modèle Deye après démarrage, puis choisir explicitement la borne dans **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, enregistrer et laisser redémarrer.

L'ancienne activation VE désignait deux systèmes différents selon le firmware. Elle ne devient pas automatiquement un choix LoRa ou WB01. Sans sélection explicite, la recharge reste désactivée. Les réglages communs et espaces par modèle sont conservés ; les écritures VE natives restent verrouillées lors de leur première migration.

Les nouveaux exports JSON contiennent `ev_backend`. Les anciens exports avec VE activé sans ce choix explicite sont refusés. Les autres sauvegardes de réglages préservent la borne enregistrée.

Le redémarrage n'envoie aucune consigne de charge. Désactiver ou changer la borne dans l'écran n'arrête pas une charge déjà en cours. Les restrictions tarifaires concernent les charges lancées et confirmées localement pendant le démarrage courant ; aucune ancienne pause n'est reprise après redémarrage.

## Validation et limites

Compilation PlatformIO et tests logiciels réussis : profils, sauvegardes, migrations, compatibilité des bornes, gardes des écritures natives, tarifs, codecs JSON/WB01, scripts Web réels simulés et aperçu LVGL des deux interfaces.

**Aucun flash ni essai de charge réel n'a été effectué pour cette version.** Les mesures, commandes et automatismes restent à vérifier sur le matériel. La puissance WB01 précédée de `~` est une estimation à 230 V.

La passerelle WB01 conserve ses propres registres Deye, coefficients et régulation solaire. Changer le modèle sur l'écran ne la reconfigure pas. La disponibilité du client avec tous les profils ne valide pas toutes ces combinaisons réelles.

[Guide](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/GUIDE_UNIFIE_4.3.5.md) · [Installation](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/INSTALLATION_WINDOWS.md) · [Validation](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/VALIDATION.md)

Les sources historiques V3 4.3.3 et Vetronic V3 4.3.4 restent conservées dans le dépôt. Le projet courant est **firmware/DEYE_UNIFIED/**, environnement **deye_unified**.
