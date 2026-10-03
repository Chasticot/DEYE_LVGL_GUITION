# Guide du firmware unifié 4.3.7

La version 4.3.7 réunit le suivi de plusieurs modèles Deye et les deux systèmes de recharge de la V3 et de Vetronic V3. Le même fichier s'installe sur l'écran GUITION ESP32-S3 480 × 480 du projet. Le modèle Deye et la borne se choisissent ensuite dans les réglages de l'écran ou du Web.

## Installer et démarrer

Télécharger le [pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/DEYE_V3_4.3.7_Installation_Windows.zip) ou le [binaire OTA](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.7/DEYE_V3_4.3.7_OTA.bin). Le pack contient les quatre binaires et l'outil nécessaire à une installation USB. L'OTA s'utilise uniquement sur un écran dont le partitionnement est déjà compatible. [Procédure Windows](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/INSTALLATION_WINDOWS.md).

Avant installation, exporter la configuration JSON depuis le Web de l'écran et noter les paramètres importants. Un effacement complet supprime les réglages ; une mise à jour compatible sans effacement permet leur migration.

Après le premier démarrage :

1. Dans **RÉSEAU / HEURE**, configurer le Wi-Fi et vérifier la synchronisation de l'heure.
2. Dans **DEYE / SOLARMAN > CONNEXION / LOGGER**, renseigner l'IP locale et le numéro de série du logger.
3. Dans **DEYE / SOLARMAN > MODÈLE DEYE**, choisir la référence de l'onduleur, enregistrer et laisser redémarrer.
4. Comparer les mesures du tableau de bord au LCD de l'onduleur. Contrôler notamment PV, réseau, consommation, batterie et SOC.
5. Dans **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, choisir la borne, enregistrer et laisser redémarrer.

Le menu de modèle reste disponible avec chacune des trois options de recharge. L'enregistrement des autres pages de réglages conserve le choix de borne.

## Modèle Deye

Le catalogue contient douze profils sélectionnables :

| Profils monophasés / ESS | Profils triphasés |
| --- | --- |
| SUN-12K-SG02LP1-EU-AM2 | SUN-12K-SG05LP3-EU-SM2 |
| AI-W5.1 ESS (P1) | SUN-25K-SG01HP3-EU-AM2 |
| SUN-8K-SG01LP1-EU | SUN-10K-SG04LP3-EU |
| SUN-6K-SG03LP1-EU | SUN-20K-SG01HP3-EU-AM2 |
| SUN-8K-SG05LP1-EU | |
| SUN-5K-SG05LP1-EU-AM2-P | |
| SUN-6K-SG05LP1 | |
| SUN-12K-SG02LP1-EU-AM3 | |

SUN-6K-SG06LP1 est visible dans la liste mais indisponible : sa cartographie complète reste à établir. Les fonctions PV supplémentaires, GEN ou SmartLoad dépendent du profil. Les réglages des registres et coefficients restent séparés par modèle. Voir [profils et registres](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/PROFILS_ET_REGISTRES.md).

Pour AI-W5.1 ESS P1, les adresses monophasées intégrées en 4.3.3 sont conservées. Le retour utilisateur qui les confirme ne couvre pas toutes les révisions ni la variante P3.

## Choisir la borne

| Choix | Effet |
| --- | --- |
| **Aucune** | Mesures Deye et fonctions communes ; aucun client de recharge actif. |
| **Deye LoRa** | Lectures et commandes des registres VE natifs, uniquement avec **SUN-12K-SG02LP1-EU-AM2**. |
| **Vetronic WB01** | Client HTTP de la passerelle ESP32 WB01, disponible avec tous les profils sélectionnables de l'écran. |

Le sélecteur reste accessible sur les autres profils ; un choix Deye LoRa incompatible est refusé à l'enregistrement. WB01 et Aucune restent disponibles.

Une seule recharge fonctionne à la fois. Le menu et le clic sur la voiture ouvrent la page correspondante. Les pages du système inactif n'envoient pas de commandes. En mode WB01, les lectures automatiques et écritures VE natives sont bloquées, même si une ancienne permission les autorisait. En mode LoRa ou Aucune, le client WB01 ne commande pas la passerelle.

Le modèle et la borne sont chargés au démarrage. Une modification dans le formulaire reste un brouillon jusqu'à **ENREGISTRER**, puis s'applique après redémarrage. Changer de modèle peut rendre LoRa incompatible : il reste alors inactif et l'écran demande un nouveau choix. Le choix mémorisé incompatible est conservé tant qu'un nouveau choix de borne n'est pas enregistré.

**Le redémarrage n'envoie aucune consigne de charge.** Désactiver ou changer la borne dans l'écran n'arrête pas une charge déjà lancée sur l'équipement. Utiliser sa commande d'arrêt auparavant si un arrêt est souhaité.

## Migration depuis V3 ou Vetronic V3

L'ancienne activation VE avait deux sens : Deye LoRa dans V3 et WB01 dans Vetronic V3. La 4.3.7 ne peut pas en déduire la borne réelle et laisse la recharge désactivée jusqu'au choix explicite.

1. Mettre à jour sans effacement si le partitionnement est compatible et si la conservation des réglages est souhaitée.
2. Vérifier le modèle Deye après démarrage ; le modèle mémorisé par V3 est conservé.
3. Ouvrir **ACTIVATION / TARIFS** si l'écran indique « Choisir la borne puis enregistrer ».
4. Sélectionner Aucune, Deye LoRa ou Vetronic WB01, enregistrer et laisser redémarrer.
5. Contrôler la page de recharge, les paramètres et sa liaison avant toute commande.
6. Créer un nouvel export JSON avec le choix de borne explicite.

Les réglages communs Wi-Fi, logger, heure, affichage, tarifs et authentification Web sont repris. Les registres et options conservent leurs espaces de sauvegarde V3 par modèle. Les adresses VE natives personnalisées sont reprises lors de la première migration, mais les écritures restent verrouillées jusqu'au déblocage explicite dans la nouvelle version.

Un ancien export sans `ev_backend` et avec VE activé est refusé, car il serait ambigu. Un export avec VE désactivé reste importable pour le même modèle. Les nouveaux exports portent `ev_backend` avec `none`, `deye_lora` ou `vetronic_wb01` ; la cohérence modèle/borne est contrôlée à l'import.

Les espaces NVS sont partagés avec les versions précédentes : certains changements de réglages restent visibles si l'on revient à un ancien firmware. Conserver la sauvegarde initiale avant tout retour de version.

## Deye LoRa

Cette option concerne les registres VE du **SG02 AM2**. Vérifier leur cartographie et les réglages personnalisés avant de déverrouiller les écritures depuis le Web.

La page distingue le plafond de charge, le mode lu et le résultat d'une commande. **Le registre R490 est un plafond, pas une mesure instantanée.** Le tableau de bord garde une puissance VE indisponible tant que sa mesure native n'est pas cartographiée de manière vérifiée.

Les tarifs optionnels concernent uniquement une charge Libre lancée et confirmée depuis l'écran tactile pendant le démarrage courant. Une charge autonome ou déjà présente au démarrage n'est pas reprise. Une commande locale vers un autre mode, une modification externe détectée ou un résultat incertain annulent la propriété tarifaire de l'écran. Après redémarrage, aucune pause ni reprise ancienne n'est récupérée.

## VE TRONIC WB01

Choisir **Vetronic WB01**, enregistrer puis ouvrir sa page de recharge. Dans **RÉSEAU**, renseigner l'adresse IP de la passerelle ESP32 compatible. L'écran doit pouvoir la joindre sur le réseau local.

| Commande | Fonction |
| --- | --- |
| **ARRÊT** | Demander l'arrêt de la charge. |
| **CHARGE IMMÉDIATE** | Demander une charge manuelle avec le courant préparé. |
| **SOLAIRE** | Confier la régulation solaire à la passerelle. |
| **RENDRE LA MAIN À LA BORNE** | Rendre la WB01 autonome et annuler toute reprise tarifaire locale. |

Les boutons +/− préparent le courant sans envoyer de commande. **CHARGE IMMÉDIATE** applique la demande. Le courant respecte le plafond annoncé par la passerelle et la limite locale de 32 A.

La consigne, le courant mesuré et la confirmation WB01 sont distincts. La puissance précédée de **`~`** correspond à une estimation `courant × 230 V`, pas à une mesure de puissance active. Une mesure indisponible ou périmée ne devient pas une puissance nulle fictive.

La **protection SOC solaire** expose des seuils MIN/arrêt et MAX/reprise séparés d'au moins cinq points. Modifier les seuils prépare un brouillon ; **ENREGISTRER SOC** les envoie à la passerelle lorsque son API prend cette fonction en charge. Une passerelle sans cette API demande une mise à jour. Une commande en cours ou une liaison perdue désactive les contrôles qui nécessitent une mesure fraîche.

La restriction tarifaire WB01, si activée, concerne les charges manuelles lancées et confirmées depuis l'écran ou son Web pendant le démarrage courant. Une pause peut reprendre quand les tarifs l'autorisent. Arrêt manuel, Solaire ou Rendre la main annulent cette reprise. Une réponse POST perdue ne déclenche pas de renvoi automatique.

**La passerelle garde son propre firmware et sa propre configuration Deye.** Le modèle choisi sur l'écran ne change pas ses registres, coefficients ou calculs solaires. Vérifier sa configuration pour l'onduleur utilisé avant de passer en solaire. Le firmware de passerelle n'est pas inclus dans cette release.

## GEN MO comme source PV supplémentaire

Dans **DEYE / SOLARMAN > PRODUCTION PV / GEN**, sélectionner **GEN MO** puis activer **Cumuler GEN MO + PV**, enregistrer et laisser redémarrer.

GEN agit alors comme un PV5 virtuel : sa puissance de production s'ajoute aux PV visibles dans le total instantané, le Web et l'historique. Son énergie du jour s'ajoute au compteur journalier PV. Par exemple, 2 000 W de PV + 750 W de GEN donnent 2 750 W ; 12,3 kWh PV + 4,5 kWh GEN donnent 16,8 kWh.

La puissance GEN MO est interprétée en valeur positive après calibration, même si le coefficient GEN inverse son signe. Le compteur journalier GEN configuré reste prioritaire ; avec une adresse GEN Daily à 0, l'estimation utilise cette même puissance positive. Un trou de communication n'est pas intégré et le compteur estimé change de jour avec l'heure locale.

SmartLoad, une option de cumul désactivée ou un profil sans GEN excluent GEN des deux totaux. GEN est une source virtuelle supplémentaire : aucune cinquième entrée MPPT physique n'est ajoutée. Le firmware distinct de la passerelle WB01 conserve son propre calcul solaire.

## Fonctions communes et Web

Les fonctions V3 sont conservées : tableau de bord, historique, PV/GEN, réglages des registres et coefficients, tarifs Tempo/heures creuses, relais de l'écran GPIO40, veille horaire, thèmes, luminosité, diagnostic et OTA. Les fonctions propres à un équipement suivent son profil.

Ouvrir l'adresse IP de l'écran dans un navigateur pour sa configuration Web. Les commandes utilisent l'authentification et la protection CSRF existantes. L'ouverture d'une page n'envoie pas de commande de charge. Après modification du modèle ou de la borne depuis le Web, enregistrer et laisser redémarrer avant de vérifier le nouveau choix.

Pour le détail des fonctions communes, le [guide V3 historique](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/GUIDE_UTILISATEUR_V3.md) reste utile ; ses indications de version et de simple activation VE ont été remplacées par le sélecteur décrit ici.

## Vérification des versions et migration depuis Vetronic V3

Le bouton de vérification accepte les anciennes versions suffixées, par exemple `4.3.4-vetronic-v3`. Une version inconnue ou une réponse GitHub incorrecte donne un message explicite. La vérification consulte les releases ; elle ne télécharge ni n'installe automatiquement le firmware.

Sur la 4.3.4 déjà installée, le bouton peut afficher « b is null » parce qu'il ne comprend pas le suffixe. Télécharger manuellement le fichier OTA 4.3.7 depuis cette release, puis l'envoyer avec le formulaire OTA de l'écran. Ce formulaire reste indépendant du bouton de vérification. Sauvegarder la configuration avant l'opération ; vérifier le modèle et choisir explicitement la borne après migration initiale vers le firmware unifié.

## Vérifier l'installation réelle

Les [tests logiciels](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/VALIDATION.md) valident les migrations, gardes de commande, horaires, interfaces et codecs avec des données simulées. Aucun écran ni équipement de recharge n'a été flashé ou commandé pour valider 4.3.7.

Sur l'installation, vérifier successivement les mesures au LCD, la persistance des deux choix après redémarrage, la liaison de la borne choisie, les commandes attendues et les restrictions tarifaires. Contrôler la réaction à une perte de liaison. Avec WB01, vérifier aussi courant, retour à la borne, solaire et SOC avec les paramètres propres à la passerelle.
