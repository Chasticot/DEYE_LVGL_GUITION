Cette publication réunit **deux variantes d'écran** : **V3 4.3.3** et **VEtronic 4.2-vetronic-32A**.

## Quel fichier télécharger ?

- **Première installation depuis Windows** : choisir `DEYE_V3_4.3.3_Installation_Windows.zip` ou `DEYE_VETRONIC_4.2-vetronic-32A_Installation_Windows.zip`, extraire tout le ZIP puis lancer `INSTALLER.bat`.
- **Mise à jour d'un écran compatible par son interface Web** : choisir le fichier `_OTA.bin` de la bonne variante.
- **Documentation** : le guide V3 et le récapitulatif PDF sont disponibles séparément et inclus dans le pack V3.
- **Sources** : les archives « Source code » de GitHub contiennent les deux variantes, les outils et la documentation de compilation.

Les packs contiennent l'outil Espressif, le BAT, son script PowerShell et les quatre binaires. Le port COM est choisi automatiquement si un seul CH340 est détecté ; sinon il faut le sélectionner. L'effacement complet des réglages est facultatif et explicitement demandé. Aucun Arduino IDE ni Python n'est nécessaire pour installer ces packs.

## V3 4.3.3 et AI-W5.1 ESS P1

Le profil AI-W5.1 ESS (P1) utilise désormais la cartographie monophasée confirmée par retour utilisateur le 2 octobre 2026 : blocs R76–108 et R169–194, PV186/187, batterie190, SOC184, consommation178 et état réseau194. Les anciennes adresses sauvegardées de ce seul profil sont migrées automatiquement. Les autres profils conservent leurs réglages.

Après installation : **Configuration > Deye / Solarman > Modèle Deye > AI-W5.1 ESS (P1)**, puis enregistrer. Ne pas réimporter ensuite un ancien JSON AI-W5.1 avec les adresses 600 ; faire un nouvel export.

La V3 regroupe la sélection de modèles, les améliorations V2, les sources PV/GEN, tarifs, relais et veille. GEN journalier utilise R62 en LP1 et R536 en LP3/HP3. Le [récapitulatif](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.3.3/docs/RECAP_UTILISATEURS_V3.md) détaille les changements depuis les versions de base.

## VEtronic

La variante VEtronic reste distincte, avec son propre binaire et son numéro de version. Elle pilote une passerelle compatible et ne contient pas le sélecteur de modèles V3. Le firmware de cette passerelle est un projet séparé, non inclus ici. Voir le [guide VEtronic](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.3.3/docs/GUIDE_VETRONIC.md).

## Installation et validation

Les nouveaux packs utilisent un partitionnement 4 Mo avec OTA. Sur un écran neuf ou lors d'un changement de partitionnement, utiliser l'USB complet. Sauvegarder les réglages avant l'installation ; l'effacement les supprime.

Compilations PlatformIO et Arduino vérifiées pour les deux variantes. Tests de profils, migration AI-W5.1, isolation des modèles, JSON, logique et rendu LVGL réussis. Les packs sont contrôlés après extraction, y compris les fichiers absents/corrompus et les cas de sélection du port COM. Les empreintes des fichiers joints figurent dans `SHA256SUMS.txt`.

Le retour AI-W5.1 concerne une installation P1 ; il ne valide pas toutes les révisions ni les P3. Aucun flash physique ni commande de charge n'a été effectué lors de la préparation de cette publication.

L'historique du dépôt et les anciennes releases sont conservés. L'état précédent la refonte est repéré par le tag `archive-avant-v3-2026-10-02`.
