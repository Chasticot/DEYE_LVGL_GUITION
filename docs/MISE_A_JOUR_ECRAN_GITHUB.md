# Guide utilisateur — Deye Monitor

**Firmware unifié 4.7.0 - Édition du 21 octobre 2026.** [Manuel des fonctions communes (édition 4.6.3)](GUIDE_UTILISATEUR_V3.md) · [Récapitulatif](RECAP_UTILISATEURS_V3.md) · [PDF des fonctions communes (édition 4.6.3)](pdf/GUIDE_UTILISATEUR_V3.pdf).

Ce guide explique comment mettre en route et utiliser l’écran Deye Monitor au quotidien. Il concerne l’écran tactile, sa page accessible depuis un téléphone ou un ordinateur et les fonctions optionnelles de recharge.

**Si votre écran affiche déjà les bonnes mesures, vous pouvez l’utiliser directement. Il n’est pas nécessaire de le réinstaller.**

## Trouver rapidement une réponse

- [Première utilisation : connecter l’écran](#première-utilisation--connecter-lécran).
- [Faire apparaître les mesures de l’onduleur](#faire-apparaître-les-mesures-de-londuleur).
- [Comprendre le tableau de bord](#comprendre-le-tableau-de-bord).
- [Se repérer dans les menus](#se-repérer-dans-les-menus).
- [Configurer une borne de recharge](#configurer-une-borne-de-recharge).
- [Régler l’affichage et l’heure](#régler-laffichage-et-lheure).
- [Ouvrir l’écran sur un téléphone et sauvegarder ses réglages](#ouvrir-lécran-sur-un-téléphone-ou-un-ordinateur).
- [Mettre l’écran à jour](#mettre-lécran-à-jour).
- [Résoudre les problèmes courants](#résoudre-les-problèmes-courants).

## Première utilisation : connecter l’écran

1. Allumer l’écran et ouvrir les paramètres avec le bouton de configuration.
2. Choisir **RÉSEAU / HEURE**, puis **WIFI / RÉSEAU**.
3. Toucher **SCAN**, choisir le réseau Wi-Fi de la maison et saisir son mot de passe.
4. Sauvegarder, puis laisser l’écran redémarrer si demandé.
5. Revenir dans la page Wi-Fi pour vérifier qu’une adresse IP est affichée. Cette adresse permet aussi d’ouvrir l’écran dans le navigateur d’un téléphone ou d’un ordinateur.

L’écran et le module de communication de l’onduleur doivent pouvoir communiquer sur le réseau local. Un Wi-Fi invité qui isole ses appareils peut empêcher cette liaison.

L’accès à Internet sert notamment à consulter les mises à jour, à synchroniser l’heure et aux informations Tempo. Les mesures de l’onduleur passent par le réseau local.

## Reconnexion automatique du Wi-Fi

Depuis la 4.6.3, après une perte du Wi-Fi, l'écran laisse cinq secondes à la reconnexion du pilote, puis relance une connexion avec les identifiants enregistrés. Si le routeur reste indisponible, il recommence toutes les 30 secondes, sans limite de durée. Au démarrage et après un scan manuel, la première tentative dispose de 30 secondes.

Les paramètres DHCP ou IP fixe sont conservés et l'interface reste utilisable pendant l'attente. Un scan manuel suspend les relances, puis la connexion reprend à sa fin. Les lectures réseau reprennent par leurs tâches habituelles dès que le Wi-Fi revient. La récupération automatique d'une nouvelle adresse du logger reste limitée au mode LSW/V5.

## Faire apparaître les mesures de l’onduleur

Le **logger** est le module de communication branché sur l’onduleur. Son numéro de série est celui à renseigner dans les réglages de connexion ; il peut être différent du numéro de série de l’onduleur lui-même.

1. Dans **DEYE / SOLARMAN > MON ONDULEUR**, choisir la puissance et la référence exacte, puis **APPLIQUER / REDÉMARRER** et confirmer.
2. Ouvrir **DEYE / SOLARMAN > CONNEXION / LOGGER**.
3. Choisir le type de module : **LSW**, **LSE** ou **DL1000B**, selon l’équipement installé.
4. Renseigner le vrai numéro du logger. Les lettres sont acceptées pour le DL1000B.
5. Toucher **RECHERCHER**. La recherche peut prendre plusieurs minutes ; les mesures peuvent être momentanément suspendues.
6. Vérifier l’adresse trouvée, puis sauvegarder.

Si le résultat indique **numéro non vérifié**, ouvrir **RESULTATS** et choisir explicitement le bon module avant de sauvegarder. Une réponse sur le réseau ne suffit pas toujours à identifier son numéro.

Si vous connaissez déjà l’adresse IP du logger, vous pouvez la saisir directement. Les ports proposés sont **8899 pour LSW**, **8899 pour LSE** et **502 pour DL1000B** ; conserver un port différent si votre équipement a été configuré ainsi. L’unité doit correspondre à l’adresse de communication configurée sur l’onduleur. Si ces informations ne sont pas connues, les demander à l’installateur.

Après redémarrage, attendre l’arrivée des mesures et les comparer à celles de l’afficheur de l’onduleur. Une erreur de modèle ou de connexion doit être corrigée avant d’interpréter les valeurs.

Avec un logger **LSW / Solarman V5** dont le vrai numéro est enregistré, l’écran peut retrouver automatiquement son adresse IP après une perte durable de connexion. Pour une connexion TCP, vérifier l’adresse et les paramètres dans ce menu si la liaison est perdue.

## Choisir le modèle et les sources de production

Choisir **DEYE / SOLARMAN > MON ONDULEUR**, puis la **puissance** et la **référence exacte** de l’étiquette, suffixes compris. Vérifier le récapitulatif, toucher **APPLIQUER / REDÉMARRER** et confirmer. Le Web propose aussi une recherche par fragments.

Le catalogue comporte **112 entrées** : 106 profils documentés ou historiques, 5 candidats SG06 BM3 expérimentaux et une ancienne entrée SG06 indisponible. Les voies PV, le nombre de MPPT et les fonctions proposées suivent le modèle choisi. Les réglages existants par référence sont conservés. Les SG06 expérimentaux et les fonctions non vérifiées restent signalés à confirmer. Comparer les mesures au LCD de l’onduleur.

Dans **DEYE / SOLARMAN > PRODUCTION SOLAIRE / GEN**, choisir **Automatique**, **Port non utilisé**, **Appareils pilotés (SmartLoad)**, **Production solaire supplémentaire** ou **Groupe électrogène**. Le mode automatique lit le réglage de l’onduleur ; une réponse absente ou périmée reste « à préciser ». Ces choix adaptent l’écran sans changer le mode de l’onduleur.

La production solaire supplémentaire peut être cumulée avec PV si son énergie n’est pas déjà incluse dans PV Daily. Le groupe électrogène n’est jamais ajouté au solaire. Les fonctions et voies absentes sont masquées. Le choix de borne reste explicite.

[Guide détaillé 4.7.0](GUIDE_UNIFIE_4.7.0.md).

## Comprendre le tableau de bord

| Indication | Ce qu’elle signifie |
| --- | --- |
| **PV / solaire** | Production des panneaux solaires. |
| **Batterie / SOC** | Niveau de charge de la batterie, exprimé en pourcentage. Les autres valeurs indiquent son fonctionnement. |
| **Réseau** | Échanges d’électricité avec le réseau public. |
| **Maison / consommation** | Électricité utilisée par l’installation. |
| **W ou kW** | Puissance à cet instant. 1 kW correspond à 1 000 W. |
| **kWh / JOUR** | Quantité d’énergie cumulée sur la journée. |
| **GEN / SMARTLOAD** | Entrée ou sortie supplémentaire selon le modèle et les réglages de l’installation. |
| **Voiture** | État de la recharge lorsque cette fonction a été configurée. |

Un tiret ou une indication indisponible signifie que l’écran ne dispose pas d’une mesure exploitable. Cela ne signifie pas forcément zéro.

Certaines informations dépendent du modèle d’onduleur, des équipements présents et de leur communication. Les compteurs journaliers dépendent aussi d’une heure correcte.

## Se repérer dans les menus

Le menu **CONFIGURATION** regroupe les réglages :

| Menu | Utilisation |
| --- | --- |
| **DEYE / SOLARMAN** | Choisir l’onduleur, connecter son logger et régler les sources de production. |
| **TARIFS / HEURES CREUSES** | Configurer les informations tarifaires utilisées par les fonctions prévues. |
| **VÉHICULE ÉLECTRIQUE** | Choisir la borne et accéder à sa page de recharge. |
| **RELAIS** | Configurer la règle du relais de l’écran, si cette fonction est utilisée. |
| **RÉSEAU / HEURE** | Connexion Wi-Fi, fuseau et réglage de l’heure. |
| **AFFICHAGE** | Thème, luminosité et veille horaire. |
| **MISE À JOUR** | Chercher et installer une nouvelle version ; ouvrir ce guide avec le QR code. |

**RETOUR** quitte la page. **SAUVEGARDER**, **ENREGISTRER** ou **APPLIQUER** valide les modifications selon la page. Laisser le redémarrage se terminer lorsqu’il est annoncé.

Les menus de registres et de coefficients servent à adapter les mesures à un matériel particulier. Pour une utilisation courante, conserver les valeurs prévues pour le modèle choisi ; les modifier avec les informations de l’installateur si nécessaire.

## Configurer une borne de recharge

Si vous n’utilisez pas la fonction de recharge de l’écran, choisir **Aucune**.

Sinon, ouvrir **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, sélectionner le système utilisé et enregistrer :

| Choix | Équipement concerné |
| --- | --- |
| **Aucune** | Utilisation du suivi Deye sans pilotage d’une borne par l’écran. |
| **Deye LoRa** | Système Deye compatible avec le profil **SUN-12K-SG02LP1-EU-AM2**. |
| **Vetronic WB01** | Borne WB01 accompagnée de la passerelle ESP32 compatible avec ce projet. |

Après le redémarrage, ouvrir la page de la borne. Pour une WB01, renseigner l’adresse IP de sa passerelle. La configuration de cette passerelle reste distincte de celle de l’écran.

Les modes disponibles dépendent du système choisi. Ils permettent notamment d’arrêter la charge, de demander une charge immédiate ou d’utiliser le solaire. Avec WB01, **Rendre la main à la borne** permet de la laisser fonctionner de façon autonome.

Changer le choix de borne ou désactiver son utilisation dans l’écran n’arrête pas une charge déjà en cours. Pour arrêter une charge, utiliser d’abord sa commande d’arrêt.

Les horaires et tarifs doivent être vérifiés dans les réglages. Ils ne prennent pas automatiquement le contrôle de toutes les charges lancées depuis un autre appareil.

## Régler l’affichage et l’heure

Dans **AFFICHAGE > THÈME / LUMINOSITÉ**, choisir le thème clair ou sombre et les niveaux de luminosité souhaités.

Dans **AFFICHAGE > VEILLE HORAIRE**, régler les plages de veille si vous souhaitez que l’écran s’éteigne à certaines heures. Un toucher permet de le réveiller temporairement selon le délai configuré.

Dans **RÉSEAU / HEURE > HEURE / NTP**, vérifier le fuseau et la synchronisation. Pour la France métropolitaine, utiliser **Europe/Paris**.

Sans synchronisation Internet, choisir **RÉGLER L’HEURE MANUELLEMENT**, saisir la date et l’heure locales, puis appliquer. Après une coupure d’alimentation, régler de nouveau l’heure si elle ne peut pas se synchroniser automatiquement.

## Ouvrir l’écran sur un téléphone ou un ordinateur

1. Connecter le téléphone ou l’ordinateur au réseau de la maison.
2. Relever l’adresse IP de l’écran dans sa page Wi-Fi.
3. Saisir cette adresse dans la barre d’adresse du navigateur, par exemple `http://192.168.1.50` en remplaçant l’exemple par l’adresse réelle.
4. Si une identification est demandée, utiliser le compte configuré pour l’écran.

La page web donne accès à la configuration, au tableau de bord et aux outils de diagnostic.

Pour conserver une copie des réglages, utiliser l’**export de configuration JSON** et garder le fichier sur le téléphone ou l’ordinateur. Il est conseillé d’en faire une copie avant une mise à jour ou un changement important de réglages. Le fichier d’import doit correspondre à la configuration et au modèle utilisés.

## Mettre l’écran à jour

Sur un écran qui possède le menu **MISE À JOUR** :

1. Vérifier que le Wi-Fi et l’heure sont corrects.
2. Ouvrir **CONFIGURATION > MISE À JOUR**.
3. Toucher **VÉRIFIER SUR GITHUB**.
4. Si une nouvelle version est proposée, toucher **INSTALLER**, puis confirmer.
5. Attendre la fin de l’installation et le redémarrage.

**L’écran va scintiller pendant l’installation. C’est normal. Ne coupez pas l’alimentation jusqu’au redémarrage.**

Une mise à jour compatible conserve les réglages. Si l’écran indique qu’il est à jour, aucune installation supplémentaire n’est nécessaire. Si une erreur apparaît, lire le message et réessayer après avoir corrigé la connexion ou l’heure.

Sur une ancienne version sans ce menu, télécharger le **firmware OTA** depuis [la dernière release](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest), puis utiliser le formulaire de mise à jour de la page web de l’écran. Le dépôt public distribue uniquement les binaires OTA pour un écran déjà installé avec un partitionnement compatible.

## Résoudre les problèmes courants

| Problème | Vérification à faire |
| --- | --- |
| L’écran ne se connecte pas au Wi-Fi | Vérifier le réseau choisi, le mot de passe et la portée du Wi-Fi. |
| Le Wi-Fi fonctionne, mais les mesures ne s’affichent pas | Vérifier le type de logger, son adresse IP, son numéro, son port et le modèle d’onduleur. |
| La recherche ne trouve pas le logger | Vérifier qu’il est allumé et joignable sur le même réseau ; saisir son IP connue si nécessaire. |
| Plusieurs adresses sont proposées | Choisir le bon équipement dans **RESULTATS** avant de sauvegarder. |
| Une mesure affiche un tiret | Vérifier la liaison et la présence de cette mesure sur le modèle utilisé. |
| Le solaire affiche zéro la nuit | C’est normal en l’absence de production des panneaux. |
| L’heure ou les compteurs du jour sont incohérents | Vérifier le fuseau, la date et l’heure. |
| L’écran est noir à certaines heures | Vérifier la veille horaire et toucher l’écran pour le réveiller. |
| La page web ne s’ouvre pas | Vérifier l’IP actuelle de l’écran et la connexion du téléphone au réseau de la maison. |
| La mise à jour échoue | Vérifier l’accès Internet et l’heure, puis réessayer avec une alimentation stable. |
| La borne n’apparaît pas ou ne répond pas | Vérifier son choix dans **ACTIVATION / TARIFS** et, pour WB01, l’IP de sa passerelle. |

Si le problème persiste, noter le modèle d’onduleur, le type de logger, la version du firmware, le message affiché et les réglages de connexion. Ces informations aideront la personne qui accompagne votre installation.

## Liens utiles

- [Dernière version et téléchargements](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest).
- Mise à jour depuis le menu de l’écran ou sa page Web OTA.
- [Guide détaillé de la version 4.7.0](GUIDE_UNIFIE_4.7.0.md).
- [Projet Deye Monitor sur GitHub](https://github.com/Chasticot/DEYE_LVGL_GUITION).
