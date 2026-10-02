# Deye Monitor V3
## Les évolutions des versions de base à la V3

**Version 4.3.3 - 2 octobre 2026**

La V2 a enrichi le suivi de la production et ajouté des automatismes, le relais et la veille. La V3 reprend ces fonctions et simplifie la configuration de plusieurs modèles Deye avec un menu déroulant. Voici le récapitulatif des ajouts et des améliorations disponibles dans la V3 actuelle.

### V2 : des menus réorganisés

Les réglages sont regroupés en six rubriques : **Deye / Solarman**, **Tarifs / Heures creuses**, **Véhicule électrique**, **Relais**, **Réseau / Heure** et **Affichage**. Le choix SMARTLOAD / GEN MO rejoint le menu Production PV / GEN. Les nouveaux réglages sont aussi accessibles sur le Web et inclus dans les sauvegardes JSON.

### V2 : une production PV / GEN personnalisable

- **Afficher ou masquer PV1, PV2 et PV3** : chaque entrée peut être retirée des lignes, de la puissance totale affichée et de son historique. Cela ne coupe pas les panneaux et ne reprogramme pas l'onduleur.
- **Cumuler GEN MO avec les PV** : la production positive reçue sur le port GEN peut être ajoutée au total en mode GEN MO, notamment pour des micro-onduleurs. Si toutes les lignes PV sont masquées, GEN peut alimenter seul la tuile Production.
- **Lire GEN indépendamment des autres blocs** : une adresse GEN personnalisée éloignée n'oblige plus à agrandir les blocs habituels.
- **Suivre l'énergie GEN quotidienne**, soit par un compteur de l'onduleur, soit par estimation à partir des puissances reçues. Le signe **~** identifie l'estimation.
- **Choisir la source directement dans GEN Daily** : une adresse non nulle active la lecture du compteur ; une adresse vide ou 0 utilise l'estimation. Le coefficient en kWh est personnalisable.

L'estimation GEN est journalière, avec remise à zéro à minuit local et sauvegarde toutes les cinq minutes. Elle n'invente pas de production pendant les coupures ou les intervalles de mesure trop longs. L'heure doit être synchronisée ; un redémarrage peut perdre la dernière période non sauvegardée.

Le compteur PV journalier reste global : masquer une entrée ne soustrait pas ses kWh. Avant d'ajouter GEN, vérifiez que PV Daily ne le comprend pas déjà. Si un compteur nécessaire manque, le logiciel évite d'afficher un total journalier partiel comme s'il était complet.

### V2 : batterie et diagnostic plus cohérents

La puissance batterie dispose d'un coefficient personnalisable, avec **charge positive et décharge négative** par défaut. Les libellés, les historiques et les données Web suivent cette convention.

Le message **DEYE INJOIGNABLE** tient compte des lectures réussies de tous les blocs, y compris GEN, GEN Daily et relais, et revient après environ deux minutes sans réponse. La validité de chaque mesure reste contrôlée séparément.

<!-- pagebreak -->

## Les ajouts de la V2, conservés dans la V3

### Tarifs et automatisation de la recharge

- **Deux plages d'heures creuses personnalisées**, au format HH:MM, avec possibilité de traverser minuit. Début = fin désactive une plage.
- **Un mode tarifaire Tempo**, avec les HC 22:00-06:00 et le changement de jour tarifaire à 06:00, associé à l'affichage Tempo déjà présent.
- **Des restrictions de recharge configurables** : appliquer les tarifs, autoriser le réseau seulement en HC et interdire les heures pleines rouges Tempo.
- **Un passage automatique de Libre à Solaire** lorsqu'une règle interdit le réseau, avec vérification par relecture. Le retour Libre n'est automatique que si le logiciel avait lui-même imposé le changement pendant le démarrage en cours ; une action manuelle annule cette reprise.
- **Des boutons +/- pour le plafond de recharge**, en complément du curseur et de la saisie. La puissance indiquée est un plafond, pas la consommation mesurée de la borne.

L'heure ou la couleur Tempo manquante empêche d'autoriser le réseau lorsqu'elle est nécessaire à la règle. Les écritures VE restent soumises au verrou déjà présent. Dans la V3, cette fonction reste limitée au profil **SUN-12K-SG02LP1-EU-AM2**, avec une borne Deye LoRa compatible ; elle n'est pas activée sur l'AM3 ou sur tous les modèles du menu.

### Commande du relais intégré à l'écran

La V2 ajoute une règle **registre × coefficient**, avec choix de valeur signée ou non, comparateur **<, = ou >**, seuil et délai de commutation. Elle permet par exemple d'activer le relais au-dessus d'un niveau de batterie choisi.

L'état ON/OFF apparaît sur le tableau de bord ; l'âge de la mesure est visible dans le menu Relais. Le relais est OFF au démarrage. En cas de perte de communication, la dernière mesure reste utilisable cinq minutes, puis OFF est forcé. Le matériel prévu est l'**ESP32-4848S040C à un relais** ; ce relais est distinct du SmartLoad de l'onduleur.

### Veille horaire avec réveil tactile

La veille coupe le rétroéclairage pendant une plage choisie, sans arrêter les mesures, le Web, le relais ni les automatismes. Le réveil tactile est réglable de **1 à 3 600 secondes**, avec 60 secondes par défaut. Le premier toucher réveille sans déclencher le bouton situé dessous.

L'extinction attend la synchronisation de l'heure et laisse un délai de réveil. Cette veille complète le mode nuit et le réglage de luminosité déjà disponibles.

### Sauvegarde et fonctions déjà présentes dans la base

Les paramètres ajoutés en V2 sont persistants, configurables sur le Web et exportables/importables en JSON, avec contrôles des valeurs. Les anciens exports sans section V2 conservent ces nouveaux réglages.

**Le socle existant est conservé** : tableau de bord, accès Web local, historique 24 h, diagnostic, export/import, mise à jour OTA, protection Web, thèmes clair/sombre, luminosité jour/nuit et suivi du lever/coucher du soleil. Ces fonctions figuraient déjà dans la base locale comparée ; les nouveaux menus et automatismes viennent les compléter.

<!-- pagebreak -->

## Ce que la V3 ajoute à la V2

Choisissez votre modèle Deye dans le menu déroulant : le logiciel charge les registres, les blocs de lecture et les conversions associés.

- **Un choix direct du modèle**, sur l'écran et depuis le navigateur. Plus besoin de recopier les adresses de registres pour une référence déjà proposée.
- **Des réglages conservés pour chaque modèle** : production PV, GEN, registres personnalisés, tarifs, relais et veille. Revenir à un modèle retrouve sa configuration ; le Wi-Fi et le logger restent communs.
- **Un affichage adapté aux entrées PV** : PV3 apparaît sur les profils concernés et PV4 est disponible sur les profils 20/25K SG01HP3. PV4 participe au total affiché et à l'historique.
- **Des conversions adaptées aux familles LP1, LP3 et HP3**, avec une capacité accrue pour les puissances converties afin d'éviter leur plafonnement à 32 767 W.
- **GEN Daily corrigé** : R62 pour les profils monophasés LP1, R536 pour LP3/HP3, coefficient par défaut 0,1 kWh. Les anciens R536 des profils LP1 sont corrigés automatiquement. Cette correction existe aussi en V2 4.2.1.
- **Un import de configuration contrôlé par modèle**, pour éviter d'appliquer les réglages d'une autre famille.

### Les références ajoutées en 4.3.2

SUN-5K-SG05LP1-EU-AM2-P, SUN-6K-SG05LP1 et SUN-12K-SG02LP1-EU-AM3 rejoignent les références déjà proposées. Les doublons et abréviations des retours utilisateurs sont regroupés.

La liste comprend 13 entrées : 12 profils sélectionnables, dont AI-W5.1 P1, et SG06LP1 encore indisponible. Des utilisateurs rapportent un fonctionnement satisfaisant sur neuf références ; cela ne signifie pas que chaque fonction et chaque version de firmware ont été testées.

### Passer à la V3

Exportez la configuration actuelle, installez le binaire V3 compatible avec votre écran, puis ouvrez **Configuration > DEYE / SOLARMAN > MODELE DEYE**. Choisissez la référence exacte et enregistrez : l'écran redémarre. Contrôlez ensuite quelques mesures avec le LCD de l'onduleur.

Le mode estimation GEN déjà choisi avec l'adresse 0 est conservé. La recharge VE reste limitée au profil SUN-12K-SG02LP1-EU-AM2 ; elle n'est pas activée sur l'AM3. Le guide utilisateur détaille la mise en route et chaque menu.

Mise à jour 4.3.3 : AI-W5.1 P1 utilise désormais la cartographie monophasée validée par retour utilisateur le 2 octobre 2026. Les anciennes adresses sauvegardées de ce seul profil sont remplacées automatiquement ; les autres modèles sont conservés.
