# Deye Monitor 4.8.3

Cette mise à jour reprend les réglages UTC et le Time of Use de la 4.8.2, ajoute la lecture de passerelles de micro-onduleurs et la gestion des historiques sur microSD. Paris reste le réglage par défaut. Pour les Antilles françaises : UTC−04:00, heure d’été désactivée.

## Historique Web et microSD

Le traitement de l’historique utilise désormais un tampon mémoire alloué et une réponse envoyée progressivement. Cela supprime le grand tableau placé sur la pile lors de l’ouverture de la page, cause probable du redémarrage signalé en 4.8.2. Le cas sans microSD est pris en charge : les 24 dernières heures restent disponibles en mémoire, avec une mesure toutes les cinq minutes. Cette mémoire est perdue au redémarrage.

Une microSD FAT32 permet d’archiver les mesures par journée, de sélectionner une date et de télécharger le CSV. Conservation configurable : 7, 30, 90 jours ou un an ; 90 jours par défaut. L’écran ne formate jamais la carte. Il supprime uniquement les anciens fichiers de mesure valides de son dossier `deye-history`. Une journée de 25 heures est prise en charge. La date et l’heure doivent être réglées pour archiver.

## Micro-onduleurs

Jusqu’à quatre passerelles configurables depuis **DEYE / SOLARMAN > MICRO-ONDULEURS**, avec un QR code vers la page locale de l’écran.

- APsystems ECU-B/R/Pro et ECU-C solaire utilisant le protocole local reconnu ; ECU-3 et ECU-4 via leur page locale ; EZ1 avec API locale activée en permanence.
- Enphase Envoy / IQ Gateway avec les API de production prises en charge. Les versions récentes peuvent nécessiter un jeton Enphase et une confirmation du certificat de la passerelle.
- Trois choix : afficher séparément, ajouter au total solaire, ou déjà mesurée par le port GEN.

Commencer par une lecture séparée et attendre une réponse valide avant d’activer l’ajout au total. Une confirmation rappelle de ne pas compter deux fois une production déjà incluse dans les PV ou le GEN. Les doublons identifiés sont exclus. Les valeurs périmées sont signalées ; un total incomplet est indiqué comme partiel. Les kWh du jour ne sont ajoutés que si la passerelle utilise la même journée locale.

Les micro-onduleurs modifient les affichages et l’historique. Leur puissance n’est pas utilisée pour les commandes de recharge ou de relais. La compatibilité dépend de la passerelle et de son firmware ; toutes les gammes et versions ne peuvent pas être garanties sans essais réels. Les ECU-C de stockage ne sont pas couvertes.

## Sofar : prise en charge expérimentale

Deux familles explicitement sélectionnables sont proposées en **lecture seule** : HYD 3–6K ES et HYD G3, limité à deux MPPT et la batterie 1. Le logger doit être un LSW3/Solarman compatible, avec son vrai numéro de série et son adresse locale.

Le logger ne permet pas de déduire la cartographie de l’onduleur. Ne pas choisir une famille sans avoir identifié le modèle sur son étiquette. Comparer ensuite puissances, sens réseau/batterie, tension, SOC et énergies au LCD. Deux options permettent de corriger les signes réseau et batterie. Les autres Sofar ne sont pas annoncés compatibles.

En mode Sofar, les écritures d’onduleur, le TOU Deye, les commandes de recharge et les automatismes de relais de l’écran sont désactivés. Le profil Deye reste enregistré pour revenir à cette marque.

[Guide des nouvelles fonctions](GUIDE_MICRO_SD_SOFAR.md)

## Validation

Compilation ESP32-S3 réussie et tests logiciels des lecteurs, des interfaces, du stockage et des fonctions existantes. L’historique a passé 100 consultations de 288 mesures, avec contrôle de la pile, erreurs mémoire et déconnexions simulées. Les tests SD couvrent les fichiers incomplets, la conservation limitée au bon dossier et une journée de 25 heures. Les règles horaires ont été comparées à IANA sur 13 régions et 114 360 instants.

Aucun écran, carte SD, logger Sofar ou passerelle APsystems/Enphase n’a été connecté pour cette validation. La disparition du redémarrage sur votre écran et les mesures des nouvelles passerelles restent à confirmer sur le matériel.

L’OTA conserve les réglages enregistrés. Le fichier `_OTA.bin` est une mise à jour de l’application pour un écran déjà installé avec un partitionnement OTA compatible.
