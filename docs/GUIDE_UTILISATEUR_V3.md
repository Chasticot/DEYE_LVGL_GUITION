# Deye Monitor V3
## Guide utilisateur complet

**Firmware 4.3.3 - Édition du 2 octobre 2026**

Ce guide accompagne l'écran tactile Deye Monitor. Il explique comment relier l'écran au logger de l'onduleur, lire les mesures et utiliser les menus. Les libellés reprennent ceux du logiciel, parfois affichés sans accents.

### Le principe

L'onduleur communique avec son logger. L'écran rejoint le réseau de la maison et interroge ce logger pour afficher les mesures. Un téléphone ou un ordinateur connecté au même réseau peut également ouvrir la page Web de l'écran.

**Onduleur > logger > réseau de la maison > écran Deye Monitor**

Pour les mesures locales, l'écran n'a pas besoin des identifiants de votre compte DeyeCloud ou Solarman. Internet sert notamment à récupérer l'heure, les informations Tempo et à rechercher les mises à jour.

### Parcours de lecture

1. Préparer l'installation et installer la V3.
2. Retrouver l'adresse IP et le numéro de série du logger.
3. Connecter l'écran et obtenir les premières mesures.
4. Choisir le bon modèle Deye.
5. Comprendre le tableau de bord.
6. Régler Production PV / GEN et les registres.
7. Régler les tarifs et la recharge VE.
8. Configurer le relais, l'affichage et le réseau.
9. Utiliser le Web, sauvegarder, mettre à jour et dépanner.

### Deux adresses à distinguer

| Adresse | À quoi sert-elle ? |
|---|---|
| IP du logger | L'écran l'utilise pour lire l'onduleur. |
| IP de l'écran | Le téléphone ou l'ordinateur l'utilise pour ouvrir Deye Monitor dans un navigateur. |

**Les adresses données en exemple dans ce guide ne doivent pas être recopiées telles quelles.** Utilisez celles de votre installation.

<!-- pagebreak -->

## 1. Préparer l'installation

### Ce qu'il faut avoir sous la main

- Un écran compatible avec ce firmware ; la variante matérielle prévue pour le relais est l'ESP32-4848S040C à un relais.
- La référence complète de l'onduleur, avec sa puissance et ses suffixes, relevée sur son étiquette.
- Un logger compatible avec la communication locale Solarman V5 utilisée par ce firmware.
- Le nom et le mot de passe du Wi-Fi 2,4 GHz de la maison.
- L'IP locale et le numéro de série numérique du logger, à retrouver au chapitre 2.
- Un téléphone ou un ordinateur sur le réseau local pour la configuration Web.

Le logger doit déjà être relié au réseau de la maison. Si ce n'est pas le cas, suivez sa notice ou l'assistant réseau Solarman/Deye correspondant à son modèle [S1]. La présence de mesures dans le cloud ne prouve pas à elle seule que le logger accepte les lectures locales de l'écran.

### Écran déjà équipé d'une version précédente

1. Ouvrez la page Web de l'écran, puis exportez sa configuration JSON.
2. Installez le fichier applicatif V3 par **Mise à jour OTA**, comme décrit au chapitre 12.
3. Après redémarrage, choisissez votre modèle dans **DEYE / SOLARMAN > MODELE DEYE**.
4. Vérifiez l'IP et le numéro du logger, puis comparez les mesures au LCD de l'onduleur.

Sur le même écran, les réglages V2 du profil SG02 d'origine sont repris lorsque le profil V3 ne dispose pas encore de sa propre configuration. Un modèle différent démarre avec ses valeurs par défaut. Les anciens réglages R536 en LP1 sont corrigés en R62.

Mise à jour 4.3.3 : AI-W5.1 P1 utilise désormais la cartographie monophasée validée par retour utilisateur le 2 octobre 2026. Les anciennes adresses sauvegardées de ce seul profil sont remplacées automatiquement ; les autres modèles sont conservés. Ne réimportez pas un ancien JSON AI-W5.1 contenant les adresses 600 ; faites un nouvel export après mise à jour.

### Écran neuf ou logiciel incompatible

Téléchargez le pack Windows V3 4.3.3 dans les Releases GitHub, extrayez tout le ZIP puis lancez INSTALLER.bat. Un CH340 unique est sélectionné automatiquement ; sinon choisissez le port COM. Suivez les étapes et confirmez l'installation. Le pack contient tous les binaires pour l'installation USB de l'écran prévu par ce projet. Le seul fichier applicatif utilisé en OTA ne constitue pas nécessairement une installation complète pour une mémoire vierge.

Pour une installation depuis les sources : ouvrir le projet Deye-LVGL dans PlatformIO, sélectionner l'environnement **12KSG02LP1_v3**, compiler, puis téléverser sur le bon port USB. La V3 est l'environnement par défaut du dépôt ; vérifiez le choix avant téléversement. Un écran déjà préparé peut passer directement au chapitre 2.

<!-- pagebreak -->

## 2. Retrouver l'IP du logger

### Méthode A - Dans l'interface de votre box

1. Connectez le téléphone ou l'ordinateur au réseau de la maison.
2. Ouvrez l'application ou la page d'administration de la box.
3. Cherchez **Appareils connectés**, **Réseau local** ou **Baux DHCP**, selon la box.
4. Identifiez le logger à l'aide de son nom, de son numéro de série ou de son adresse MAC. Ne choisissez pas l'écran Deye Monitor.
5. Notez son adresse IPv4, par exemple `192.168.1.45`.
6. Si la box le permet, réservez cette adresse au logger avec un **bail DHCP fixe** : elle restera la même après redémarrage [S1].

### Méthode B - Depuis le Wi-Fi du logger

Cette procédure concerne les loggers Solarman qui proposent une page Web locale. Les autres modèles peuvent utiliser une procédure différente.

1. Dans les réglages Wi-Fi du téléphone, rejoignez le réseau du logger, souvent nommé **AP_ suivi de son numéro de série**. Utilisez le mot de passe indiqué sur son étiquette ou sa notice [S1].
2. Acceptez de rester connecté si le téléphone signale « pas d'accès Internet ». Si nécessaire, coupez temporairement les données mobiles pour éviter un changement automatique de réseau.
3. Ouvrez explicitement **http://10.10.100.254** dans le navigateur. Utilisez les identifiants du logger. Sur les modèles Solarman documentés avec les identifiants d'origine, ils sont `admin` / `admin` ; ils peuvent avoir été changés [S2].
4. Ouvrez **Status / Device information**. Dans **Wireless STA mode**, relevez **IP address**, ainsi que le nom du routeur connecté. En Ethernet, utilisez l'adresse de la rubrique **Cable / LAN mode** [S3].
5. Relevez aussi **Device serial number**, le numéro du logger.
6. Reconnectez le téléphone au Wi-Fi de la maison.

**Ne saisissez pas automatiquement 10.10.100.254 dans Deye Monitor.** C'est l'adresse de configuration du point d'accès du logger. Dans une installation reliée à la box, l'écran doit utiliser l'adresse locale relevée dans STA ou LAN.

Une IP vide, `0.0.0.0` ou un état déconnecté indique qu'il faut d'abord rétablir la connexion du logger à la box. Si le réseau AP est masqué ou absent, utilisez la méthode A ; ne réinitialisez pas le logger simplement pour retrouver son IP.

<!-- pagebreak -->

## 3. Première mise en route de l'écran

### Étape 1 - Connecter le Wi-Fi

1. Touchez le bouton de configuration en haut à droite du tableau de bord, représenté par des curseurs.
2. Ouvrez **RESEAU / HEURE > WIFI / RESEAU**.
3. Touchez **SCAN**, puis sélectionnez le réseau 2,4 GHz de la maison dans la liste.
4. Saisissez le mot de passe et touchez **SAUVEGARDER**. L'écran redémarre.
5. Revenez dans ce menu pour relever **IP Serveur Web/Ecran**. Notez cette adresse séparément de celle du logger.

Si un mot de passe est déjà enregistré, laisser le champ vide le conserve. Pour changer de réseau, saisissez le mot de passe du nouveau réseau. La recherche Wi-Fi peut interrompre temporairement la liaison.

### Étape 2 - Choisir l'onduleur

Ouvrez **DEYE / SOLARMAN > MODELE DEYE**, déroulez la liste, choisissez la référence puis **ENREGISTRER**. Attendez le redémarrage avant de poursuivre. La correspondance des modèles est détaillée au chapitre 4.

### Étape 3 - Renseigner le logger

Dans **DEYE / SOLARMAN > CONNEXION / LOGGER**, saisissez :

| Champ | Valeur à utiliser |
|---|---|
| Adresse IP du logger | L'adresse STA/LAN locale, par exemple `192.168.1.45`. Sans `http://` ni chemin. |
| Numéro de série du logger | Le numéro numérique du logger, pas celui de l'onduleur, du compte cloud ou de l'installation. |

Touchez **SAUVEGARDER**, puis attendez le redémarrage. Les valeurs d'exemple préremplies ne correspondent pas à votre installation. Un identifiant avec des lettres n'est pas accepté par ce firmware : ne le transformez pas arbitrairement en nombre, vérifiez le type de logger.

### Étape 4 - Régler l'heure et contrôler

Dans **RESEAU / HEURE > HEURE / NTP**, choisissez **Europe/Paris** pour la France métropolitaine. Les serveurs proposés sont `pool.ntp.org` et `time.google.com`. Sauvegardez et attendez la synchronisation.

Au tableau de bord, comparez le SOC batterie, la tension, le réseau et la consommation avec le LCD de l'onduleur. La nuit, une production PV nulle est normale. Dans **CONNEXION / LOGGER**, les compteurs OK doivent progresser pour les blocs utilisés ; le bloc VE peut rester sans lecture lorsqu'il est désactivé.

<!-- pagebreak -->

## 4. Modèles et organisation des menus

Le choix du modèle règle automatiquement les adresses et les conversions. Le nombre de lignes PV indiqué ci-dessous est celui du profil logiciel ; il ne signifie pas que toutes les entrées sont câblées.

| Référence proposée dans le menu | Lignes PV | GEN Daily |
|---|---|---|
| SUN-12K-SG02LP1-EU-AM2 | 3 | R62 |
| SUN-12K-SG02LP1-EU-AM3 | 3 | R62 |
| SUN-8K-SG01LP1-EU | 2 | R62 |
| SUN-6K-SG03LP1-EU | 2 | R62 |
| SUN-5K-SG05LP1-EU-AM2-P | 2 | R62 |
| SUN-6K-SG05LP1 | 2 | R62 |
| SUN-8K-SG05LP1-EU | 2 | R62 |
| SUN-12K-SG05LP3-EU-SM2 | 2 | R536 |
| SUN-10K-SG04LP3-EU | 2 | R536 |
| SUN-20K-SG01HP3-EU-AM2 | 4 | R536 |
| SUN-25K-SG01HP3-EU-AM2 | 4 | R536 |
| AI-W5.1 ESS (P1) | 2 | Sans GEN ; registres confirmés par retour utilisateur |
| SUN-6K-SG06LP1 | - | Sélection indisponible |

Le libellé court 12KSG05LP3-EU des retours utilisateurs est regroupé avec l'entrée SM2. « SG05LP1 » seul désigne une famille : choisissez aussi la puissance et, lorsqu'il est indiqué, le suffixe. Un modèle absent ne doit pas être considéré compatible sur la seule ressemblance de son nom.

### Les six rubriques principales

| Menu | Contenu |
|---|---|
| DEYE / SOLARMAN | Modèle, connexion/logger, production PV/GEN, registres perso. |
| TARIFS / HEURES CREUSES | Deux plages ou Tempo, affichage des couleurs. |
| VEHICULE ELECTRIQUE | Recharge Deye LoRa et conditions tarifaires. |
| RELAIS | Règle de commande du relais intégré. |
| RESEAU / HEURE | Wi-Fi et synchronisation de l'heure. |
| AFFICHAGE | Thème, luminosité et veille horaire. |

Faites défiler les formulaires pour voir les réglages situés plus bas. **RETOUR** quitte le formulaire ; **ENREGISTRER**, **SAUVEGARDER** ou **APPLIQUER** valide les changements selon la page. La plupart des réglages entraînent un redémarrage.

<!-- pagebreak -->

## 5. Lire le tableau de bord

### Puissance et énergie

**W ou kW : ce qui se passe maintenant. kWh : ce qui s'est cumulé dans le temps.** Un appareil qui consomme 1 000 W pendant une heure utilise 1 kWh.

| Indication | Signification |
|---|---|
| Production / PV | Puissance issue des entrées PV visibles, avec ajout éventuel de GEN MO. |
| Production du jour | Énergie produite sur la journée, selon les compteurs disponibles et le cumul choisi. |
| Consommation / Load | Puissance consommée suivie par l'onduleur ; le périmètre dépend de l'installation et des capteurs. |
| Consommation du jour | Énergie consommée depuis le début de la journée selon le compteur de l'onduleur. |
| Réseau | Importation depuis le réseau ou exportation vers celui-ci. Les libellés indiquent le sens. |
| Batterie / SOC | Niveau de charge en %, tension et puissance. Charge positive ; décharge négative dans cette interface. |
| GEN / SmartLoad | Production sur le port GEN ou état SmartLoad, selon le mode choisi. |
| Températures | Mesures internes transmises par l'onduleur. |

**Exemple :** les panneaux ont produit 20 kWh et la maison en a consommé 15. Ces nombres n'ont pas à être identiques : une partie de la production peut charger la batterie ou être exportée. GEN Daily mesure une énergie liée au port GEN ; ce n'est pas la consommation de la maison.

### Les états à connaître

- **CHARGE / DECHARGE** : sens de circulation de l'énergie de la batterie.
- **HC / HP** : période tarifaire calculée avec les réglages horaires.
- **ON / OFF près du Wi-Fi** : état du relais intégré.
- **~ devant les kWh GEN** : estimation calculée à partir de la puissance.
- **DEYE INJOIGNABLE** : aucune lecture Modbus réussie depuis environ deux minutes.

La disparition du message de liaison ne prouve pas que tous les compteurs sont valides : une seule réponse réussie peut suffire. Consultez le diagnostic si une valeur reste absente ou incohérente. Une valeur indisponible ne doit pas être interprétée comme zéro.

Les compteurs journaliers de l'onduleur et les mesures du cloud peuvent avoir des heures de mise à jour différentes. Pour contrôler une puissance instantanée, comparez d'abord au LCD local au même moment.

<!-- pagebreak -->

## 6. Production PV / GEN

**Chemin : Configuration > DEYE / SOLARMAN > PRODUCTION PV / GEN**

### Afficher PV1, PV2, PV3 ou PV4

Ces interrupteurs choisissent les lignes affichées et les puissances incluses dans le total et son historique. Ils ne coupent pas les panneaux et ne modifient pas le fonctionnement électrique de l'onduleur.

Les options PV3/PV4 n'apparaissent que sur les profils concernés. Masquez une ligne inutilisée si nécessaire. Le compteur journalier PV reste global : masquer une entrée ne retire pas automatiquement ses kWh du compteur de l'onduleur. Quand toutes les lignes PV sont masquées, ce compteur est omis du total affiché.

### Mode SMARTLOAD ou GEN MO

| Mode | Usage dans Deye Monitor |
|---|---|
| SMARTLOAD | Afficher l'état de la sortie SmartLoad lorsque le profil et l'appareil le permettent. |
| GEN MO | Suivre la puissance et l'énergie de production reçues sur le port GEN, notamment des micro-onduleurs. |

Ce choix adapte l'interprétation de l'écran. **Il ne reconfigure pas le port GEN de l'onduleur.** Le réglage doit correspondre à l'utilisation réelle du port, configurée sur l'onduleur.

**Cumuler GEN MO + PV** ajoute la puissance GEN positive au total en mode GEN MO. Pour le total journalier, vérifiez que le compteur PV de votre onduleur n'inclut pas déjà cette énergie, afin d'éviter de la compter deux fois. Les commandes GEN sont masquées sur AI-W5.1.

### Compteur GEN du jour

Le réglage se trouve dans **REGISTRES PERSO > GEN Daily** :

- **R62 en LP1 ; R536 en LP3/HP3**, coefficient par défaut **0,1** : lecture directe du compteur journalier.
- **Adresse vide ou 0** : estimation. L'écran additionne la puissance positive au fil du temps, après synchronisation de l'heure, et affiche `~`.

L'estimation repart à zéro à minuit local. Elle est sauvegardée environ toutes les cinq minutes ; un redémarrage peut perdre la dernière période non sauvegardée. Les coupures et les intervalles de mesure supérieurs à 45 secondes ne sont pas extrapolés.

Une ancienne configuration à 0 reste en estimation après mise à jour. Pour passer au compteur, renseignez l'adresse adaptée dans GEN Daily puis sauvegardez. Si le compteur nécessaire est invalide, le total journalier peut rester indisponible.

<!-- pagebreak -->

## 7. Registres personnalisés

**Chemin : Configuration > DEYE / SOLARMAN > REGISTRES PERSO**

Le profil du modèle fournit les valeurs de départ. Ce menu sert à adapter une adresse ou une conversion lorsqu'une différence a été identifiée pour votre appareil. Exportez la configuration avant une modification.

| Groupe | Champs et rôle |
|---|---|
| Solaire | PV1/2/3/4 Power : puissances ; PV Daily : compteur journalier. |
| Batterie | SOC, Volt, Power, Temp : niveau, tension, puissance et température. |
| Réseau | Grid Power, Status, Buy, Sell : puissance, état et énergies achetée/vendue. |
| Consommation | Load Power et Load Daily : puissance et énergie quotidienne. |
| Port GEN | GEN Power et GEN Daily : puissance et énergie quotidienne. |
| Températures | DC Temp et AC Temp. |
| SmartLoad | Adresse et numéro de bit utilisé pour l'état. |
| Communications | Connect, Response, Frame et Block Interval, en millisecondes. |

### Adresse, coefficient et bit

L'adresse désigne la donnée à lire. Le coefficient adapte son unité ou son signe. Pour GEN Daily, une valeur brute de 123 avec le coefficient 0,1 donne **12,3 kWh**.

Pour le réseau et la consommation, un facteur de base est déjà appliqué par le logiciel avant le coefficient. N'utilisez donc pas le même coefficient au hasard sur toutes les familles. Un écart d'un facteur dix doit être vérifié avec le modèle sélectionné et sa calibration.

Le numéro de bit SmartLoad n'est pas un coefficient. Un mauvais bit peut donner un état ON/OFF plausible mais incorrect ; comparez un changement réel avec le LCD.

**65535 signifie « absent » uniquement pour les mesures optionnelles prévues : PV3, PV4, GEN Power ou SmartLoad.** GEN Daily utilise **0** pour l'estimation. Les adresses obligatoires et les blocs de lecture sont contrôlés avant sauvegarde.

### Réinitialiser et sauvegarder

**REINITIALISER** remplit les champs avec les valeurs du modèle actif. Cela peut remplacer vos calibrations personnalisées. Il faut ensuite sauvegarder pour les appliquer ; quitter sans sauvegarder permet de conserver la configuration enregistrée.

Les délais de communication ne sont pas des adresses. En cas de liaison lente, commencez par vérifier le Wi-Fi et le diagnostic avant de modifier ces valeurs. Les paramètres de lecture de ce menu ne sont pas ceux du verrou d'écriture VE, présenté au chapitre 8.

<!-- pagebreak -->

## 8. Tarifs et véhicule électrique

### TARIFS / HEURES CREUSES

**2 plages** : saisissez début et fin au format HH:MM pour chaque plage. Une plage peut traverser minuit. Début = fin la désactive. L'heure de début est incluse, celle de fin exclue. Exemple : 22:30-06:30 et 12:00-14:00, à remplacer par vos horaires réels.

**Tempo** : le logiciel utilise les heures creuses 22:00-06:00 et le changement de jour tarifaire à 06:00. Sélectionnez Europe/Paris pour une installation française. **Afficher Tempo** active la récupération et l'affichage de ses informations ; **Couleurs accessibles** ajoute le nom de la couleur. Ces réglages ne modifient pas votre contrat d'électricité.

### VEHICULE ELECTRIQUE > ACTIVATION / TARIFS

La recharge Deye LoRa est prise en charge dans cette version uniquement par le profil **SUN-12K-SG02LP1-EU-AM2**. L'AM3 et les autres modèles n'activent pas cette fonction.

| Option | Effet |
|---|---|
| Activer VE Deye LoRa | Rend accessible le suivi et les commandes sur le profil pris en charge. |
| Appliquer les tarifs | Active les restrictions automatiques choisies ci-dessous. |
| Réseau en HC seulement | Empêche le mode Libre hors heures creuses. |
| Interdire HP rouge Tempo | En mode Tempo, interdit le réseau en heures pleines rouges. |

### VEHICULE ELECTRIQUE > RECHARGE DEYE LORA

Choisissez **ARRET**, **SOLAIRE** ou **LIBRE**, puis le plafond de puissance et **APPLIQUER**. Les boutons +/-, le curseur et la saisie permettent de régler le plafond entre **1 400 et 7 400 W**. Le plafond affiché n'est pas une mesure de la consommation réelle de la borne. Contrôlez le **Mode lu** et le résultat de la commande.

Les écritures sont verrouillées par défaut. Leur déblocage se fait sur le Web, dans **VE / registres d'écriture**, après vérification des registres et confirmation explicite. Sans liaison valide, profil adapté ou autorisation, les commandes restent bloquées.

Si une règle interdit Libre, le logiciel refuse la demande ou tente de basculer vers Solaire. Il peut rétablir Libre si c'est lui qui avait imposé ce changement pendant le démarrage en cours ; une commande manuelle annule cette reprise. Une heure ou une couleur nécessaire mais inconnue interdit le réseau. Cette automatisation dépend du logger et de l'onduleur : ce n'est pas une coupure électrique indépendante.

<!-- pagebreak -->

## 9. Relais intégré

**Chemin : Configuration > RELAIS**

Cette fonction commande le relais de l'écran **ESP32-4848S040C à un relais**, via GPIO40. Elle ne correspond pas au relais SmartLoad de l'onduleur. Les variantes d'écran à trois relais ne sont pas pilotées par cette configuration.

### Construire une règle

Le principe affiché est : **ON si (registre × coefficient) satisfait la comparaison ; OFF sinon.**

| Champ | Explication |
|---|---|
| Activer la règle | Autorise la commande automatique. Désactivée par défaut. |
| Registre Modbus | Adresse à lire pour la condition. |
| Valeur signée | À activer si la donnée peut être négative. |
| Coefficient | Conversion propre à cette règle, indépendante du tableau de bord. |
| Comparateur | Inférieur à, égal à ou supérieur à. L'égalité est exacte. |
| Seuil | Valeur à comparer après conversion. |
| Délai commutation | Durée pendant laquelle la condition doit rester stable avant ON ou OFF. |

### Exemple : activer au-dessus de 80 % de batterie

Sur un profil LP1, le SOC est R184 ; sur les profils triphasés de ce guide, R588. Utilisez une valeur non signée, le coefficient 1, le comparateur **>**, le seuil **80** et un délai de **5 secondes**.

À 81 %, la condition est vraie ; à 80 %, elle est fausse. Il n'y a pas deux seuils séparés d'enclenchement et d'arrêt : la temporisation réduit les changements rapides mais ne crée pas une hystérésis.

### En cas de perte de communication

Le relais démarre à OFF. La dernière mesure valide peut être utilisée pendant cinq minutes malgré une panne de liaison. Après ce délai sans nouvelle mesure, OFF est forcé. La page indique l'âge de la mesure ; une valeur ancienne ne décrit pas nécessairement l'état actuel de la batterie.

Les lectures et la règle continuent pendant la navigation et la veille. L'état du relais est également affiché près du Wi-Fi.

Le paramétrage logiciel ne définit pas la charge électrique admissible du relais. Pour le raccordement, respectez les caractéristiques du matériel et faites intervenir une personne qualifiée si vous commandez une charge secteur.

<!-- pagebreak -->

## 10. Affichage, veille et heure

### AFFICHAGE > THEME / LUMINOSITE

Choisissez le thème **Sombre** ou **Clair**, puis les niveaux de luminosité jour et nuit. Le firmware 4.3.3 limite les valeurs brutes à **220-255** ; 255 correspond au maximum. Le curseur applique cette plage : elle n'est pas une échelle linéaire de luminosité perçue.

**Mode nuit programmé** active la luminosité nocturne. Choisissez les heures de début et de fin. Si le mode est désactivé, la luminosité jour est utilisée en permanence. Sans heure synchronisée, le niveau jour est conservé.

**Suivre coucher / lever du soleil** remplace les horaires fixes par des horaires calculés. Renseignez la latitude et la longitude de l'installation sur la page Web ; les coordonnées initiales correspondent au centre de la France. La transition est progressive autour du lever et du coucher.

Touchez **APPLIQUER** pour sauvegarder ; l'écran redémarre.

### AFFICHAGE > VEILLE HORAIRE

La veille coupe le rétroéclairage dans une plage horaire. Elle est désactivée par défaut ; la plage proposée est **23:00-07:00**, avec un réveil tactile de **60 secondes**, réglable de 1 à 3 600 secondes.

1. Activez la veille.
2. Choisissez le début, la fin et la durée du réveil.
3. Enregistrez et attendez le redémarrage.

L'écran attend la synchronisation de l'heure et laisse un délai de réveil avant de s'éteindre. Un premier toucher réveille l'affichage sans actionner le bouton situé dessous ; les touches suivantes prolongent le réveil.

**La veille n'arrête pas l'appareil.** Les mesures, le serveur Web, les tarifs et le relais continuent de fonctionner. Elle se distingue du mode nuit, qui réduit la luminosité sans éteindre le rétroéclairage.

### RESEAU / HEURE > HEURE / NTP

Le fuseau choisi gère les changements saisonniers lorsqu'ils sont prévus dans sa règle. Conservez les serveurs NTP par défaut si votre réseau les autorise. Une heure absente ou incorrecte affecte les plages tarifaires, Tempo, la veille et le compteur GEN estimé.

Si la synchronisation échoue, vérifiez la connexion Internet et le DNS. En IP statique, une passerelle ou un DNS incorrect peut laisser fonctionner les lectures locales tout en empêchant NTP.

<!-- pagebreak -->

## 11. Utiliser la page Web

Dans **RESEAU / HEURE > WIFI / RESEAU**, relevez **IP Serveur Web/Ecran**. Depuis un appareil sur le même réseau, ouvrez `http://IP_DE_LECRAN/`. Exemple : si l'écran affiche 192.168.1.60, ouvrez `http://192.168.1.60/`.

### Les pages disponibles

| Page | Utilisation |
|---|---|
| `/` | Configuration Wi-Fi, réseau, modèle, logger, heure, affichage, registres, options, accès Web et OTA. |
| `/dashboard` | Tableau de bord, rafraîchi automatiquement toutes les cinq secondes. |
| `/history` | Jusqu'à 24 h de mesures, un point toutes les cinq minutes. |
| `/diagnostic` | Âge des blocs, lectures réussies/échouées et dernière exception Modbus. Actualiser la page pour rafraîchir. |

L'historique est conservé en mémoire vive : un redémarrage l'efface. Il ne s'agit pas d'un archivage longue durée. Le lien **Sonde VE** du diagnostic est un outil avancé de comparaison en lecture seule, destiné au profil SG02 ; il n'est pas nécessaire à la mise en route.

Les rubriques de configuration Web se déplient en touchant leur titre. Chaque formulaire possède son bouton de sauvegarde. Une sauvegarde entraîne généralement un redémarrage : procédez rubrique par rubrique.

### Réseau : DHCP / IP statique

Ce formulaire règle **l'adresse de l'écran**, pas celle du logger. DHCP est le mode initial. Pour conserver une adresse stable, une réservation DHCP dans la box évite de saisir manuellement masque, passerelle et DNS.

En IP statique, saisissez une adresse libre adaptée à votre réseau, le masque et la passerelle. Un DNS vide utilise la passerelle. Après sauvegarde, ouvrez la nouvelle IP indiquée ou retrouvez-la sur l'écran. Le retour au DHCP ignore les champs statiques.

### Authentification serveur Web

Elle est désactivée initialement. Dans la rubrique correspondante, saisissez un identifiant et un mot de passe, activez la protection et sauvegardez. Un champ de mot de passe vide conserve le secret déjà enregistré.

En cas d'oubli : sur l'écran, **RESEAU / HEURE > WIFI / RESEAU**, cochez **Reset authentification serveur Web**, puis **SAUVEGARDER**. Les identifiants Web sont effacés, le Wi-Fi est conservé et l'écran redémarre. Cette action ne réinitialise pas le mot de passe du logger. Le serveur utilise HTTP et est prévu pour le réseau local ; n'exposez pas directement sa page sur Internet.

<!-- pagebreak -->

## 12. Sauvegardes et mises à jour

### Exporter la configuration

Sur la page Web, ouvrez **Export / import de configuration JSON**, puis **Télécharger la configuration JSON**. Rangez le fichier avec le modèle et la date dans son nom.

L'export contient la configuration du modèle actif et les paramètres généraux inclus, mais pas tous les profils sauvegardés en une seule fois. Il ne contient ni le mot de passe Wi-Fi ni les identifiants Web. Exportez séparément chaque modèle si vous souhaitez en conserver une copie externe.

### Importer un fichier

1. Sélectionnez d'abord le modèle correspondant au fichier, puis laissez l'écran redémarrer.
2. Sur le Web, choisissez le fichier JSON dans la rubrique d'import.
3. Touchez **Importer et redémarrer**.
4. Après redémarrage, vérifiez modèle, logger et mesures.

Un fichier V3 portant l'identifiant d'un autre modèle est refusé. Un ancien export sans modèle est accepté uniquement sur le profil SG02 d'origine. Les mots de passe ne sont pas remplacés par l'import. Les anciens R536 sont corrigés lors de la sauvegarde sur les profils LP1 concernés ; une adresse 0 et les autres adresses personnalisées restent conservées.

### Mettre à jour par Wi-Fi : OTA

1. Exportez votre configuration.
2. Récupérez le **fichier applicatif compatible avec votre écran et la variante V3**. Dans les sources compilées, il se nomme `firmware.bin` ; une publication peut le renommer.
3. Ouvrez **Mise à jour OTA** sur la page Web. La version installée y est affichée.
4. Sélectionnez le fichier `.bin`, puis **Installer la mise à jour**.
5. Gardez l'écran alimenté pendant l'envoi et attendez le message de réussite puis le redémarrage.
6. Rouvrez la page, contrôlez la version et les mesures.

N'utilisez pas un bootloader, une table de partitions ou une image USB fusionnée dans ce formulaire. Les réglages persistants sont conservés, tandis que l'historique en mémoire vive repart à zéro.

Le bouton **Vérifier si une version plus récente existe** consulte la dernière release stable du dépôt GitHub depuis le navigateur. Il nécessite Internet et ne garantit pas que le fichier proposé correspond à votre variante. La version 4.3.3 décrite ici a été compilée localement ; sa présence dans une release publique n'est pas présumée.

<!-- pagebreak -->

## 13. Dépannage

| Symptôme | Vérifications utiles |
|---|---|
| Wi-Fi de la maison absent | Vérifier le 2,4 GHz, rapprocher l'écran, refaire SCAN. |
| L'écran n'obtient pas d'IP | Vérifier le mot de passe et le DHCP de la box. |
| Impossible d'ouvrir le Web | Utiliser l'IP de l'écran, avec http:// ; vérifier que le téléphone est sur le même réseau, sans isolation Wi-Fi invité. |
| DEYE INJOIGNABLE | Contrôler l'IP STA/LAN du logger, son numéro numérique et sa présence sur le réseau. |
| Tout est visible dans le cloud, rien sur l'écran | Vérifier l'accès local du logger ; cloud et communication locale sont deux chemins distincts. |
| Certains blocs réussissent, d'autres échouent | Vérifier le modèle exact et les adresses personnalisées ; relever les exceptions dans Diagnostic. |
| Valeur dix fois trop grande/petite | Vérifier modèle, unité et coefficient ; comparer au LCD avant modification. |
| Batterie affichée dans le mauvais sens | Vérifier le coefficient Battery Power ; l'interface utilise charge positive, décharge négative. |
| PV3/PV4 absent | Vérifier le nombre d'entrées du profil et les interrupteurs Afficher PV. |
| GEN Daily faux ou absent | Vérifier GEN MO, R62/R536 selon la famille et le coefficient 0,1 ; vérifier la réponse du compteur. |
| Total journalier trop élevé | Vérifier que PV Daily n'inclut pas déjà GEN avant de les cumuler. |
| Veille ou tarifs inopérants | Vérifier heure NTP, fuseau et activation de la fonction. |
| Relais reste ON après une perte Wi-Fi | La dernière mesure reste utilisable cinq minutes ; consulter son âge. |
| Commande VE indisponible | Vérifier profil AM2 pris en charge, activation, liaison, verrou d'écriture et restrictions tarifaires. |
| Import refusé | Sélectionner le modèle correspondant au JSON et vérifier son origine. |

### Contrôle technique de la liaison

Le firmware utilise **Solarman V5 sur TCP 8899**, avec l'adresse Modbus **1**. Ces valeurs ne sont pas réglables dans le formulaire Connexion / Logger. Un logger utilisant un autre protocole, un autre port ou un identifiant incompatible demande une vérification technique.

Si l'IP du logger change après un redémarrage de la box, mettez à jour l'écran et réservez ensuite cette adresse dans la box. Si les échecs sont intermittents, vérifiez aussi le signal et les autres logiciels qui interrogent simultanément le logger.

### Pour demander de l'aide

Indiquez la référence complète, le modèle sélectionné, la version de Deye Monitor, le modèle du logger, le symptôme, une photo du LCD et les compteurs/âges du diagnostic. Un export JSON aide à comprendre les réglages ; il peut contenir des identifiants d'appareil et le nom du réseau, à masquer avant publication publique. Ne communiquez pas vos mots de passe.

<!-- pagebreak -->

## 14. Repères, limites et sources

### Ce que la V3 apporte

Le choix du modèle, les profils de registres et de conversion, PV4, les réglages par modèle et le contrôle du modèle à l'import constituent les principaux changements. La correction GEN Daily a également été reportée dans la V2 4.2.1.

Les tarifs, la veille, le relais, le serveur Web et la recharge Deye LoRa existaient déjà dans la base V2. Les tests logiciels et la compilation ne remplacent pas le contrôle des mesures sur chaque installation.

Les retours transmis le 1er octobre 2026 ne signalent pas de problème pour : 8K-SG01LP1, 12K-SG05LP3, 25K-SG01HP3-AM2, 5K-SG05LP1-AM2-P, 6K-SG03LP1, 6K-SG05LP1, 12K-SG02LP1-AM3, 8K-SG05LP1 et 10K-SG04LP3. Les suffixes détaillés figurent au chapitre 4. Les fonctions et versions de firmware testées ne sont pas précisées.

Le profil AI-W5.1 P1 reprend en 4.3.3 les registres confirmés par l’utilisateur sur la variante autonome ; les autres révisions et variantes P3 ne sont pas validées par ce retour. SG06LP1 apparaît dans la liste mais ne peut pas être enregistré. SmartLoad doit être confronté au LCD sur les variantes où son interprétation n'a pas été confirmée. Les profils 20/25K HP3 ne déclarent pas compatibles les modèles de puissance supérieure.

### Petit lexique

| Terme | Sens |
|---|---|
| Logger | Module de communication raccordé à l'onduleur. |
| AP | Réseau Wi-Fi émis par le logger pour sa configuration. |
| STA | Connexion du logger au Wi-Fi de la maison. |
| DHCP | Attribution automatique d'une adresse IP par la box. |
| NTP | Synchronisation de l'heure par le réseau. |
| SOC | Niveau de charge de la batterie, en %. |
| OTA | Mise à jour du logiciel par le réseau. |
| Registre | Adresse d'une donnée ou d'un réglage dans le protocole de l'appareil. |

### Sources de la procédure logger

- **[S1] SOLARMAN, Network Settings** : Wi-Fi 2,4 GHz, accès AP et réservation DHCP. [Consulter la procédure](https://helpcenter.solarmanpv.com/portal/en/kb/articles/network-settings).
- **[S2] SOLARMAN, Fixed IP Settings** : accès local 10.10.100.254 et identifiants d'origine des modèles documentés. [Consulter la procédure](https://helpcenter.solarmanpv.com/portal/en/kb/articles/fixed-ip-settings).
- **[S3] Deye, Guia de Configuração Solarman PRO**, pages 34 et 45, et **Deye Data Logger Quick Guide DL1000B-ETH**, écran Device information : distinction AP/STA/LAN. [Guide Solarman](https://pt.deyeinverter.com/deyeinverter/2021/03/19/guiasolarmanpro.pdf) ; [guide logger Deye](https://www.deyeinverter.com/deyeinverter/2026/05/20/DeyeDataLoggerQuickGuideDL1000B-ETHV11.pdf). Ces interfaces illustrent les adresses ; leur présence ne prouve pas la compatibilité Solarman V5 du logger avec ce firmware.

Les menus et comportements de ce guide ont été vérifiés dans les sources locales de la V3 4.3.3. Les protocoles de registres utilisés pour la correction GEN sont conservés dans `docs/protocols/` du projet.

