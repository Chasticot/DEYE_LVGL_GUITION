# Deye Monitor — firmware unifié 4.3.5

Suivi solaire, batterie, réseau et consommation sur écran tactile GUITION ESP32-S3 480 × 480, avec configuration depuis l'écran ou un navigateur.

**Un seul firmware : choisissez le modèle Deye et la borne de recharge dans les réglages.** La version 4.3.5 réunit la V3 multi-onduleurs et le client VE TRONIC WB01. Le catalogue conserve 12 profils sélectionnables ; la recharge propose Aucune, Deye LoRa ou VE TRONIC WB01.

## Télécharger et installer

| Installation | Fichier |
| --- | --- |
| Écran neuf ou installation USB complète Windows | [DEYE_V3_4.3.5_Installation_Windows.zip](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_Installation_Windows.zip) |
| Mise à jour d'un écran avec partitionnement OTA compatible | [DEYE_V3_4.3.5_OTA.bin](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_OTA.bin) |
| Vérification des fichiers téléchargés | [SHA256SUMS-4.3.5.txt](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/SHA256SUMS-4.3.5.txt) |

1. Sauvegardez la configuration de l'écran avant la mise à jour.
2. Pour l'installation USB, extrayez tout le ZIP, branchez un câble USB de données et lancez **INSTALLER.bat**. Un CH340 unique est sélectionné automatiquement ; sinon choisissez le port COM.
3. Après installation, vérifiez le Wi-Fi, le logger et **DEYE / SOLARMAN > MODÈLE DEYE**.
4. Dans **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, choisissez la borne, enregistrez et laissez l'écran redémarrer.
5. Pour WB01, configurez l'adresse IP de sa passerelle. Pour Deye LoRa, vérifiez les registres avant de débloquer les écritures depuis le Web.

Le pack Windows contient l'outil Espressif, les quatre binaires, les empreintes et les instructions ; aucun Arduino IDE ni Python n'est nécessaire. Le fichier OTA est l'application de **l'écran**, utilisable depuis sa page Web de mise à jour. Un changement de partitionnement nécessite le pack USB complet. [Installation détaillée](docs/INSTALLATION_WINDOWS.md).

## Choisir la recharge

| Recharge | Compatibilité et fonctionnement |
| --- | --- |
| Aucune | Tableau de bord Deye et fonctions communes, sans communication avec une borne. |
| Deye LoRa | Registres et commandes VE natifs, uniquement sur **SUN-12K-SG02LP1-EU-AM2**. |
| VE TRONIC WB01 | Client HTTP vers une passerelle ESP32 WB01 compatible, disponible avec les 12 profils Deye sélectionnables. |

Le modèle et la borne sont des choix indépendants, appliqués après sauvegarde et redémarrage. Une seule recharge est active. Les pages, communications et automatismes suivent ce choix.

**Après migration depuis la V3 ou Vetronic V3, choisissez explicitement la borne.** L'ancienne activation VE ne distinguait pas LoRa et WB01 ; elle ne réactive aucun des deux automatiquement. Les réglages communs et les espaces de réglages des modèles sont conservés. Un ancien export avec VE activé sans choix de borne explicite est refusé à l'import. [Migration et guide 4.3.5](docs/GUIDE_UNIFIE_4.3.5.md).

La passerelle WB01 garde son propre firmware et ses propres paramètres Deye. Changer le modèle sur l'écran ne reconfigure pas sa régulation solaire. La disponibilité du client avec tous les profils ne valide pas toutes ces combinaisons sur matériel.

## Fonctions

Mesures PV/batterie/réseau/consommation, historique, sources PV/GEN, registres et coefficients, tarifs Tempo ou heures creuses, relais GPIO40, veille horaire, thèmes, luminosité, configuration Web, diagnostic, export/import JSON et OTA.

WB01 propose Arrêt, Charge immédiate, Solaire et **Rendre la main à la borne**, ainsi que la protection SOC si la passerelle expose son API. La charge manuelle respecte son plafond, avec une limite locale de 32 A. La puissance précédée de `~` est estimée à 230 V à partir du courant mesuré.

Les restrictions tarifaires concernent uniquement une charge Libre démarrée et confirmée depuis l'écran tactile pour LoRa, ou une charge manuelle démarrée et confirmée depuis l'écran ou son Web pour WB01, pendant le démarrage courant. Le redémarrage ne récupère aucune ancienne pause tarifaire et n'envoie aucune consigne de charge. Désactiver ou changer la borne dans l'écran n'arrête pas une charge déjà en cours.

## Guides et validation

- [Guide du firmware unifié 4.3.5](docs/GUIDE_UNIFIE_4.3.5.md).
- [Installation Windows et OTA](docs/INSTALLATION_WINDOWS.md).
- [Compiler avec PlatformIO](docs/COMPILATION_PLATFORMIO.md) ou [Arduino IDE](docs/COMPILATION_ARDUINO.md).
- [Profils et registres](docs/PROFILS_ET_REGISTRES.md).
- [Validation logicielle et limites matérielles](docs/VALIDATION.md).
- [Notes de la release 4.3.5](docs/RELEASE_v4.3.5.md) et [historique](CHANGELOG.md).
- Guides historiques : [V3](docs/GUIDE_UTILISATEUR_V3.md), [récapitulatif V2/V3](docs/RECAP_UTILISATEURS_V3.md), [Vetronic V3](docs/GUIDE_VETRONIC_V3.md). Leurs descriptions de variantes et d'activation VE concernent ces anciennes versions.

Compilation et tests logiciels réussis pour les sources unifiées. **Aucun flash ni essai de charge réel n'a été réalisé pour cette version** ; vérifier les mesures, commandes et automatismes sur l'installation. [Détails](docs/VALIDATION.md).

## Matériel et dépôt

GUITION ESP32-S3 480 × 480 avec le brochage du projet, PSRAM OPI et logger local Solarman V5. Le relais vise l'ESP32-4848S040C à un relais, GPIO40. Flash logique 4 Mo, partition Minimal SPIFFS avec OTA. Comparez les mesures au LCD lors de la mise en route.

Le catalogue contient 13 références : 12 sélectionnables et SG06LP1 indisponible tant que sa cartographie reste incomplète. [Compatibilité par modèle](docs/PROFILS_ET_REGISTRES.md).

```text
firmware/DEYE_UNIFIED/     Sources actuelles 4.3.5 + entrée Arduino IDE
firmware/DEYE_V3/          Sources historiques V3 4.3.3
firmware/DEYE_VETRONIC_V3/  Sources historiques Vetronic V3 4.3.4
docs/                     Guides, références et validation
installation/             Installateur Windows
tools/                    Préparation Arduino, assemblage et vérification des packs
platformio.ini            Environnement deye_unified par défaut
```

Les binaires sont dans la [release v4.3.5](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.5). **Code > Download ZIP** contient les sources ; pour installer sans compiler, téléchargez le pack de la release.

Les anciennes releases restent historiques : [V3 4.3.3](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.3), [Vetronic V3 4.3.4](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.4-vetronic-v3). L'état antérieur du dépôt est repéré par le [tag d'archive](https://github.com/Chasticot/DEYE_LVGL_GUITION/tree/archive-avant-v3-2026-10-02).

## Licence

Code sous [licence MIT](LICENSE). Les bibliothèques et esptool conservent leurs licences respectives. Les PDF de protocoles constructeur restent la propriété de leurs éditeurs.
