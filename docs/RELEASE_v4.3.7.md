# Firmware unifié 4.3.7 — correction de la vérification GitHub

Le bouton **Vérifier si une version plus récente existe** accepte les anciennes versions suffixées, notamment `4.3.4-vetronic-v3`. Le code précédent ne reconnaissait pas ce suffixe et tentait de lire une valeur vide, provoquant « b is null ».

La comparaison utilise les trois nombres de version, avec suffixe de variante et métadonnées facultatifs. Les versions inconnues et les réponses GitHub invalides donnent un message explicite. Le bouton consulte uniquement la release ; il n'installe aucun firmware automatiquement.

## Mise à jour depuis 4.3.4 Vetronic V3

Le bouton du firmware déjà installé reste celui de l'ancienne version. Télécharger manuellement [DEYE_V3_4.3.7_OTA.bin](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/DEYE_V3_4.3.7_OTA.bin), puis le sélectionner dans le formulaire **Mise à jour OTA** et lancer l'installation. Le formulaire est indépendant du bouton de vérification.

Sauvegarder d'abord la configuration. Après migration initiale depuis Vetronic V3, vérifier le modèle Deye et sélectionner explicitement la borne, enregistrer et laisser redémarrer. Depuis 4.3.5/4.3.6, les sélections sont conservées avec une mise à jour compatible sans effacement.

## Téléchargements

- [Firmware OTA 4.3.7](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/DEYE_V3_4.3.7_OTA.bin).
- [Pack Windows](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/DEYE_V3_4.3.7_Installation_Windows.zip).
- [Guide 4.3.7](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/GUIDE_UNIFIE_4.3.7.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/SHA256SUMS-4.3.7.txt).

Les fonctions de la 4.3.6 sont conservées, dont le cumul GEN MO + PV en puissance instantanée et kWh du jour. Le firmware concerne l'écran ; la passerelle WB01 conserve son firmware et son calcul solaire.

Compilation PlatformIO, tests du JavaScript de production et contrôles du pack réussis. Le test reproduisait l'erreur avant correction et couvre les suffixes historiques, la comparaison numérique, les versions invalides et les erreurs GitHub/réseau. Aucun flash ni commande réelle de charge effectué.
