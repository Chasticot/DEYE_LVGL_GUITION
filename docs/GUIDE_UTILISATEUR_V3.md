# Deye Monitor - firmware unifié

> **Archive 4.6.3 des fonctions détaillées.** Pour la première mise en route et les versions actuelles, utiliser le [guide à jour](MISE_A_JOUR_ECRAN_GITHUB.md), la [FAQ](FAQ_ECRAN.md) et le [mémo](RECAP_UTILISATEURS_V3.md). Les anciens noms de menus, catalogues, procédures et états de validation ci-dessous décrivent cette édition historique. Le [PDF associé](pdf/GUIDE_UTILISATEUR_V3.pdf) reste lui aussi en édition 4.6.3.
## Guide utilisateur complet

**Firmware 4.6.3 - Édition du 20 octobre 2026**

Ce guide accompagne l'écran tactile Deye Monitor. Il explique comment relier l'écran au logger de l'onduleur, lire les mesures et utiliser les menus. Les libellés reprennent ceux du logiciel, parfois affichés sans accents.

Ce guide couvre le firmware unifié : le modèle Deye et la borne se choisissent dans les réglages. Les noms des fichiers V3 sont conservés pour les liens existants. [Récapitulatif des fonctions](RECAP_UTILISATEURS_V3.md) · [PDF du guide](pdf/GUIDE_UTILISATEUR_V3.pdf).

### Le principe

L'onduleur communique avec son logger. L'écran rejoint le réseau de la maison et interroge ce logger pour afficher les mesures. Un téléphone ou un ordinateur connecté au même réseau peut également ouvrir la page Web de l'écran.

**Onduleur > logger > réseau de la maison > écran Deye Monitor**

Pour les mesures locales, l'écran n'a pas besoin des identifiants de votre compte DeyeCloud ou Solarman. Internet sert notamment à récupérer l'heure, les informations Tempo et à rechercher les mises à jour.

### Parcours de lecture

1. Préparer l'installation et installer le firmware unifié.
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
- Un logger LSW/Solarman V5 ou un logger LSE/DL1000B compatible avec le Modbus TCP local proposé par le firmware. Le DL1000B reste vérifié sur simulateur ; un modèle de logger ne garantit pas à lui seul la compatibilité de toutes ses révisions.
- Le nom et le mot de passe du Wi-Fi 2,4 GHz de la maison.
- L'IP locale et le vrai numéro de série du logger : numérique pour LSW/LSE, éventuellement alphanumérique pour DL1000B.
- Un téléphone ou un ordinateur sur le réseau local pour la configuration Web.

Le logger doit déjà être relié au réseau de la maison. Si ce n'est pas le cas, suivez sa notice ou l'assistant réseau Solarman/Deye correspondant à son modèle [S1]. La présence de mesures dans le cloud ne prouve pas à elle seule que le logger accepte les lectures locales de l'écran.

### Écran déjà équipé d'une version précédente

1. Exportez la configuration JSON depuis la page Web de l'écran.
2. Si le menu existe, ouvrez **Configuration > MISE À JOUR > VÉRIFIER SUR GITHUB > INSTALLER**, puis confirmez. Sinon, envoyez le fichier applicatif OTA 4.6.3 depuis le Web (chapitre 12).
3. Après redémarrage, vérifiez le Wi-Fi, le logger et **DEYE / SOLARMAN > MODELE DEYE**.
4. Lors de la première migration depuis V3 ou Vetronic V3, choisissez explicitement **Aucune**, **Deye LoRa** ou **Vetronic WB01** dans **VEHICULE ELECTRIQUE > ACTIVATION / TARIFS**. Enregistrez et laissez redémarrer.
5. Comparez les mesures au LCD et vérifiez la liaison de la borne avant toute commande.

Les réglages communs et les configurations V3 par modèle sont conservés sur une installation compatible sans effacement. L'ancienne activation VE ne permet pas de deviner la borne : elle reste inactive jusqu'au choix explicite. Faites un nouvel export après migration. Le chapitre 12 détaille les restrictions sur les anciens JSON.

Le profil AI-W5.1 ESS P1 conserve la correction monophasée introduite en 4.3.3. Ne réimportez pas un ancien JSON de ce profil avec les adresses 600 ; contrôlez les registres puis créez un nouvel export.

### Écran neuf ou logiciel incompatible

Un écran vierge ou un changement de partitionnement nécessite une installation USB complète. Le dépôt public fournit uniquement les mises à jour OTA pour un écran déjà installé et compatible.

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

Depuis la 4.6.3, après une perte du Wi-Fi, l'écran laisse cinq secondes à la reconnexion du pilote, puis relance la connexion avec les identifiants enregistrés toutes les 30 secondes, sans limite de durée. Au démarrage et après un scan, la première tentative dispose de 30 secondes. Les réglages DHCP/IP fixe sont conservés et l'interface reste utilisable ; un scan suspend les relances. Les lectures reprennent au retour du réseau.

### Étape 2 - Choisir l'onduleur

Ouvrez **DEYE / SOLARMAN > MODELE DEYE**, déroulez la liste, choisissez la référence puis **ENREGISTRER**. Attendez le redémarrage avant de poursuivre. La correspondance des modèles est détaillée au chapitre 4.

<!-- pagebreak -->

### Étape 3 - Renseigner ou rechercher le logger

Ouvrez **DEYE / SOLARMAN > CONNEXION / LOGGER** :

| Champ | Valeur à utiliser |
|---|---|
| Type de logger | **LSW - Solarman V5**, **LSE - Ethernet Modbus TCP** ou **DL1000B - Modbus TCP**, selon le module. |
| Adresse IP | L'adresse STA/LAN locale, sans `http://` ni chemin. |
| Numéro de série | Celui du logger, pas de l'onduleur. Numérique pour LSW/LSE ; les lettres sont acceptées pour DL1000B. |
| Port | Valeur proposée : **8899 pour LSW**, **8899 pour LSE**, **502 pour DL1000B**. Modifiable si l'installation utilise un autre port. |
| Unité Modbus | L'adresse de communication configurée sur l'onduleur ; demandez-la à l'installateur si nécessaire. |

Renseignez le vrai numéro, puis touchez **RECHERCHER**. La recherche manuelle utilise les champs du formulaire sans devoir les sauvegarder d'abord. Elle peut durer plusieurs minutes et suspend temporairement les lectures Deye.

Si l'identité est confirmée, vérifiez l'IP proposée. Si l'écran indique **numéro non vérifié**, ouvrez **RESULTATS**, sélectionnez explicitement le bon équipement, puis sauvegardez. Une réponse Modbus ne prouve pas le numéro du logger. Vous pouvez aussi saisir directement une IP connue.

Touchez **SAUVEGARDER** et attendez le redémarrage. Les exemples préremplis ne correspondent pas à votre installation. Le numéro TCP reste enregistré mais n'est pas envoyé comme identifiant dans les trames Modbus TCP.

### Étape 4 - Régler l'heure et contrôler

Dans **RESEAU / HEURE > HEURE / NTP**, choisissez **Europe/Paris** pour la France métropolitaine. Les serveurs proposés sont `pool.ntp.org` et `time.google.com`. Sans NTP, utilisez **REGLER L'HEURE MANUELLEMENT** et saisissez la date et l'heure locales (chapitre 10).

Comparez SOC, tension, réseau et consommation au LCD de l'onduleur. La nuit, une production PV nulle est normale. Les compteurs OK doivent progresser pour les blocs utilisés ; le bloc VE peut rester sans lecture s'il est désactivé.

<!-- pagebreak -->

### Si l'adresse IP du logger change

La reconnexion du Wi-Fi de l'écran et la recherche d'une nouvelle IP du logger sont deux fonctions distinctes. La récupération automatique d'IP concerne uniquement **LSW / Solarman V5**, avec le **vrai numéro de série enregistré** dans l'écran. Une saisie non sauvegardée ne l'active pas. Un numéro vide, nul ou factice `123456789` la désactive.

1. Avec le Wi-Fi connecté, l'écran attend au moins **trois lectures échouées et 60 secondes sans réponse V5 valide**. Une erreur Modbus reçue du logger confirme sa présence et ne déclenche pas cette recherche.
2. Il recherche le même numéro sur le réseau local par UDP et TCP/V5. Les lectures Deye sont suspendues pendant le balayage ; l'interface et la tâche WB01 restent actives.
3. Si une **nouvelle IP pour ce même numéro** est trouvée, seule l'IP est enregistrée. Après vérification de la sauvegarde, l'écran redémarre automatiquement. Le numéro et les autres réglages restent conservés.
4. Si l'IP est inchangée, si le logger n'est pas trouvé ou si l'enregistrement échoue, il n'y a pas de redémarrage. Une nouvelle recherche peut partir cinq minutes après la fin de la précédente si la liaison reste perdue.

Une coupure Wi-Fi suspend cet automatisme. Un changement manuel d'IP ou de numéro pendant la recherche invalide son résultat ; une OTA ou un redémarrage déjà prévu reporte son application.

Le balayage suit le masque réseau, au maximum un /24. L'écran et le logger doivent pouvoir communiquer sans isolation entre clients. Pour **LSE et DL1000B**, contrôlez manuellement l'adresse, le port et l'unité si la liaison est perdue. Une réservation DHCP dans la box reste utile pour conserver une IP stable.

Exemple : le logger passe de `192.168.1.12` à `192.168.1.45`. En LSW/V5 avec son vrai numéro enregistré, l'écran peut retrouver cette nouvelle adresse, sauvegarder uniquement l'IP et redémarrer.

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

### Les sept rubriques principales

| Menu | Contenu |
|---|---|
| DEYE / SOLARMAN | Modèle, connexion/logger, production PV/GEN, registres perso. |
| TARIFS / HEURES CREUSES | Deux plages ou Tempo, affichage des couleurs. |
| VEHICULE ELECTRIQUE | Choix Aucune / Deye LoRa / Vetronic WB01, recharge et tarifs. |
| RELAIS | Règle de commande du relais intégré. |
| RESEAU / HEURE | Wi-Fi, synchronisation NTP et réglage manuel de l'heure. |
| AFFICHAGE | Thème, luminosité et veille horaire. |
| MISE A JOUR | Vérification GitHub, installation et QR code du guide. |

Faites défiler pour voir la suite. **RETOUR** quitte ; **ENREGISTRER / SAUVEGARDER / APPLIQUER** valide. La plupart des réglages redémarrent l'écran.

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
- **ON / OFF près du Wi-Fi** : état du relais intégré, visible uniquement si sa règle est activée.
- **~ devant les kWh GEN** : estimation calculée à partir de la puissance.
- **DEYE INJOIGNABLE** : aucune lecture Modbus réussie depuis environ deux minutes.

La disparition du message de liaison ne prouve pas que tous les compteurs sont valides : une seule réponse réussie peut suffire. Consultez le diagnostic si une valeur reste absente ou incohérente. Une valeur indisponible ne doit pas être interprétée comme zéro.

Près de la voiture, le soleil jaune indique un mode Solaire confirmé, même en attente de surplus. Avec WB01 : coche verte si le véhicule est branché, croix rouge s'il est débranché, tiret gris si la mesure manque ou est périmée. Deye LoRa ne fournit pas de statut de câble vérifié. Le plafond LoRa n'est pas affiché comme une puissance mesurée ; la puissance WB01 précédée de `~` est estimée à 230 V.

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

**Cumuler GEN MO + PV** ajoute GEN comme une source PV supplémentaire virtuelle, parfois appelée PV5. Sa puissance de production est prise en valeur positive après calibration, même si le coefficient en inverse le signe. Elle s'ajoute aux PV visibles dans le total instantané, le Web et l'historique ; ses kWh s'ajoutent au total du jour. Cela n'ajoute aucune entrée MPPT physique. Pour le total journalier, vérifiez que le compteur PV de votre onduleur n'inclut pas déjà cette énergie, afin d'éviter de la compter deux fois. Les commandes GEN sont masquées sur AI-W5.1.

### Afficher les kWh à côté de SMARTLOAD / GEN

Activez **Afficher les kWh GEN Daily** dans **PRODUCTION PV / GEN**, puis enregistrez. Désactivée par défaut, cette option est indépendante du cumul GEN MO + PV et fonctionne dans les deux modes.

Le bandeau peut afficher `SMARTLOAD : ON / 278 kWh` ou `GEN : 2650 W / 278 kWh`. Il lit le registre GEN Daily et son coefficient, même sans NTP. Une adresse non configurée ou une mesure absente donne `-- kWh` : le bandeau ne remplace pas cette valeur par l'estimation. Les températures restent sur la même ligne.

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

Choisissez le système utilisé, puis **ENREGISTRER** et laissez redémarrer :

| Choix | Compatibilité |
|---|---|
| Aucune | Suivi Deye sans client de recharge actif. |
| Deye LoRa | Uniquement **SUN-12K-SG02LP1-EU-AM2** ; pas l'AM3. |
| Vetronic WB01 | Passerelle ESP32 compatible, avec les 12 profils sélectionnables de l'écran. |

Le modèle Deye et la borne sont deux choix indépendants. Une seule recharge est active ; les pages du système inactif ne commandent pas l'équipement. Un choix LoRa incompatible est refusé. Après changement de modèle, vérifiez de nouveau la borne.

**Changer ou désactiver la borne dans l'écran n'arrête pas une charge en cours.** Utilisez d'abord sa commande d'arrêt si nécessaire. Le redémarrage n'envoie aucune consigne de charge.

Les options **Appliquer les tarifs**, **Réseau en HC seulement** et **Interdire HP rouge Tempo** limitent les charges prises en charge par l'automatisme. Elles ne prennent pas le contrôle des charges autonomes ou déjà présentes au démarrage. Une heure ou une couleur requise mais inconnue interdit le réseau pour la charge gérée.

<!-- pagebreak -->

### Recharge Deye LoRa

Après choix du système, ouvrez **VEHICULE ELECTRIQUE > RECHARGE DEYE LORA**. Choisissez **ARRET**, **SOLAIRE** ou **LIBRE**, réglez le plafond entre **1 400 et 7 400 W**, puis **APPLIQUER**. Les boutons +/-, le curseur et la saisie préparent ce plafond.

Le registre R490 est un **plafond**, pas la consommation mesurée de la borne. Contrôlez le **Mode lu** et le résultat de la commande ; la puissance native reste indisponible tant qu'elle n'est pas cartographiée de manière vérifiée.

Les écritures sont verrouillées par défaut. Vérifiez les registres sur le Web dans **VE / registres d'écriture**, puis confirmez explicitement leur déblocage. Sans profil adapté, liaison valide ou autorisation, les commandes restent bloquées. Lors de la première migration vers le firmware unifié, les écritures doivent être déverrouillées de nouveau.

Les restrictions tarifaires LoRa concernent uniquement une charge **Libre lancée et confirmée depuis l'écran tactile pendant le démarrage courant**. Si une règle interdit le réseau, l'écran peut basculer vers Solaire et rétablir Libre lorsque la règle l'autorise, s'il avait lui-même imposé cette pause.

Une commande locale vers un autre mode, une modification externe détectée ou un résultat incertain annulent cette reprise. Après redémarrage, aucune pause ancienne n'est récupérée. Cette fonction dépend des communications avec le logger et l'onduleur.

<!-- pagebreak -->

### Recharge VE TRONIC WB01

Choisissez **Vetronic WB01**, enregistrez, puis ouvrez la page de recharge, également accessible par la voiture. Dans **RESEAU**, saisissez l'IP de la passerelle ESP32 compatible. L'écran doit pouvoir la joindre sur le réseau local.

| Commande | Action |
|---|---|
| ARRET | Demander l'arrêt de la charge. |
| CHARGE IMMEDIATE | Appliquer le courant préparé avec les boutons +/-. |
| SOLAIRE | Confier la régulation solaire à la passerelle. |
| RENDRE LA MAIN A LA BORNE | Rendre la WB01 autonome et annuler la reprise tarifaire locale. |

Les boutons +/- ne commandent pas à eux seuls la charge. Le courant demandé respecte le plafond annoncé par la passerelle et la limite locale de **32 A**. La consigne, le courant mesuré et la confirmation WB01 sont distincts. `~` indique une puissance estimée par **courant mesuré × 230 V**, pas une mesure de puissance active.

La **protection SOC solaire** propose MIN/arrêt et MAX/reprise, séparés d'au moins cinq points. Modifiez les seuils puis touchez **ENREGISTRER SOC**. La passerelle doit exposer l'API correspondante ; sinon elle demande une mise à jour. Une liaison perdue ou une commande en cours désactive les contrôles qui exigent une mesure fraîche.

Les restrictions tarifaires WB01 concernent uniquement les charges manuelles lancées et confirmées depuis **l'écran ou son Web pendant le démarrage courant**. Arrêt, Solaire, Rendre la main, modification externe détectée ou résultat incertain annulent la reprise. Aucune pause ancienne n'est récupérée après redémarrage. Une réponse POST perdue n'entraîne pas de renvoi automatique.

**La passerelle garde son propre firmware et sa propre configuration Deye.** Changer le modèle sur l'écran ne change pas ses registres ni son calcul solaire. Contrôlez ses réglages avant le mode Solaire. Sa disponibilité avec les 12 profils de l'écran ne valide pas toutes les combinaisons sur matériel.

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

Les lectures et la règle continuent pendant la navigation et la veille. L'état du relais est affiché près du Wi-Fi uniquement si la règle est activée, sur le tableau de bord tactile et Web.

Le paramétrage logiciel ne définit pas la charge électrique admissible du relais. Pour le raccordement, respectez les caractéristiques du matériel et faites intervenir une personne qualifiée si vous commandez une charge secteur.

<!-- pagebreak -->

## 10. Affichage, veille et heure

### AFFICHAGE > THEME / LUMINOSITE

Choisissez le thème **Sombre** ou **Clair**, puis les niveaux de luminosité jour et nuit. Le firmware limite les valeurs brutes à **220-255** ; 255 correspond au maximum. Le curseur applique cette plage : elle n'est pas une échelle linéaire de luminosité perçue.

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

Sans NTP, touchez **REGLER L'HEURE MANUELLEMENT**, saisissez date et heure locales, puis appliquez sans redémarrer. NTP reprend la main dès qu'il reçoit une heure valide. Après une coupure d'alimentation, réglez de nouveau l'heure si NTP est indisponible.

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
| `/vetronic` | Commandes WB01 si ce système est sélectionné. |
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

L'export contient la configuration du modèle actif et les paramètres généraux inclus, mais pas tous les profils sauvegardés en une seule fois. Il inclut aussi le type de logger, son port, son unité, son numéro complet et le choix de borne `ev_backend`. Il ne contient ni le mot de passe Wi-Fi ni les identifiants Web. Exportez séparément chaque modèle si vous souhaitez en conserver une copie externe.

### Importer un fichier

1. Sélectionnez d'abord le modèle correspondant au fichier, puis laissez l'écran redémarrer.
2. Sur le Web, choisissez le fichier JSON dans la rubrique d'import.
3. Touchez **Importer et redémarrer**.
4. Après redémarrage, vérifiez modèle, logger et mesures.

Un fichier V3 portant l'identifiant d'un autre modèle est refusé. Un ancien export sans modèle est accepté uniquement sur le profil SG02 d'origine. Les mots de passe ne sont pas remplacés par l'import. Un ancien JSON avec VE activé sans `ev_backend` est refusé car la borne est ambiguë ; choisissez la borne dans la version unifiée puis créez un nouvel export. Un ancien export avec VE désactivé reste importable pour le même modèle. Les nouveaux exports identifient Aucune (`none`), Deye LoRa (`deye_lora`) ou WB01 (`vetronic_wb01`) ; la cohérence modèle/borne est contrôlée. Les anciens R536 sont corrigés lors de la sauvegarde sur les profils LP1 concernés ; une adresse 0 et les autres adresses personnalisées restent conservées.

<!-- pagebreak -->

### Mettre à jour directement depuis l'écran

1. Exportez votre configuration. Vérifiez le Wi-Fi avec accès Internet et l'heure.
2. Ouvrez **Configuration > MISE À JOUR > VÉRIFIER SUR GITHUB**.
3. Si une version stable plus récente et compatible est proposée, touchez **INSTALLER**, puis confirmez.
4. Attendez la progression, la validation du fichier et le redémarrage.
5. Vérifiez la version, le modèle, la borne et les mesures après démarrage.

**L'écran va scintiller pendant l'installation. C'est normal. Ne coupez pas l'alimentation jusqu'au redémarrage.** L'écran reste éveillé pendant le téléchargement. Aucun compte GitHub n'est nécessaire ; une vérification seule n'installe rien.

La connexion HTTPS vérifie le certificat ; la taille et le SHA-256 du fichier sont contrôlés avant activation. Le menu refuse les anciennes versions, les préversions, les images USB et les fichiers trop grands. Une mise à jour compatible conserve les réglages, mais l'historique en mémoire vive repart à zéro. En cas d'erreur, lisez le message et corrigez la connexion ou l'heure avant de réessayer.

Le QR code **DOC / GITHUB** ouvre le guide utilisateur sur le téléphone. Il ne lance pas l'installation. Son fond reste blanc dans les deux thèmes.

### Ancienne version sans le menu : OTA depuis le Web

1. Téléchargez **DEYE_V3_4.6.3_OTA.bin** dans la [release 4.6.3](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.6.3), puis ouvrez **Mise à jour OTA** sur le Web de l'écran.
2. Sélectionnez ce fichier applicatif `.bin`, puis **Installer la mise à jour**.
3. Conservez l'alimentation, attendez la réussite et le redémarrage, puis vérifiez version et mesures.
4. Lors de la première migration depuis V3/Vetronic V3, choisissez explicitement la borne et créez un nouvel export JSON.

N'envoyez ni ZIP, ni bootloader, ni partitions, ni `boot_app0.bin`, ni image USB fusionnée dans le formulaire OTA. Un partitionnement incompatible impose le pack USB complet. Le fichier est celui de **l'écran**, pas celui de la passerelle WB01.

Sur Vetronic V3 4.3.4, l'erreur « b is null » du bouton de vérification n'empêche pas ce formulaire : téléchargez manuellement le fichier OTA, puis envoyez-le. Installation détaillée.

<!-- pagebreak -->

## 13. Dépannage

| Symptôme | Vérifications utiles |
|---|---|
| Wi-Fi de la maison absent | Vérifier le 2,4 GHz, rapprocher l'écran, refaire SCAN. |
| Wi-Fi perdu après une coupure de la box | En 4.6.3, laisser la reconnexion automatique agir ; vérifier que le réseau et son mot de passe n'ont pas changé. |
| L'écran n'obtient pas d'IP | Vérifier le mot de passe et le DHCP de la box. |
| Impossible d'ouvrir le Web | Utiliser l'IP de l'écran, avec http:// ; vérifier que le téléphone est sur le même réseau, sans isolation Wi-Fi invité. |
| DEYE INJOIGNABLE | Vérifier type de logger, IP STA/LAN, numéro, port et unité. En LSW/V5, enregistrer le vrai numéro pour la récupération automatique d'IP. |
| Tout est visible dans le cloud, rien sur l'écran | Vérifier l'accès local du logger ; cloud et communication locale sont deux chemins distincts. |
| Certains blocs réussissent, d'autres échouent | Vérifier le modèle exact et les adresses personnalisées ; relever les exceptions dans Diagnostic. |
| Valeur dix fois trop grande/petite | Vérifier modèle, unité et coefficient ; comparer au LCD avant modification. |
| Batterie affichée dans le mauvais sens | Vérifier le coefficient Battery Power ; l'interface utilise charge positive, décharge négative. |
| PV3/PV4 absent | Vérifier le nombre d'entrées du profil et les interrupteurs Afficher PV. |
| GEN Daily faux ou absent | Vérifier GEN MO, R62/R536 selon la famille et le coefficient 0,1 ; vérifier la réponse du compteur. |
| Total journalier trop élevé | Vérifier que PV Daily n'inclut pas déjà GEN avant de les cumuler. |
| Veille ou tarifs inopérants | Vérifier heure NTP, fuseau et activation de la fonction. |
| Relais reste ON après une perte Wi-Fi | La dernière mesure reste utilisable cinq minutes ; consulter son âge. |
| Commande VE indisponible | Vérifier le choix de borne et sa liaison ; pour LoRa, profil AM2 et verrou d'écriture ; pour WB01, IP et état de la passerelle. |
| Import refusé | Vérifier le modèle et le choix de borne ; un ancien export avec VE actif sans ev_backend est ambigu. |

### Contrôle technique de la liaison

LSW utilise **Solarman V5** ; LSE et DL1000B utilisent **Modbus TCP**. Les ports proposés sont 8899, 8899 et 502 ; ils sont modifiables, ainsi que l'unité Modbus. Vérifiez le protocole réellement disponible sur votre logger. Un accès cloud fonctionnel ne confirme pas l'accès local.

Si le DHCP change l'IP du logger, la récupération automatique concerne LSW/V5 avec son vrai numéro enregistré. Pour TCP, vérifiez l'IP et le port. Réservez l'adresse dans la box si possible. En cas d'échecs intermittents, vérifiez aussi le signal et les autres logiciels qui interrogent le logger.

### Pour demander de l'aide

Indiquez la référence complète, le modèle sélectionné, la version de Deye Monitor, le modèle du logger, le symptôme, une photo du LCD et les compteurs/âges du diagnostic. Un export JSON aide à comprendre les réglages ; il peut contenir des identifiants d'appareil et le nom du réseau, à masquer avant publication publique. Ne communiquez pas vos mots de passe.

<!-- pagebreak -->

## 14. Repères, limites et sources

### Des fonctions V3 au firmware unifié

La V3 a apporté le choix du modèle, les registres et conversions associés, PV4, les réglages par modèle et le contrôle du modèle à l'import. La correction GEN Daily existe aussi en V2 4.2.1.

Le firmware unifié ajoute le choix de borne WB01/Deye LoRa, les loggers LSE/DL1000B, la recherche de logger, la récupération d'IP LSW, l'heure manuelle, GEN comme source PV supplémentaire, les kWh GEN dans le bandeau, la mise à jour GitHub depuis l'écran et la reconnexion Wi-Fi 4.6.3. Les fonctions communes V2/V3 restent disponibles.

Les retours du 1er octobre 2026 concernent neuf références : 8K-SG01LP1, 12K-SG05LP3, 25K-SG01HP3-AM2, 5K-SG05LP1-AM2-P, 6K-SG03LP1, 6K-SG05LP1, 12K-SG02LP1-AM3, 8K-SG05LP1 et 10K-SG04LP3. Ils ne précisent pas toutes les fonctions et versions testées. Voir les suffixes au chapitre 4.

AI-W5.1 P1 conserve les registres confirmés en 4.3.3 ; ce retour ne valide pas les autres révisions ni P3. SG06LP1 reste indisponible. Contrôlez SmartLoad au LCD. Les profils 20/25K HP3 ne valident pas les puissances supérieures.

La 4.6.3 a été compilée et testée par logiciel, avec une coupure Wi-Fi d'une heure simulée. Aucun écran n'a été flashé et aucune commande de recharge envoyée pour cette publication. Retour Wi-Fi après redémarrage réel du routeur, téléchargement complet depuis le menu et scénarios de charge restent à vérifier sur matériel. DL1000B reste testé sur simulateur. Validation 4.6.3.

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

- **[S1] SOLARMAN, [Network Settings](https://helpcenter.solarmanpv.com/portal/en/kb/articles/network-settings)** : Wi-Fi 2,4 GHz, accès AP, réservation DHCP.
- **[S2] SOLARMAN, [Fixed IP Settings](https://helpcenter.solarmanpv.com/portal/en/kb/articles/fixed-ip-settings)** : accès 10.10.100.254 et identifiants d'origine des modèles documentés.
- **[S3] Deye, [Guia de Configuração Solarman PRO](https://pt.deyeinverter.com/deyeinverter/2021/03/19/guiasolarmanpro.pdf)**, pages 34 et 45 ; **[Quick Guide DL1000B-ETH](https://www.deyeinverter.com/deyeinverter/2026/05/20/DeyeDataLoggerQuickGuideDL1000B-ETHV11.pdf)**, Device information : distinction AP/STA/LAN. Ces interfaces illustrent les adresses, sans prouver la compatibilité locale du logger.

Cette édition suit les sources et la validation du firmware unifié 4.6.3. Les protocoles de registres GEN sont conservés dans `docs/protocols/`.


