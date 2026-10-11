# Deye Monitor — firmware unifié 4.4.0

## Retrouver automatiquement l’adresse IP du logger

Renseigner **le vrai numéro de série du logger** dans **DEYE / SOLARMAN > CONNEXION / LOGGER**, puis **SAUVEGARDER**. Le numéro doit être enregistré dans l’écran ; une saisie laissée dans un formulaire ne suffit pas. Avec ce numéro, le bouton **RECHERCHER LOGGER** permet aussi de trouver l’IP à la première configuration. La recherche manuelle propose les champs avant sauvegarde.

La récupération automatique est active dès qu’un numéro de série réel est enregistré. Elle utilise ce numéro pour identifier le même logger et conserve sa valeur, même si le DHCP change son adresse IP.

1. L’écran attend **au moins trois lectures échouées et 60 secondes sans réponse Solarman V5 valide**, avec son Wi-Fi connecté. Une réponse du logger signalant une erreur Modbus confirme encore sa présence et ne déclenche pas cette recherche.
2. Il recherche ce logger sur le réseau local : broadcast UDP, puis TCP/V5 avec son numéro de série, puis unicast UDP si nécessaire. Les lectures Deye sont suspendues pendant le balayage ; les tâches de l’interface et de la passerelle WB01 restent actives.
3. Si **une IP différente** est trouvée pour **le même numéro de série**, seule l’adresse IP est remplacée dans les paramètres enregistrés. Le numéro de série n’est pas réécrit. Après vérification de la sauvegarde, l’écran redémarre automatiquement et utilise la nouvelle IP.
4. Si l’IP reste identique, si aucun logger correspondant n’est trouvé ou si la sauvegarde échoue, aucun redémarrage n’est déclenché. Les lectures reprennent et une nouvelle recherche peut partir **cinq minutes après la fin de la précédente**, si la liaison reste perdue.

Une coupure Wi-Fi suspend l’automatisme : après reconnexion, plusieurs échecs et le délai de perte sont de nouveau nécessaires. Une mise à jour OTA ou un redémarrage déjà programmé reporte l’application d’un résultat. Un changement manuel d’IP ou de numéro pendant la recherche invalide le résultat automatique.

Le numéro vide, nul ou factice `123456789` désactive la récupération automatique. L’écran ne peut pas deviner un numéro de série inconnu par TCP. La recherche UDP manuelle peut le retrouver si le logger répond.

Le balayage suit le masque réseau pour les /24 ou plus petits ; sur un réseau plus grand, il reste limité au /24 de l’écran. L’écran et le logger doivent être joignables sur ce réseau, sans isolation entre clients. Un balayage peut prendre plusieurs minutes. La page de connexion affiche la phase UDP/TCP et le redémarrage lorsqu’une nouvelle adresse a été sauvegardée. Les traces série détaillent les sondes et les réponses rejetées.

Exemple : le logger enregistré avec son numéro de série répondait sur `192.168.1.12`, puis le DHCP lui attribue `192.168.1.45`. Après perte durable de liaison, l’écran retrouve `192.168.1.45` pour ce même numéro, enregistre uniquement cette IP et redémarre. Les autres réglages restent conservés.

Suivi solaire, batterie, réseau et consommation sur écran tactile GUITION ESP32-S3 480 × 480, avec configuration depuis l'écran ou un navigateur.

**Un seul firmware : choisissez le modèle Deye et la borne de recharge dans les réglages.** La version 4.4.0 réunit la V3 multi-onduleurs et le client VE TRONIC WB01. Le catalogue conserve 12 profils sélectionnables ; la recharge propose Aucune, Deye LoRa ou VE TRONIC WB01.

**GEN MO + PV** cumule la puissance instantanée et les kWh du jour. GEN devient une source PV supplémentaire, avec correction du cas où son signe après calibration empêchait le cumul des watts en 4.3.5.

La correction de la vérification introduite en 4.3.7 est conservée pour les anciennes versions suffixées (`4.3.4-vetronic-v3`). Si leur bouton affiche « b is null », utiliser directement le fichier OTA et le formulaire de mise à jour.

Les indicateurs rétablis en 4.3.8 sont conservés près de la voiture : soleil jaune en mode solaire confirmé, coche verte quand la WB01 signale le véhicule branché (en attente ou en charge), croix rouge quand il est débranché. Une liaison ou une mesure indisponible affiche un tiret gris pour le branchement et masque le soleil si le mode ne peut plus être confirmé.

L’affichage optionnel des kWh du registre GEN Daily, ajouté en 4.3.10, reste disponible à côté de SMARTLOAD ou de la puissance GEN. Les températures restent sur la même ligne. L’état ON/OFF du relais est masqué quand sa règle est désactivée.

## Télécharger et installer

| Installation | Fichier |
| --- | --- |
| Écran neuf ou installation USB complète Windows | [DEYE_V3_4.4.0_Installation_Windows.zip](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/DEYE_V3_4.4.0_Installation_Windows.zip) |
| Mise à jour d'un écran avec partitionnement OTA compatible | [DEYE_V3_4.4.0_OTA.bin](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/DEYE_V3_4.4.0_OTA.bin) |
| Vérification des fichiers téléchargés | [SHA256SUMS-4.4.0.txt](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/SHA256SUMS-4.4.0.txt) |

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

**Après migration depuis la V3 ou Vetronic V3, choisissez explicitement la borne.** L'ancienne activation VE ne distinguait pas LoRa et WB01 ; elle ne réactive aucun des deux automatiquement. Les réglages communs et les espaces de réglages des modèles sont conservés. Un ancien export avec VE activé sans choix de borne explicite est refusé à l'import. [Migration et guide 4.4.0](docs/GUIDE_UNIFIE_4.4.0.md).

La passerelle WB01 garde son propre firmware et ses propres paramètres Deye. Changer le modèle sur l'écran ne reconfigure pas sa régulation solaire. La disponibilité du client avec tous les profils ne valide pas toutes ces combinaisons sur matériel.

## Fonctions

Mesures PV/batterie/réseau/consommation, historique, sources PV/GEN, registres et coefficients, tarifs Tempo ou heures creuses, relais GPIO40, veille horaire, thèmes, luminosité, configuration Web, diagnostic, export/import JSON et OTA.

WB01 propose Arrêt, Charge immédiate, Solaire et **Rendre la main à la borne**, ainsi que la protection SOC si la passerelle expose son API. La charge manuelle respecte son plafond, avec une limite locale de 32 A. La puissance précédée de `~` est estimée à 230 V à partir du courant mesuré.

Les restrictions tarifaires concernent uniquement une charge Libre démarrée et confirmée depuis l'écran tactile pour LoRa, ou une charge manuelle démarrée et confirmée depuis l'écran ou son Web pour WB01, pendant le démarrage courant. Le redémarrage ne récupère aucune ancienne pause tarifaire et n'envoie aucune consigne de charge. Désactiver ou changer la borne dans l'écran n'arrête pas une charge déjà en cours.

## Guides et validation

- [Guide du firmware unifié 4.4.0](docs/GUIDE_UNIFIE_4.4.0.md).
- [Installation Windows et OTA](docs/INSTALLATION_WINDOWS.md).
- [Compiler avec PlatformIO](docs/COMPILATION_PLATFORMIO.md) ou [Arduino IDE](docs/COMPILATION_ARDUINO.md).
- [Profils et registres](docs/PROFILS_ET_REGISTRES.md).
- [Validation logicielle et limites matérielles](docs/VALIDATION.md).
- [Notes de la release 4.4.0](docs/RELEASE_v4.4.0.md) et [historique](CHANGELOG.md).
- Guides historiques : [V3](docs/GUIDE_UTILISATEUR_V3.md), [récapitulatif V2/V3](docs/RECAP_UTILISATEURS_V3.md), [Vetronic V3](docs/GUIDE_VETRONIC_V3.md). Leurs descriptions de variantes et d'activation VE concernent ces anciennes versions.

Compilation et tests logiciels réussis pour les sources unifiées. **Aucun flash ni essai de charge réel n'a été réalisé pour cette version** ; vérifier les mesures, commandes et automatismes sur l'installation. [Détails](docs/VALIDATION.md).

## Matériel et dépôt

GUITION ESP32-S3 480 × 480 avec le brochage du projet, PSRAM OPI et logger local Solarman V5. Le relais vise l'ESP32-4848S040C à un relais, GPIO40. Flash logique 4 Mo, partition Minimal SPIFFS avec OTA. Comparez les mesures au LCD lors de la mise en route.

Le catalogue contient 13 références : 12 sélectionnables et SG06LP1 indisponible tant que sa cartographie reste incomplète. [Compatibilité par modèle](docs/PROFILS_ET_REGISTRES.md).

```text
firmware/DEYE_UNIFIED/     Sources actuelles 4.4.0 + entrée Arduino IDE
firmware/DEYE_V3/          Sources historiques V3 4.3.3
firmware/DEYE_VETRONIC_V3/  Sources historiques Vetronic V3 4.3.4
docs/                     Guides, références et validation
installation/             Installateur Windows
tools/                    Préparation Arduino, assemblage et vérification des packs
platformio.ini            Environnement deye_unified par défaut
```

Les binaires sont dans la [release v4.4.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.4.0). **Code > Download ZIP** contient les sources ; pour installer sans compiler, téléchargez le pack de la release.

Les anciennes releases restent historiques : [unifiée 4.3.5](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.5), [V3 4.3.3](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.3), [Vetronic V3 4.3.4](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.4-vetronic-v3). L'état antérieur du dépôt est repéré par le [tag d'archive](https://github.com/Chasticot/DEYE_LVGL_GUITION/tree/archive-avant-v3-2026-10-02).

## Licence

Code sous [licence MIT](LICENSE). Les bibliothèques et esptool conservent leurs licences respectives. Les PDF de protocoles constructeur restent la propriété de leurs éditeurs.
