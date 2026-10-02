# Installer depuis un PC Windows

Les [Releases](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases) proposent deux packs USB complets : **V3** et **VEtronic**. Choisir le pack voulu, puis extraire tout le ZIP. Ne pas lancer le BAT directement dans l'archive.

## Procédure

1. Si l'écran fonctionne déjà, exporter sa configuration JSON depuis son interface Web.
2. Brancher l'écran avec un câble USB de données. Fermer Arduino IDE, les moniteurs série et les logiciels qui utilisent le port.
3. Double-cliquer sur **INSTALLER.bat**.
4. Le programme contrôle les fichiers. Si un seul **CH340** est détecté, il choisit automatiquement son port COM. Sinon, il affiche les ports et demande le numéro de celui de l'écran.
5. Appuyer sur Entrée pour 460800 bauds, ou saisir 115200 si la connexion est instable.
6. Choisir l'effacement : **O** pour un écran neuf ou un changement de variante/partitionnement ; **N** pour conserver les zones de données d'une installation compatible. L'effacement supprime tous les réglages. La conservation ne garantit pas la compatibilité des anciennes données.
7. Vérifier le récapitulatif puis saisir **INSTALLER**. Attendre la fin sans débrancher.
8. Après réussite, laisser l'écran redémarrer, ou appuyer sur RESET. Configurer le Wi-Fi et le logger ; dans la V3, choisir le modèle Deye et enregistrer.

Le programme s'utilise sous Windows 10/11 avec Windows PowerShell 5.1. Aucun Python ni Arduino IDE n'est nécessaire. Les privilèges administrateur ne sont normalement pas nécessaires ; l'installation d'un pilote USB peut en demander.

## Trouver le port COM

Ouvrir **Gestionnaire de périphériques > Ports (COM et LPT)**, débrancher/rebrancher l'écran et repérer le port qui apparaît. Si aucun port n'apparaît, essayer un autre câble de données et un autre port USB. Installer le pilote du convertisseur USB réellement identifié par Windows si celui-ci manque.

En cas de blocage à la connexion : maintenir BOOT, appuyer puis relâcher RESET, relâcher BOOT et relancer. Le numéro COM peut changer. Choisir le nouveau port.

## Contenu du pack

| Fichier | Usage |
| --- | --- |
| `INSTALLER.bat`, `installer.ps1` | Lanceur et installation guidée |
| `esptool.exe` | Outil Espressif 4.5.1 |
| `bootloader.bin` | Démarrage, adresse 0x00000 |
| `partitions.bin` | Table de partitions, adresse 0x08000 |
| `boot_app0.bin` | Réinitialisation de la sélection OTA, adresse 0x0E000 |
| `firmware.bin` | Application de la variante, adresse 0x10000 |
| `manifest.json` | Version et empreintes SHA-256 |
| `LISEZ_MOI.txt`, `LICENSE_ESPTOOL.txt` | Notice et licence de l'outil |

Le pack V3 contient aussi le guide utilisateur et le récapitulatif PDF. Les quatre binaires forment un ensemble : ne pas mélanger des versions ou des variantes. Le partitionnement utilise 4 Mo avec deux emplacements OTA. La remise à zéro de la sélection OTA permet de démarrer sur l'application écrite par USB, même si l'écran utilisait auparavant l'autre emplacement.

Les empreintes détectent les fichiers corrompus ou mélangés ; elles ne constituent pas une signature numérique. Pour vérifier sans écran : `powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\installer.ps1 -VerificationSeule`.

## Mise à jour OTA

Utiliser seulement le fichier **OTA de votre variante**, depuis la page de mise à jour Web de l'écran. Ne pas envoyer les autres binaires. Une mise à jour OTA suppose un logiciel et un partitionnement compatibles, avec suffisamment de place. En cas de changement de partitionnement ou sur un écran vierge, utiliser le pack USB complet.

Pour AI-W5.1 P1, la V3 4.3.3 remplace automatiquement les anciennes adresses sauvegardées. Ne pas réimporter ensuite un ancien JSON AI-W5.1 contenant les adresses 600 ; faire un nouvel export après mise à jour.
