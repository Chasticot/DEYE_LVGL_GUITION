# Deye Monitor — questions fréquentes

**Édition du 26 octobre 2026.** Repères pour le firmware public 4.7.2 et les écrans du projet. [Guide pas à pas](MISE_A_JOUR_ECRAN_GITHUB.md) · [Mémo de démarrage](RECAP_UTILISATEURS_V3.md) · [Dernière version stable](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest).

Les réponses distinguent les fonctions disponibles, les retours de fonctionnement et les matériels qui restent à tester. Un essai réussi sur un modèle ne valide pas toutes ses révisions ni toutes les fonctions.

**Besoin de voir les boutons ?** Le [guide illustré](MISE_A_JOUR_ECRAN_GITHUB.md#commencer-en-images) accompagne les manipulations avec des captures. Le [mémo illustré](RECAP_UTILISATEURS_V3.md) reprend le parcours de démarrage.

## Trouver une réponse

| Question | Réponse |
| --- | --- |
| Faut-il Home Assistant ou un abonnement ? | [Principe](#principe) |
| Mon onduleur ou mon logger est-il compatible ? | [Compatibilité](#compatibilite) |
| Comment démarrer à réception ? | [Démarrage](#demarrage) |
| USB, batterie, powerbank ou 230 V ? | [Alimentation](#alimentation) |
| Peut-on encastrer l’écran ? | [Fixation et boîtier](#encastrement) |
| Quelle IP faut-il renseigner ? | [Les deux adresses](#adresses-ip) |
| Peut-il fonctionner sans box/Internet ? | [Liaison directe](#sans-box) |
| Que faire si « DEYE INJOIGNABLE » apparaît ? | [Diagnostic local](#deye-injoignable) |
| Pourquoi la liaison coupe ou le cloud perd des données ? | [Stabilité et fréquence](#liaison-instable) |
| Pourquoi une valeur ou un total est faux ? | [Mesures et GEN](#mesures-et-production) |
| Comment mettre à jour ou récupérer un écran ? | [OTA et USB](#mise-a-jour) |
| Qu’envoyer pour demander de l’aide ? | [Informations utiles](#demande-aide) |

<a id="principe"></a>
## Principe

### Faut-il Home Assistant, Jeedom ou un ordinateur allumé ?

Non. L’écran interroge localement le logger de l’onduleur et affiche ses mesures. Un téléphone/PC est utile pour la page Web, les sauvegardes et le dépannage, mais il n’a pas à rester allumé. Aucun identifiant de compte DeyeCloud/Solarman n’est à saisir dans l’écran pour cette lecture locale.

### Est-ce du temps réel ?

Les données s’actualisent périodiquement. Un rythme d’environ **3 à 5 secondes** a été constaté sur une liaison Ethernet testée ; ce n’est pas une cadence garantie pour tous les loggers et toutes les mesures. Le délai varie avec la qualité réseau, les blocs lus et les réglages. Le cloud a sa propre fréquence et peut afficher un instant différent.

### L’écran remplace-t-il l’application Deye ou commande-t-il tout l’onduleur ?

L’application reste utilisable avec un logger correctement configuré. Le suivi courant lit les mesures. Choisir un modèle ou le mode d’affichage GEN dans l’écran ne reprogramme pas le mode de l’onduleur. Des commandes optionnelles existent pour certains équipements de recharge et pour le relais de l’écran ; elles ne sont pas des commandes universelles de l’installation.

<a id="compatibilite"></a>
## Compatibilité et fonctions

### Suffit-il de connaître la puissance, par exemple « DEYE 8 kW » ?

Non. Relever la référence complète sur la plaque et ses suffixes, par exemple LP1/LP3, AM2/AM3, SM2/BM3 et -P. Il faut aussi identifier le logger. Ne pas choisir un triphasé à la place d’un monophasé ou un modèle haute tension sur sa seule puissance.

### LSW3, LSW5 et LSE3 : quel choix dans le menu ?

LSW3/LSW5 utilisent le choix **LSW — Solarman V5**. Pour un LSE compatible, choisir **LSE — Ethernet Modbus TCP**. Des retours existent avec les familles LSW et un LSE3 a été testé. En Ethernet, le logger peut être câblé à la box et l’écran rester en Wi-Fi : l’essentiel est qu’ils se joignent sur le réseau local.

### Le DL1000B-WIFI USB est-il compatible ?

**À valider sur le matériel réel avant de confirmer une installation.** Le logiciel propose un mode DL1000B et une recherche Modbus TCP, mais cela ne prouve pas que le firmware du logger expose le service local ni que toutes les lectures fonctionnent. Relever sa référence, sa version et vérifier l’activation Modbus TCP dans son interface si elle existe. Le port proposé est 502, à confirmer selon sa configuration. Ne pas annoncer « compatible » à partir du menu, de la seule prise USB ou d’un test sur simulateur.

### Et les micro-onduleurs seuls, APsystems, Shelly, Marstek, Izipower, WKS ou Victron ?

Ils ne sont pas automatiquement pris en charge. Le projet lit les données des **hybrides DEYE et loggers prévus**. Des micro-onduleurs branchés sur GEN du DEYE sont un autre cas : le DEYE peut en fournir les mesures. Des sources externes ou batteries autonomes ne deviennent pas lisibles simplement parce qu’elles figurent dans une application. Une intégration envisageable n’est pas une fonction déjà disponible ni un développement promis.

### Une borne V2C apparaîtra-t-elle séparément ?

Aucune intégration V2C dédiée n’est validée ici. Sa consommation peut être incluse dans une mesure globale du DEYE selon le câblage, sans détail propre à la borne. Le fait qu’une borne échange avec le DEYE ne signifie pas que l’écran sait lire et piloter cette borne. Les fonctions Deye LoRa et WB01 du manuel concernent leurs propres matériels et conditions.

### Mon modèle exact manque dans la liste, puis-je prendre un modèle voisin ?

Vérifier d’abord la version de l’écran et le catalogue de la dernière mise à jour. Les anciens conseils de dépannage sur un profil voisin ne sont pas une règle générale. Faire confirmer le profil adapté ; une puissance proche ne suffit pas à établir la correspondance des registres. Les profils expérimentaux et fonctions marqués « à confirmer » doivent être comparés au LCD de l’onduleur.

<a id="demarrage"></a>
## Démarrage

### Que faire quand je reçois un écran déjà préparé ?

1. L’alimenter avec la solution prévue pour son matériel.
2. Dans **RÉSEAU / HEURE > WIFI / RÉSEAU**, scanner, choisir le Wi-Fi 2,4 GHz et sauvegarder son mot de passe.
3. Dans **DEYE / SOLARMAN > MON ONDULEUR**, choisir la puissance puis la référence exacte ; appliquer et laisser redémarrer.
4. Dans **CONNEXION / LOGGER**, sélectionner le type, entrer le vrai numéro du logger, rechercher son IP puis sauvegarder le bon résultat.
5. Attendre les lectures et comparer les valeurs au LCD du DEYE. Vérifier la date et l’heure.

Un écran déjà préparé ne nécessite pas une première compilation ni une réinstallation USB systématique. [Étapes détaillées](MISE_A_JOUR_ECRAN_GITHUB.md#première-utilisation--connecter-lécran).

### Le clavier est petit : puis-je utiliser mon téléphone ?

Oui. Une fois l’écran connecté au réseau, relever **son IP** dans le menu Wi-Fi puis l’ouvrir dans le navigateur d’un téléphone/PC du même réseau. La page Web permet de saisir les réglages plus confortablement. Chaque formulaire doit être sauvegardé ; laisser finir le redémarrage annoncé.

<a id="alimentation"></a>
## Alimentation

### Y a-t-il une batterie ? Combien de temps tient une powerbank ?

Il n’y a pas de batterie intégrée. Une alimentation USB **5 V** ou une powerbank compatible peut convenir. Aucune durée d’autonomie mesurée n’est annoncée. La capacité de la powerbank, son comportement à faible charge, le câble, la luminosité et le Wi-Fi influent sur le résultat. L’écran continue à fonctionner quand le rétroéclairage est éteint.

### Toutes les versions prennent-elles le 230 V ?

Non. Seule une variante dotée de l’alimentation secteur prévue par le fabricant peut recevoir le 230 V à son entrée dédiée. La version uniquement USB ne le peut pas. Vérifier la variante et son marquage. Sur les modèles concernés, l’alimentation intégrée évite un transformateur externe, mais les borniers arrière doivent être protégés. Faire réaliser le raccordement par une personne compétente.

### À quoi sert L1 ? Est-ce le SmartLoad du DEYE ?

Sur la variante à un relais discutée dans le projet, la borne **L1** est liée au relais de l’écran. Ce n’est pas une entrée d’alimentation à deviner ni le relais SmartLoad de l’onduleur. Le logiciel du relais intégré vise la variante prévue à un relais ; il ne pilote pas automatiquement trois relais parce qu’un autre écran en comporte trois. Pour tout raccordement, suivre le schéma exact du fabricant plutôt qu’une photo d’une autre version.

<a id="encastrement"></a>
## Fixation et boîtier

### Une boîte ronde standard convient-elle ? Avez-vous un support universel ?

Les boîtes rondes courantes ne conviennent pas nécessairement au dos de ces écrans. Vérifier les cotes de **la variante exacte**, la profondeur, le passage des fils, les fixations et la protection des bornes. Une boîte carrée adaptée ou un support peut être nécessaire. Aucune dimension universelle ni modèle d’impression 3D validé n’est annoncé ici. Le capot peut avoir des clips fragiles : ne pas forcer, appareil débranché.

### Puis-je le mettre dans une boîte étanche ?

Le comportement thermique de ce montage n’a pas été caractérisé. Ne pas considérer une boîte fermée/étanche comme validée parce que l’écran fonctionne à l’air libre. Vérifier les prescriptions du fabricant, le volume disponible et la dissipation avant installation ; demander un avis pour le montage concerné. Il n’existe pas ici de température de fonctionnement mesurée permettant de garantir ce montage.

<a id="adresses-ip"></a>
## Les deux adresses IP

![IP du logger pour les mesures, IP de l’écran pour la page Web ; exemples fictifs.](images/demarrage/reseau-maison.svg)

Utiliser les vraies adresses de votre installation. **IP du logger dans les réglages ; IP de l’écran dans le navigateur.**

### Faut-il entrer l’IP de l’écran ou celle du logger ?

Dans **CONNEXION / LOGGER**, entrer **l’IP du logger**. Dans le navigateur du téléphone/PC, ouvrir **l’IP de l’écran** pour son Web, son diagnostic ou son OTA. Le numéro de série demandé dans la connexion est celui du **logger**, pas celui de l’onduleur.

### Puis-je toujours utiliser 10.10.100.254 ?

Non. C’est l’adresse AP habituelle de certains loggers LSW. Elle sert dans leur réseau Wi-Fi direct. Quand écran et logger sont reliés à la box, utiliser l’IP **STA/LAN** du logger sur ce réseau. Ne pas copier l’adresse d’exemple d’un autre utilisateur. Les autres familles de logger peuvent avoir une autre procédure.

<a id="sans-box"></a>
## Sans box et sans Internet

![L’écran et le téléphone rejoignent le Wi-Fi AP d’un logger LSW compatible.](images/demarrage/reseau-sans-box.svg)

Le téléphone et l’écran rejoignent le même AP. Pour ouvrir l’écran, relever son IP : ce n’est pas automatiquement 10.10.100.254.

### Puis-je connecter l’écran directement au Wi-Fi AP du logger ?

Oui, ce mode a été utilisé avec des LSW compatibles. Vérifier d’abord depuis un téléphone l’accès au réseau AP et son mot de passe, puis configurer l’écran sur ce réseau, en mode LSW, avec l’adresse AP vérifiée du logger. Le téléphone peut signaler « sans Internet » tout en permettant les échanges locaux.

Régler l’heure manuellement sans NTP. Pour ouvrir le Web de l’écran, connecter le téléphone au même AP et utiliser l’IP propre à l’écran. NTP, Tempo et téléchargement GitHub demandent Internet. Un fichier OTA téléchargé auparavant peut être envoyé sur le réseau local si l’écran reste joignable. [Procédure directe complète](MISE_A_JOUR_ECRAN_GITHUB.md#connexion-directe-au-logger-sans-box).

### Faut-il un autre firmware si je passe plus tard par ma box ?

Non : c’est le même firmware. Changer la connexion Wi-Fi, s’assurer que le logger est lui aussi joignable sur le réseau choisi et vérifier son IP. Les données locales ne dépendent pas de l’ouverture d’un compte cloud.

<a id="deye-injoignable"></a>
## « DEYE INJOIGNABLE »

### Le Wi-Fi de l’écran est connecté, pourquoi n’y a-t-il aucune mesure ?

Être connecté au Wi-Fi ne prouve pas que le logger répond. Vérifier dans cet ordre :

1. Bon réseau local pour l’écran et le logger, sans isolation invité ni second réseau derrière un routeur/répéteur.
2. Bon choix **LSW / LSE / DL1000B**, vrai numéro du logger, IP et unité de communication.
3. Bon port : valeurs proposées **8899 / 8899 / 502**, à adapter à la configuration réelle, sans les permuter au hasard.
4. Modèle exact du DEYE ; lectures/diagnostic pour distinguer aucune réponse d’une lecture partielle.
5. Qualité du Wi-Fi et éventuels autres logiciels qui interrogent le même logger.

Sur un DL1000B, vérifier aussi l’accès Modbus TCP local. Le cloud peut fonctionner sans que cet accès soit actif.

### Le scan trouve une IP « numéro non vérifié ». Puis-je l’enregistrer ?

Une réponse Modbus ne prouve pas l’identité du logger. Dans **RESULTATS**, identifier explicitement le bon équipement avant sauvegarde. Une recherche peut prendre plusieurs minutes et suspendre les mesures ; attendre sa fin. Ne pas choisir la première adresse simplement parce qu’elle répond.

### Après une coupure, l’IP du logger a changé. Est-ce normal ?

Le DHCP de la box peut attribuer une autre adresse. Préférer une réservation DHCP pour la stabiliser. Avec **LSW/V5**, le vrai numéro du logger doit être enregistré pour permettre sa récupération automatique après perte durable de liaison. Cette récupération d’IP est distincte de la reconnexion Wi-Fi et ne s’applique pas automatiquement aux modes LSE/DL1000B TCP. Pour ces derniers, vérifier IP et port.

<a id="liaison-instable"></a>
## Liaison instable et application cloud

### Faut-il le même nom Wi-Fi et le même mot de passe partout ?

Pas obligatoirement. Un point d’accès avec un autre nom/mot de passe peut convenir s’il dessert **le même réseau local joignable**. En revanche, un routeur secondaire, le NAT ou l’isolation des clients peut bloquer les lectures. Faire le premier essai avec écran et logger proches du même point d’accès, puis vérifier la configuration du répéteur/mesh.

### Ça marche près de l’onduleur, mais pas à l’étage. Que faire ?

Tester d’abord la portée à l’endroit voulu. Une réception faible peut donner des valeurs puis des coupures. Déplacer le point d’accès, réduire les obstacles ou utiliser une liaison réseau adaptée. Ne pas déclarer le firmware défaillant à partir de la seule portée. Si une antenne déportée est envisagée, vérifier le connecteur exact : sur le logger examiné, c’était **SMA, pas RP-SMA** ; cela ne s’applique pas à tous les modèles.

### Le mode direct au logger coupe après un moment. Quelle version utiliser ?

La **4.7.2** renforce la récupération Wi-Fi et la détection d’un état « connecté » périmé, sans demander Internet pour se reconnecter. Des retours signalent plusieurs heures de fonctionnement sans problème après installation. Cela ne remplace pas un essai de coupure et de retour de l’AP sur chaque installation. Vérifier aussi la portée et le comportement propre du logger. Le réseau de la maison reste préférable quand sa couverture permet de joindre les deux appareils.

### L’application Deye perd certaines données quand l’écran est actif. Est-ce lié ?

Cela peut être lié à la fréquence des interrogations ou aux autres lecteurs du logger, sans être une cause universelle. Un cas a conduit à essayer un **Block interval** plus long dans les registres personnalisés : de 3 000 ms vers **5 000 ou 7 000 ms**, puis à attendre **5 à 10 minutes** pour observer le cloud. Noter le réglage initial et ne changer que ce délai pour le test. L’actualisation de l’écran devient plus lente. Vérifier aussi signal et autres clients ; ce réglage n’est pas une solution garantie à toute perte de données cloud.

<a id="mesures-et-production"></a>
## Mesures et production

### Une valeur est dix fois trop grande ou trop petite : quel coefficient mettre ?

Vérifier d’abord référence du modèle, version et unité (**W/kW**, **Wh/kWh**). Comparer la même mesure au LCD du DEYE au même instant. Selon la cartographie et le firmware du logger, une calibration peut être nécessaire ; il n’existe pas un coefficient unique pour tous les modèles. Exporter les réglages, modifier une seule valeur à la fois, sauvegarder et comparer. Ne pas recopier les registres d’un LP1 sur un LP3/HP3.

### Pourquoi la batterie est-elle positive ou négative ?

L’interface utilise **charge positive, décharge négative**. Vérifier l’état réel et le signe configuré avant de changer un coefficient. Une inversion de signe ne doit pas servir à cacher un mauvais profil ou une mesure ancienne.

### Les micro-onduleurs sur GEN peuvent-ils être ajoutés à PV ?

Oui, si le DEYE remonte ces données et que le profil le permet. Choisir **Production solaire supplémentaire** (ancien libellé **GEN MO**) dans **PRODUCTION SOLAIRE / GEN**, puis activer le cumul GEN + PV et sauvegarder. Le mode **Groupe électrogène** ne doit pas être ajouté au solaire. Ne pas utiliser une ligne PV libre comme contournement quand l’option GEN prévue existe.

### Pourquoi le total instantané est-il bon, mais pas les kWh du jour ?

La puissance instantanée et l’énergie journalière sont distinctes. Vérifier le compteur GEN pris en charge, sa conversion et l’heure. Le cumul peut doubler GEN si le compteur PV Daily du DEYE l’inclut déjà. **~** signale une estimation de l’écran ; **--** une donnée indisponible, pas nécessairement zéro. Activer les kWh GEN dans le bandeau et cumuler GEN dans le total sont deux options différentes.

### Les paramètres GEN affichent-ils ou commandent-ils le SmartLoad ?

Le choix du mode dans l’écran adapte l’affichage à l’usage du port. Il ne configure pas le port du DEYE à votre place. Le statut ON/OFF, la puissance GEN et le relais propre à l’écran sont trois informations différentes. Un état absent ou marqué « à confirmer » n’est pas un OFF certain. Ne pas écrire dans des registres inconnus pour essayer une fonction, notamment en changeant de famille LP/HP.

<a id="mise-a-jour"></a>
## Mises à jour et récupération

### Où est le fichier à installer ? Faut-il payer ou avoir un compte GitHub ?

La dernière release publique propose le fichier **_OTA.bin** de l’écran et son empreinte. Aucun compte GitHub n’est nécessaire pour utiliser le menu de mise à jour. Les archives **Source code** ne sont pas le firmware à installer. Ne pas confondre ce fichier avec celui d’une passerelle WB01 ou d’un logger.

### Quelle est la différence entre OTA et USB ?

**OTA** : envoyer le fichier applicatif .bin depuis le menu de l’écran ou sa page Web, sur un écran déjà installé et compatible. **USB** : première installation ou récupération avec le pack complet adapté. Une image USB, un ZIP ou des fichiers de partitions ne doivent pas être envoyés dans le formulaire OTA. [Procédures détaillées](MISE_A_JOUR_ECRAN_GITHUB.md#mettre-lécran-à-jour).

### Je ne trouve pas la page Web. Quelle adresse ouvrir ?

Relever **l’IP de l’écran**, la saisir en **http://** dans la barre d’adresse du navigateur, depuis le même réseau. Ce n’est ni l’IP du logger ni le site DeyeCloud. En direct AP, rejoindre le réseau du logger avec le téléphone. Une page locale peut rester accessible sans Internet.

### La mise à jour efface-t-elle tout ? L’écran scintille, est-ce normal ?

Une OTA compatible conserve les réglages, mais exporter le JSON avant et vérifier après redémarrage. L’historique en mémoire vive repart à zéro. Le scintillement pendant l’installation est attendu : garder l’alimentation jusqu’au redémarrage. Une installation USB complète ou un effacement peut, lui, remplacer des réglages. L’option « nouvel écran » n’est pas un dépannage systématique.

### Pourquoi le QR code ne lance-t-il pas la mise à jour ?

Le QR **DOC / GITHUB** ouvre la documentation sur le téléphone. Il ne déclenche pas une installation. Utiliser **VÉRIFIER SUR GITHUB > INSTALLER** ou le formulaire OTA.

### Météo, prévision solaire, écran plus grand : est-ce déjà disponible ?

Une suggestion ou un développement en cours n’est pas une fonction livrée. Vérifier les notes de **la version stable publique installée**. Cette FAQ n’annonce pas ces fonctions dans la 4.7.2 et ne promet ni date ni compatibilité avec un autre format d’écran.

## Affichage et heure

### L’écran devient noir ou se rallume tout seul : panne ou veille ?

Vérifier la veille horaire, la durée de réveil, l’heure et le fuseau. Le premier toucher réveille sans appuyer sur le bouton situé dessous. La veille coupe le rétroéclairage, pas les lectures ni le Web. Un ancien problème de réveils/appuis fantômes a reçu un correctif : relever la version et mettre à jour avec la bonne procédure avant de conclure à une panne matérielle.

### Peut-il fonctionner sans mise à l’heure Internet ?

Les mesures locales restent possibles. Utiliser le réglage manuel de date/heure ; après une coupure d’alimentation, le refaire si NTP est absent. Une heure fausse peut perturber veille, tarifs et énergies journalières estimées. Pour la France métropolitaine, vérifier **Europe/Paris**.

<a id="demande-aide"></a>
## Demander de l’aide

Envoyer en une fois : modèle complet DEYE, modèle/firmware logger, version Deye Monitor, mode réseau (box/AP/Ethernet/mesh), message exact, changement récent et photos comparées écran/LCD. Si demandé, ajouter diagnostic et export JSON. Ne pas publier mots de passe, QR codes d’accès, numéros d’appareil ou coordonnées privées. [Liste détaillée](MISE_A_JOUR_ECRAN_GITHUB.md#demander-de-laide-sans-multiplier-les-échanges).

Les cas de compatibilité inconnue, de données incohérentes persistantes et de raccordement secteur doivent être examinés pour le matériel concerné. Une lecture trouvée dans un menu ne suffit pas à valider toute l’installation.
