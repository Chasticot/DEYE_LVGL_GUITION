# Firmware unifié 4.4.0 — récupération automatique de l’IP du logger

L’écran retrouve désormais automatiquement l’adresse IP de son logger Deye après une perte durable de liaison, notamment lorsqu’un DHCP lui attribue une nouvelle IP. **Le vrai numéro de série doit être renseigné et sauvegardé auparavant** dans CONNEXION / LOGGER.

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


Les paramètres existants sont repris lors d’une mise à jour compatible sans effacement. Il n’y a pas d’option supplémentaire à activer : le numéro de série enregistré conditionne l’automatisme. Les fonctions de la 4.3.10 sont conservées. La passerelle WB01 garde son propre firmware et ses paramètres.

## Télécharger

- [Firmware OTA 4.4.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/DEYE_V3_4.4.0_OTA.bin).
- [Pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/DEYE_V3_4.4.0_Installation_Windows.zip).
- [Guide 4.4.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/GUIDE_UNIFIE_4.4.0.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.4.0/SHA256SUMS-4.4.0.txt).

Mettre à jour depuis le formulaire OTA de l’écran avec le fichier application si son partitionnement est compatible, ou utiliser le pack USB complet. Exporter les paramètres avant la mise à jour.

La découverte manuelle de la variante précédente a été confirmée sur le logger de l’utilisateur avec son numéro de série saisi. La récupération automatique, la sauvegarde et le redémarrage sont vérifiés par tests logiciels ; le scénario DHCP complet reste à valider sur matériel. [Validation 4.4.0](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.4.0/docs/VALIDATION_4.4.0.md).
