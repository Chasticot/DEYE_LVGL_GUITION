> Depuis la **4.8.7**, la lecture directe **Sossen Tuya 3.5** est disponible en essai : [configuration et limites](NOTES_4.8.7.md). Les profils Shelly restent disponibles.

# Sources solaires supplémentaires, historique et Sofar — 4.8.6

## Ouvrir la configuration

Connecter le téléphone et l’écran au même réseau Wi-Fi. En 4.8.5, ouvrir **Réglages > Installation > Sources solaires / PV5 à PV8** pour une passerelle ou un Sofar supplémentaire, ou **Réglages > Énergie > Historique / Carte SD**, puis scanner le QR code. Sur les versions 4.8.3/4.8.4, ces deux accès sont dans **Configuration > DEYE / SOLARMAN**. L’adresse affichée peut aussi être saisie dans un navigateur. Les réglages restent sur l’écran.

## Voir les productions PV5 et PV6 — à partir de la 4.8.4

Les quatre installations correspondent respectivement à **PV5, PV6, PV7 et PV8**. PV1 à PV4 restent réservés aux entrées de l’onduleur. Ces numéros ne changent pas lorsqu’une installation est désactivée. Chaque PV supplémentaire représente la production de toute sa passerelle.

Les puissances apparaissent à côté des MPPT sur l’écran principal. Jusqu’à six sources tiennent ensemble ; avec sept ou huit sources, les valeurs alternent toutes les quatre secondes. Toucher **PRODUCTION PV >** pour consulter toutes les sources dans une liste défilante : numéro, nom choisi, puissance et indication de comptage. Le tableau de bord Web propose également ce détail.

**Afficher séparément** rend la puissance visible sans l’ajouter au total. **Déjà mesurée par le port GEN** conserve le comptage existant sans ajouter la passerelle une deuxième fois. Un doublon est indiqué comme exclu. Une source indisponible affiche `-- W`, tandis que `0 W` signifie qu’une mesure nulle a bien été reçue. Le port GEN garde son propre nom.

## Ajouter une passerelle de micro-onduleurs

1. Ouvrir une des quatre installations et lui donner un nom.
2. Choisir le matériel et saisir l’adresse IP ou le nom local de la passerelle, sans `http://` ni chemin.
3. Garder **Afficher séparément**, enregistrer et attendre **Lecture OK**. **Relire** demande un nouveau test ; les lectures sont espacées pour ne pas surcharger les passerelles.
4. Vérifier que la puissance correspond à l’application du fabricant. Pour ajouter cette production au total, choisir **Ajouter au total solaire** et confirmer qu’elle n’est déjà comptée ni dans les PV ni dans le GEN de l’onduleur.
5. Si le GEN mesure déjà cette production, choisir **Déjà mesurée par le port GEN** : la lecture de la passerelle reste visible sans ajout supplémentaire.

Cocher la même journée locale uniquement si la passerelle et l’écran ont la même date et le même changement de journée. Sinon, les watts restent utilisables et les kWh du jour sont indiqués indisponibles. Au changement de date, une énergie d’hier n’est pas présentée comme celle d’aujourd’hui.

### APsystems

- ECU-B/R/Pro/C solaire : connexion locale TCP reconnue par le lecteur, intervalle minimal cinq minutes. Les ECU-C de stockage ne sont pas couvertes.
- ECU-3/4 : accès à la page locale de production ; l’horloge de l’écran et celle de la passerelle doivent être correctes. Les lignes périmées ou non reconnues provoquent un état indisponible. L’énergie quotidienne n’est pas déduite de ces tableaux.
- EZ1 : activer le mode local permanent dans l’application APsystems. La lecture utilise l’API locale du matériel ; aucun réglage de puissance n’est envoyé.

La liste vise les protocoles reconnus, et ne constitue pas une certification de toutes les variantes matérielles ou versions de firmware.

### Enphase

Essayer d’abord la lecture locale sans jeton sur les anciennes passerelles. Si un accès refusé ou une redirection HTTPS est signalé, obtenir un jeton depuis le [portail Enphase](https://entrez.enphaseenergy.com/) et le saisir dans **Connexion et options**. L’écran ne demande pas le mot de passe du compte cloud.

Après le premier essai HTTPS, vérifier que l’adresse est celle de la passerelle, puis confirmer son empreinte de certificat. Le jeton est transmis uniquement après correspondance de cette empreinte. Un changement de certificat bloque la lecture et demande une nouvelle vérification. Un jeton expiré doit être renouvelé.

Le champ jeton reste vide après enregistrement ; cela conserve le jeton existant. Utiliser **Effacer le jeton enregistré** pour le supprimer. Les réponses de configuration ne renvoient pas le jeton.

Le lecteur privilégie la production des micro-onduleurs ; un total issu uniquement des pinces de mesure n’est pas ajouté, car il peut aussi contenir la production de l’onduleur principal.

### Hoymiles, Deye micro et Sossen — 4.8.6

Le choix **Marque et connexion locale** regroupe maintenant les profils par marque. Hoymiles peut être lu via OpenDTU, AhoyDTU ou une DTU-Pro Modbus TCP compatible avec la carte `0x1000`. Deye micro dispose de profils V118 en Solarman V5 et Modbus TCP ; le profil vérifie le type micro-onduleur et ne relit pas l’hybride principal.

Pour Sossen, la lecture proposée passe par un **compteur Shelly dédié à sa production**. La lecture directe du micro-onduleur Sossen n’est pas intégrée. Le canal du compteur ne doit pas mesurer la consommation générale ; cette confirmation est obligatoire. Les watts sont lus sans inventer de kWh du jour à partir du compteur cumulé.

Les passerelles Hoymiles qui partagent un même micro-onduleur sont reconnues comme doublons dans les sources ajoutées. Les variantes Wi-Fi propriétaires sans l’API indiquée ne sont pas couvertes. [Profils, limites et mise en service 4.8.6](NOTES_4.8.6.md).

### Une passerelle devient indisponible

La dernière valeur trop ancienne n’est plus ajoutée comme une mesure actuelle. L’écran affiche un total partiel ; la courbe solaire présente une interruption pour ces mesures. Vérifier la connexion locale, le jeton éventuel et le message de l’installation. Ces lectures n’envoient aucune commande aux micro-onduleurs.

## Conserver l’historique sur microSD

Sans carte, l’historique des 24 dernières heures fonctionne en mémoire. Pour archiver après redémarrage, insérer une microSD FAT32 dans l’écran, avec l’heure réglée. Une carte insérée est recherchée périodiquement ; la reconnaissance peut prendre environ une minute. L’état de la carte est indiqué dans la page.

L’enregistrement automatique est activé par défaut, avec 90 jours de conservation. Le formulaire permet de désactiver l’archivage ou de choisir 7, 30, 90 jours ou un an. Le nouveau choix s’applique à la conservation des fichiers de mesure : sauvegarder les CSV utiles avant de réduire la durée.

Les fichiers sont rangés dans `deye-history`, un CSV par journée locale. Sélectionner une journée pour consulter sa courbe ou télécharger son CSV. Le CSV contient l’heure UTC en secondes, la puissance solaire, le réseau, la consommation, la batterie, le SOC et l’indication de total solaire complet. Les heures de la courbe Web suivent le fuseau du navigateur.

La carte n’est jamais formatée et les autres dossiers ne sont pas nettoyés. Une carte absente ou en erreur laisse l’historique en mémoire actif. Une mesure non archivée ne peut pas être recréée après coup. Les mesures sans heure correcte ne sont pas écrites sur la carte. Éviter de retirer la carte pendant un enregistrement.

## Ajouter un Sofar 3000TL-G3 au Deye — 4.8.5

Le 3000TL-G3 est suivi comme une source supplémentaire PV5 à PV8. Conserver **Deye** comme onduleur principal. Ouvrir **Installation > Sources PV5 à PV8** sur le Web, choisir une installation libre, puis **Sofar 3000TL-G3 + LSW3**. Renseigner l’IP et le numéro numérique du logger LSW3 ; l’adresse Modbus vaut 1 par défaut.

Enregistrer d’abord en **Afficher séparément**. Après une lecture réussie, comparer la puissance AC et l’énergie du jour au LCD du Sofar. Choisir ensuite **Ajouter au total solaire** avec la confirmation d’absence de double comptage, ou **Déjà mesurée par le port GEN** si GEN mesure déjà cette production.

Ce profil d’essai vérifie la famille TL A3. Il ne couvre pas le 3000TLM-G3, les HYD ou les KTLX. Il ne transmet aucune commande à l’onduleur. Les kWh demandent la même journée locale confirmée. La validation sur un vrai Sofar reste à faire. [Procédure complète 4.8.5](NOTES_4.8.5.md#essayer-un-sofar-3000tl-g3-en-complément-du-deye).

## Configurer un Sofar HYD comme onduleur principal

La prise en charge est expérimentale et désactivée par défaut. Identifier d’abord la référence exacte sur l’étiquette de l’onduleur. Sans cette information, conserver le réglage actuel et ne pas choisir une famille au hasard.

Depuis la page de configuration locale, ouvrir **Marque / famille de l’onduleur**. Configurer d’abord le logger LSW3 avec son IP et son véritable numéro, puis choisir une famille :

- **HYD 3–6K ES** ;
- **HYD G3 : deux MPPT et batterie 1 uniquement**.

Confirmer la famille et appliquer ; l’écran redémarre. Comparer les mesures au LCD, en particulier réseau positif pour un import et batterie positive pour une charge. Les cases d’inversion permettent d’adapter ces deux sens après vérification. Les températures non disponibles restent affichées `--`.

Pour le suivi principal HYD, les autres familles, les MPPT supplémentaires, les batteries supplémentaires et les commandes Sofar ne sont pas pris en charge. Les commandes Deye, le TOU, la recharge et les automatismes de relais sont désactivés lorsque Sofar est choisi. Revenir à **Deye** restaure l’utilisation de son profil conservé.

## Heure et Time of Use

Paris reste **UTC+01:00 + Auto Europe**. Pour les Antilles françaises, choisir **UTC−04:00 + heure d’été désactivée**. La règle d’été doit correspondre à la région.

Le TOU conserve les horaires de l’onduleur. Changer le fuseau de l’écran ne modifie pas l’horloge ni les plages TOU de l’onduleur. Sur Deye, la préparation d’un brouillon ne transmet rien ; l’application demande une confirmation. Vérifier les premières lectures et modifications au LCD.

## Réglages et mise à jour

Les nouvelles options sont enregistrées sur l’écran et conservées lors d’une OTA. L’export JSON historique des réglages ne contient pas les nouvelles configurations de passerelles, leurs jetons, le choix Sofar ni la conservation SD : les noter pour un remplacement d’écran. Les archives sont sur la carte ; le tampon des 24 heures est en mémoire volatile.
