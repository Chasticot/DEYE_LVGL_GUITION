# Firmware unifié 4.4.1 — consigne de recherche du logger

La page **Configuration Deye** indique maintenant clairement :

> Renseigner le numéro de série avant de lancer la recherche de l’adresse IP du logger.

La consigne remplace le texte qui suggérait de rechercher pour remplir l’IP et le numéro de série. Elle tient sur deux lignes au-dessus du bouton, dans les thèmes clair et sombre.

La récupération automatique après une perte de connexion reste celle de la 4.4.0 : numéro de série réel enregistré, recherche du même logger, sauvegarde d’une IP différente en conservant le numéro, puis redémarrage après vérification. Les paramètres existants sont conservés lors d’une mise à jour compatible sans effacement.

## Télécharger

- [Firmware OTA 4.4.1](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.1/DEYE_V3_4.4.1_OTA.bin).
- [Pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.1/DEYE_V3_4.4.1_Installation_Windows.zip).
- [Guide 4.4.1](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.1/GUIDE_UNIFIE_4.4.1.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.1/SHA256SUMS-4.4.1.txt).

La correction du texte a été compilée et envoyée avec succès sur l’écran de l’utilisateur en COM16 avant cette publication. Les livrables 4.4.1 sont recompilés avec le numéro de version actualisé. Le scénario DHCP automatique complet reste à valider sur matériel. [Validation 4.4.1](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.4.1/docs/VALIDATION_4.4.1.md).
