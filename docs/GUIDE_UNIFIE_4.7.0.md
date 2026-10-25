# Deye Monitor 4.7.0

**Pour une première mise en route avec des images :** utiliser le [guide illustré actuel](MISE_A_JOUR_ECRAN_GITHUB.md#commencer-en-images) et le [mémo illustré](RECAP_UTILISATEURS_V3.md).

> Notes de cette version, conservées comme historique. Pour la mise en route actuelle et les problèmes rencontrés, consulter le [guide à jour](MISE_A_JOUR_ECRAN_GITHUB.md) et la [FAQ](FAQ_ECRAN.md). Le catalogue et les états de validation peuvent évoluer selon la version installée.

## Choisir son onduleur

1. Relever **Model / Modèle** sur l’étiquette de l’onduleur. Le numéro du logger ne désigne pas le modèle de l’onduleur.
2. Ouvrir **Configuration > DEYE / SOLARMAN > MON ONDULEUR**.
3. Choisir la **puissance**, puis la **référence exacte**, avec tous les suffixes : AM2, AM3, SM2, BM3, -P…
4. Vérifier la référence complète et les caractéristiques affichées. Appuyer sur **APPLIQUER / REDÉMARRER**, puis confirmer.

Le Web propose les mêmes choix et une recherche facultative, par exemple `8k sg05 sm2`. Les espaces, majuscules et tirets n’empêchent pas cette recherche. Aucune modification n’est appliquée au simple changement de liste. Le bouton reste inactif pour le modèle actuel et pour un profil indisponible.

L’écran choisit les adresses, conversions, entrées PV et fonctions disponibles. Il ne change pas la configuration de l’onduleur. Les réglages généraux, les anciennes sauvegardes par référence et les calibrations personnalisées restent conservés. Une nouvelle référence possède son propre espace de réglages.

## Ce qui est configuré automatiquement

- Famille de cartographie LP1, LP3, HP3 ou ESS P1 et conversions de tension/puissance.
- Les réglages par défaut du SG02 AM3 compensent le facteur historique ×10 du moteur de lecture : R169/R178 sont en watts directs. Les calibrations personnelles déjà sauvegardées restent prioritaires. L’ancien profil SG02 AM2 conserve sa calibration historique pour permettre les retours aux configurations existantes.
- Nombre de voies PV réellement affichables : une, deux ou trois selon les modèles du catalogue.
- Distinction entre MPPT et chaînes de panneaux : les SG01HP3 AM2 20/25 kW ont deux MPPT ; quatre chaînes ne créent pas quatre mesures MPPT. Les voies PV3/PV4 historiques inutilisées ne sont plus lues ni totalisées sur ces profils. Les adresses personnalisées sauvegardées restent conservées en mémoire.
- Présence du port GEN/SmartLoad, options d’énergie et compatibilité de la recharge native.
- Lectures 32 bits réseau/consommation/GEN pour les nouveaux SG05LP3 SM2 14–20 kW : elles permettent de suivre le passage réseau au-delà de 32 767 W. La lecture du mot haut réseau R690 est requise sur ces profils. Une lecture indisponible masque les puissances au lieu de présenter une valeur tronquée. Les profils historiques conservent leur décodage.

## Utilisation du port GEN

Dans **PRODUCTION SOLAIRE / GEN**, choisir :

| Choix | Comportement de l’écran |
|---|---|
| Automatique | Lit le mode de l’onduleur, lorsque la cartographie est documentée. |
| Port non utilisé | Masque le bandeau GEN et suspend les lectures auxiliaires de ce port. |
| Appareils pilotés (SmartLoad) | Présente la fonction de sortie pilotée. Un état non vérifié est indiqué « à confirmer », jamais présenté comme un OFF certain. |
| Production solaire supplémentaire | Présente le port comme une source solaire ; le cumul avec PV reste une option personnelle. |
| Groupe électrogène | Présente la puissance du groupe sans l’ajouter aux panneaux solaires. |

L’automatisme utilise uniquement FC03 : R235 sur LP1 et R133 sur LP3/HP3, à raison d’une tentative par minute. Une réponse inconnue ou périmée laisse le mode « à préciser ». Le réglage manuel reste disponible. Ces choix adaptent l’affichage ; ils n’envoient aucune commande de changement de mode à l’onduleur. Le choix d’une borne reste explicite.

Les anciens modes SmartLoad / GEN MO sont repris lorsqu’ils avaient été enregistrés. Les exports incluent désormais `gen_port_use`. Les anciens exports restent acceptés pour le même modèle ; une fonction absente reste inactive.

## Catalogue et limites

Le catalogue comporte 112 entrées : 106 profils utilisables hors SG06 expérimental, 5 candidats SG06 BM3 et 1 ancienne entrée SG06 incomplète non sélectionnable. Il couvre les combinaisons européennes effectivement recensées des familles SG01/02/03/04/05 LP1, SG04/05 LP3, SG01HP3 AM2 et ESS AI-W5.1 P1. Il ne génère pas toutes les combinaisons théoriques puissance/suffixe, et ne déclare pas compatibles les variantes américaines LP2, les familles haute puissance non intégrées ou toutes les révisions de firmware.

Les cinq SG06 BM3 (7 / 7,6 / 8 / 10 / 12 kW) sont signalés **à confirmer**. La cartographie LP1 de base a un exemple public testé sur un SG06 6 kW ; cela ne valide pas le troisième MPPT, les températures ou les fonctions GEN de chaque BM3. Leur sélection exige une confirmation et n’active aucune recharge native. Comparer les mesures avec le LCD de l’onduleur avant utilisation des automatismes. Une configuration connue reste retrouvable en rechoisissant son ancienne référence.

Les autres profils sont issus des cartographies de famille et des références commerciales recensées ; ils ne sont pas tous testés physiquement. L’état SmartLoad reste marqué à confirmer lorsqu’il n’a pas été validé pour ce modèle. Les références historiques ambiguës sont conservées pour respecter les sauvegardes existantes.

Les références et cartographies sont intégrées au firmware. Les sources de développement sont conservées dans un dépôt privé.

## Vérifier la connexion

La page tactile indique l’état des lectures du modèle **actif**. Sur le Web, **Vérifier les lectures** consulte les derniers résultats en lecture seule. Les messages distinguent Wi-Fi déconnecté, absence de réponse, lectures partielles et caractéristiques différentes du modèle choisi. L’identification lit R0–R22, une fois par minute ; elle ne change jamais la référence sélectionnée. Des données reçues ne prouvent pas à elles seules la compatibilité de toutes les fonctions.

## Sauvegarde et retour arrière

Les sources antérieures et la mémoire complète de l’écran d’essai ont été sauvegardées localement. Ces sauvegardes privées ne sont pas publiées.

La mise à jour 4.7.0 utilise les partitions existantes de 4 Mo ; aucun effacement complet ni changement de taille de partition n’est requis. Ne pas restaurer la sauvegarde mémoire complète sur un autre écran : elle contient les paramètres de cette unité. La restauration complète est un recours. Avant un retour au firmware 4.6.x ou antérieur, rechoisir une référence historique connue de cette version : les nouvelles références de la 4.7.0 n’existent pas dans son catalogue.

## Installer et retrouver les fonctions communes

Depuis **Configuration > Mise à jour > Vérifier sur GitHub > Installer**, ou avec le binaire OTA depuis le Web. Un écran neuf nécessite une installation USB initiale, distincte de cette distribution OTA. Exporter les réglages avant mise à jour et garder l’écran alimenté.

[Guide utilisateur](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/MISE_A_JOUR_ECRAN_GITHUB.md).

