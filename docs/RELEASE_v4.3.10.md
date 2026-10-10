# Firmware unifié 4.3.10 — kWh GEN Daily et affichage du relais

Dans **DEYE / SOLARMAN > PRODUCTION PV / GEN**, la nouvelle option **Afficher les kWh GEN Daily** ajoute le compteur journalier à côté de SMARTLOAD ou de la puissance GEN : `SMARTLOAD : ON / 278 kWh`, `SMARTLOAD : OFF / 278 kWh` ou `GEN : 2650 W / 278 kWh`.

L’option est désactivée par défaut et indépendante du cumul GEN MO + PV. Elle utilise le registre GEN Daily et son coefficient configurés. La lecture fonctionne dans les deux modes, même sans synchronisation NTP ; une valeur indisponible affiche `-- kWh`.

Les températures restent sur la même ligne. Le texte SMARTLOAD/GEN utilise une police légèrement plus petite quand les kWh sont affichés ; le rendu est vérifié jusqu’à 999,9 kWh dans les thèmes clair et sombre.

L’indication ON/OFF du relais est masquée sur le tableau de bord tactile et Web quand **Activer la règle** est désactivé. Avec la règle active, l’état reste visible.

Les réglages existants sont conservés à la mise à jour ; la nouvelle option démarre désactivée. Les anciens exports JSON restent importables. Les assertions des tests hôte et LVGL sont désormais activées explicitement avec Zig.

## Télécharger

- [Firmware OTA 4.3.10](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.10/DEYE_V3_4.3.10_OTA.bin).
- [Pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.10/DEYE_V3_4.3.10_Installation_Windows.zip).
- [Guide 4.3.10](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.10/GUIDE_UNIFIE_4.3.10.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.10/SHA256SUMS-4.3.10.txt).

Mettre à jour depuis le formulaire OTA de l’écran avec le fichier application. Une mise à jour compatible sans effacement conserve les paramètres des versions unifiées précédentes. La passerelle WB01 conserve son propre firmware.

Compilation, tests logiciels, rendu LVGL et pack vérifiés : [validation 4.3.10](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.3.10/docs/VALIDATION_4.3.10.md). La validation sur écran physique reste à effectuer ; aucun appareil n’a été flashé pour cette publication.
