# Deye Monitor 4.8.0 — prévision solaire

Cette mise à jour ajoute les prévisions de production d’aujourd’hui et de demain, avec leurs deux courbes et un indicateur tactile dans le bandeau.

- **Configuration > Prévision solaire** : position, puissance des panneaux en kWc, inclinaison et orientation.
- **PV auj. / PV dem.** : toucher pour ouvrir les deux courbes.
- **Aujourd’hui / Demain / Alternance** : choisir la valeur du bandeau. Alternance toutes les **5 secondes par défaut**, réglable de **1 à 300 secondes**.
- Sans compte ni clé API. Internet et une date correcte sont nécessaires pour Forecast.Solar.
- Choix du bandeau et durée conservés après redémarrage et OTA ; export/import JSON compatible.

La fonction est désactivée par défaut. Les kWh correspondent à la journée entière ; ils ne représentent pas seulement ce qu’il reste à produire. Un AP de logger sans Internet reste utilisable pour les mesures locales, mais ne permet pas de recevoir la prévision.

**Installation :** sauvegarder la configuration JSON, puis utiliser le menu de mise à jour de l’écran ou envoyer `DEYE_V3_4.8.0_OTA.bin` sur sa page Web OTA. Garder l’alimentation jusqu’au redémarrage. Le fichier n’est pas une image USB complète.

Compilation, tests logiciels et aperçus visuels vérifiés ; précision et fonctionnement sur l’installation réelle restent à contrôler.

[Guide illustré de la prévision solaire](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/GUIDE_PREVISION_SOLAIRE.md) · [Nouveautés et installation 4.8.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/GUIDE_UNIFIE_4.8.0.md) · [Guide utilisateur](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/MISE_A_JOUR_ECRAN_GITHUB.md).

La release contient uniquement le binaire OTA et `SHA256SUMS-4.8.0.txt`. Les sources du firmware restent privées.
