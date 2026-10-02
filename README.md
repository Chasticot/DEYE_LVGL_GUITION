# Deye Monitor — écran GUITION

Suivi solaire, batterie, réseau et consommation sur écran tactile GUITION ESP32-S3 480 × 480, avec configuration depuis l'écran ou un navigateur.

**Deux versions : V3 et VEtronic.** Dans la V3, choisissez votre modèle Deye dans un menu : le programme adapte les registres, les blocs lus et les conversions. Il n'y a pas de binaire différent pour chaque onduleur.

## Télécharger et installer

| Votre besoin | Téléchargement Windows | Firmware pour mise à jour OTA |
| --- | --- | --- |
| **V3 — 4.3.3** : choix du modèle Deye, dont AI-W5.1 ESS P1 | [Pack V3 complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.3/DEYE_V3_4.3.3_Installation_Windows.zip) | [V3 OTA](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.3/DEYE_V3_4.3.3_OTA.bin) |
| **VEtronic — 4.2-vetronic-32A** : pilotage d'une passerelle VEtronic WB01 compatible | [Pack VEtronic complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.3/DEYE_VETRONIC_4.2-vetronic-32A_Installation_Windows.zip) | [VEtronic OTA](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.3/DEYE_VETRONIC_4.2-vetronic-32A_OTA.bin) |

1. Téléchargez le **pack Windows de votre variante**, puis extrayez entièrement le ZIP.
2. Branchez l'écran avec un câble USB de données.
3. Double-cliquez sur **INSTALLER.bat**. Si un seul CH340 est détecté, son port COM est sélectionné automatiquement. Sinon, choisissez le port dans la liste.
4. Suivez les indications et attendez la réussite de l'installation.
5. Configurez le Wi-Fi et le logger. Dans la V3, sélectionnez le modèle Deye puis enregistrez.

Aucun Arduino IDE ni Python n'est nécessaire pour cette installation. Les packs contiennent `esptool.exe`, le lanceur BAT, son script PowerShell, les quatre binaires nécessaires et une notice. Le guide et le récapitulatif PDF sont aussi inclus dans le pack V3.

**Écran neuf ou changement de partitionnement : utiliser le pack USB complet.** Pour un écran déjà compatible, le fichier OTA suffit. Sauvegardez les réglages avant installation ; un effacement complet les supprime. [Installation détaillée et dépannage](docs/INSTALLATION_WINDOWS.md).

## Les guides

- [Guide utilisateur V3](docs/GUIDE_UTILISATEUR_V3.md) — [PDF](docs/pdf/GUIDE_UTILISATEUR_V3.pdf) : première mise en route, IP du logger, menus, mesures, sauvegardes et OTA.
- [Améliorations depuis les versions de base, V2 et V3](docs/RECAP_UTILISATEURS_V3.md) — [PDF](docs/pdf/RECAP_UTILISATEURS_V3.pdf).
- [Guide VEtronic et conditions d'utilisation](docs/GUIDE_VETRONIC.md).
- [Compiler avec VS Code et PlatformIO](docs/COMPILATION_PLATFORMIO.md).
- [Compiler avec Arduino IDE](docs/COMPILATION_ARDUINO.md).
- [Modèles, registres et limites connues](docs/PROFILS_ET_REGISTRES.md).
- [Historique des changements](CHANGELOG.md) et [validation de la publication](docs/VALIDATION.md).

## Nouveautés V3 4.3.3

Le profil **AI-W5.1 ESS (P1)** utilise les registres monophasés confirmés par retour utilisateur le 2 octobre 2026 : blocs R76–108 et R169–194, SOC R184, batterie R190, PV R186/R187 et consommation R178. Les anciennes adresses de ce profil sont migrées automatiquement ; les réglages des autres modèles sont conservés. Ce retour ne couvre pas toutes les révisions AI-W5.1 ni les variantes P3.

La V3 reprend les améliorations V2 : menus regroupés, choix des sources PV/GEN, réglages des coefficients, tarifs et heures creuses, relais configurable, veille horaire et sauvegardes. Elle ajoute la sélection du modèle et ses réglages propres. GEN journalier utilise R62 en LP1 et R536 en LP3/HP3.

La version VEtronic est une variante distincte. Elle conserve son numéro de version et n'intègre pas le sélecteur de modèles V3. Elle pilote une passerelle compatible via HTTP ; elle ne se connecte pas directement à la WB01.

## Matériel et compatibilité

- Écran GUITION ESP32-S3 480 × 480 avec le brochage défini dans les sources, PSRAM OPI. Le relais V3 vise l'ESP32-4848S040C à un relais, GPIO40.
- Logger compatible avec la communication locale Solarman V5, accessible sur le même réseau.
- Profil correspondant à l'onduleur ; comparez les mesures au LCD lors de la mise en route.

La liste propose 12 profils sélectionnables. SG06LP1 est affiché mais indisponible en attendant une cartographie complète. Les détails, retours terrain et fonctions à confirmer figurent dans le [tableau des profils](docs/PROFILS_ET_REGISTRES.md).

## Organisation du dépôt

```text
firmware/DEYE_V3/          Sources V3 + entrée Arduino IDE
firmware/DEYE_VETRONIC/    Sources VEtronic + entrée Arduino IDE
docs/                     Guides, PDF et références de registres
installation/             Sources de l'installateur Windows
tools/                    Préparation Arduino, assemblage et vérification des packs
platformio.ini            Deux environnements, dépendances avec versions fixées
```

Les binaires et l'exécutable Espressif sont distribués dans les **[Releases](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases)**. Le ZIP proposé par **Code > Download ZIP** contient les sources : pour installer sans compiler, utilisez les packs de la release.

L'ancien contenu reste accessible dans l'historique et sous le tag [archive-avant-v3-2026-10-02](https://github.com/Chasticot/DEYE_LVGL_GUITION/tree/archive-avant-v3-2026-10-02).

## Licence

Code du projet sous [licence MIT](LICENSE). Les bibliothèques et l'outil Espressif conservent leurs licences respectives ; la licence d'esptool est incluse dans les packs. Les PDF de protocoles constructeur restent la propriété de leurs éditeurs et servent de références techniques.
