# Installer le firmware unifié 4.3.5 sous Windows

Télécharger [DEYE_V3_4.3.5_Installation_Windows.zip](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_Installation_Windows.zip) depuis la [release v4.3.5](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.5). Ce pack installe un seul firmware pour les modèles Deye et la recharge choisie dans les réglages.

Extraire tout le ZIP ; ne pas lancer le BAT depuis l'archive. Windows 10/11 et Windows PowerShell 5.1 suffisent, sans Python ni Arduino IDE.

## Installation USB

1. Si l'écran fonctionne déjà, exporter sa configuration JSON depuis le Web et conserver une copie des réglages importants.
2. Brancher un câble USB de données. Fermer Arduino IDE, les moniteurs série et toute application utilisant le port COM.
3. Double-cliquer sur **INSTALLER.bat**.
4. L'installateur contrôle les fichiers. Un **CH340** unique est sélectionné automatiquement ; sinon il affiche les ports disponibles et demande celui de l'écran.
5. Appuyer sur Entrée pour 460800 bauds, ou choisir 115200 en cas de connexion instable.
6. Choisir l'effacement : **O** pour une remise à zéro complète ; **N** pour conserver les zones de données d'une installation compatible. L'effacement supprime tous les réglages. Un changement de partitionnement ou un écran neuf exige l'installation USB complète. Le passage de V3/Vetronic V3 à 4.3.5 ne nécessite pas à lui seul un effacement si le partitionnement est déjà compatible.
7. Contrôler le récapitulatif et saisir **INSTALLER**. Attendre la réussite sans débrancher.
8. Laisser l'écran redémarrer, ou appuyer sur RESET. Vérifier Wi-Fi, logger et modèle Deye.
9. Dans **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, choisir explicitement **Aucune**, **Deye LoRa** ou **Vetronic WB01**, enregistrer puis laisser redémarrer.
10. Pour WB01, renseigner l'IP de la passerelle depuis la page recharge. Pour LoRa, vérifier le profil SG02 AM2 et les registres avant de débloquer leurs écritures sur le Web.

Le modèle Deye et la borne sont indépendants. Deye LoRa est réservé au SUN-12K-SG02LP1-EU-AM2 ; le client WB01 est disponible avec les 12 profils sélectionnables de l'écran.

L'ancienne activation VE ne permet pas de distinguer LoRa et WB01 : après migration, aucun choix n'est deviné et la recharge reste désactivée jusqu'à une sélection explicite. Un ancien JSON avec VE activé et sans champ `ev_backend` est refusé ; utiliser ses données comme référence, sélectionner la borne puis créer un nouvel export. [Migration complète](GUIDE_UNIFIE_4.3.5.md).

## Port COM et connexion

Dans **Gestionnaire de périphériques > Ports (COM et LPT)**, débrancher/rebrancher l'écran et identifier le port apparu. En cas d'absence, essayer un autre câble de données et un autre port USB ; installer le pilote du convertisseur réellement identifié si nécessaire.

Si la connexion reste bloquée : maintenir BOOT, appuyer puis relâcher RESET, relâcher BOOT et relancer l'installateur. Le port COM peut changer. Les privilèges administrateur sont généralement inutiles pour l'installation du firmware ; un pilote USB peut en demander.

## Contenu du pack

| Fichier | Fonction |
| --- | --- |
| `INSTALLER.bat`, `installer.ps1` | Installation guidée |
| `esptool.exe` | Outil Espressif 4.5.1 |
| `bootloader.bin` | Démarrage, adresse 0x00000 |
| `partitions.bin` | Table de partitions, adresse 0x08000 |
| `boot_app0.bin` | Sélection OTA, adresse 0x0E000 |
| `firmware.bin` | Application unifiée 4.3.5, adresse 0x10000 |
| `manifest.json` | Version et empreintes des fichiers |
| `LISEZ_MOI.txt`, `LICENSE_ESPTOOL.txt` | Notice et licence |
| `GUIDE_UNIFIE_4.3.5.md` | Guide de sélection et de migration |

Les quatre binaires forment un ensemble. Ne pas les mélanger avec une ancienne release ou un autre partitionnement. La table utilise 4 Mo avec deux emplacements OTA ; `boot_app0.bin` permet de démarrer l'application écrite par USB.

Pour contrôler le pack sans écran :

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\installer.ps1 -VerificationSeule
```

Les empreintes SHA-256 détectent une corruption ou un mélange de fichiers ; elles ne constituent pas une signature numérique. La release fournit aussi [SHA256SUMS-4.3.5.txt](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/SHA256SUMS-4.3.5.txt).

## Mise à jour OTA

Depuis la page Web de mise à jour de l'écran, envoyer uniquement [DEYE_V3_4.3.5_OTA.bin](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.5/DEYE_V3_4.3.5_OTA.bin). Ne pas envoyer le ZIP, le bootloader, les partitions ou `boot_app0.bin`.

L'OTA exige un partitionnement compatible et assez de place dans l'emplacement applicatif. Pour un écran vierge ou un changement de partitionnement, utiliser l'USB. Sauvegarder la configuration avant l'opération ; après redémarrage, vérifier le modèle et sélectionner explicitement la borne lors de la migration initiale.

## Première vérification

Comparer les mesures au LCD de l'onduleur. Contrôler le choix borne après redémarrage, sa connexion et les commandes voulues. Pour WB01, vérifier la configuration Deye de sa passerelle, qui reste distincte de celle de l'écran. Le changement ou la désactivation d'une borne sur l'écran ne commande pas l'arrêt d'une charge déjà en cours.

Les anciens packs V3 4.3.3 et Vetronic V3 4.3.4 restent historiques. Compilation et tests logiciels 4.3.5 validés ; essais matériels encore nécessaires. [Validation](VALIDATION.md).
