# Deye Monitor 4.8.8 — batterie de la maison avant jour rouge

Cette version ajoute une option de recharge réseau de la **batterie de la maison** pendant la nuit précédant un jour rouge Tempo. Elle est indépendante de la voiture et désactivée par défaut. Le tableau de bord actuel, les six rubriques et les intégrations des versions précédentes sont conservés.

## Activer l’option

Sur l’écran : **Réglages > Pilotage > Batterie / Veille rouge**.

Sur le Web : **Pilotage > Time of Use / Batterie > Batterie maison : nuit avant jour rouge**.

Activer **Recharger la batterie la nuit avant un jour rouge**, choisir une cible de **50 à 100 %** (100 % par défaut), puis enregistrer. L’écran redémarre pour appliquer le réglage. Les paramètres sont conservés lors des mises à jour OTA.

## Vérifier le Deye une première fois

1. Le modèle principal doit être un **Deye avec une cartographie Time of Use prise en charge**, hors profils expérimentaux ou sans TOU. Ce réglage n’est pas disponible pour un Sofar principal en lecture seule.
2. Le Time of Use doit être **actif**, la batterie réglée en **SOC (%)**, et les jours concernés activés.
3. Les six horaires TOU doivent comporter des bornes à **06h00** et **22h00**. Exemple possible : **00h00, 06h00, 10h00, 14h00, 18h00, 22h00**. Cet exemple n’est pas appliqué automatiquement : conserver ses besoins habituels et préparer les horaires depuis l’éditeur TOU, puis confirmer sur l’écran. Seules les plages entièrement comprises entre 22h et 6h sont utilisées.
4. L’écran doit être réglé sur **UTC+01:00, heure d’été Auto Europe**. Vérifier que l’horloge du Deye correspond à l’heure de Paris et au compteur Tempo. L’option reste inactive avec un autre fuseau, notamment celui des Antilles.
5. Vérifier sur le Deye l’autorisation générale de charge réseau et ses limites de courant. Les limites de l’onduleur et du BMS restent en vigueur ; l’écran ne les relève pas.

Le message d’état explique un prérequis manquant. Pour préparer la première utilisation, l’activation de l’option peut se faire en journée : aucune charge n’est demandée avant la nuit et l’annonce d’un jour rouge.

## Fonctionnement

Entre **22h00 et 06h00**, l’écran recherche une annonce datée du jour rouge qui commence au prochain 06h00. Après minuit, il utilise donc le jour civil courant, et non le lendemain civil. Une nuit entre deux jours rouges est également concernée.

L’écran mémorise et vérifie les consignes TOU actuelles avant toute écriture. Il autorise ensuite le réseau et relève, si nécessaire, la cible SOC des seules plages de nuit jusqu’au niveau choisi. Une cible déjà supérieure n’est pas abaissée. Les horaires, puissances, jours d’activation, paramètres de jour et bits du groupe électrogène sont conservés.

À la fin du créneau, ou si l’option est désactivée, les valeurs habituelles mémorisées sont rétablies avec contrôle par relecture. Désactiver l’option et enregistrer redémarre l’écran ; la reprise est traitée après reconnexion au Deye. La programmation de nuit ne devient pas une charge réseau en heures pleines si la liaison est perdue : les bornes restent celles du TOU de l’onduleur.

L’état **consignes confirmées** signifie que les valeurs ont été relues dans le Deye. Il ne garantit pas un courant de charge : une batterie déjà chargée, le BMS ou une limite du Deye peuvent empêcher la charge effective.

## Internet, coupure et intervention manuelle

La couleur est récupérée par HTTPS avec certificat vérifié auprès de l’[API Couleur Tempo](https://www.api-couleur-tempo.fr/). Chaque annonce conserve sa date et son heure de réception. Une annonce valable peut être conservée pendant une indisponibilité temporaire, au maximum **deux heures**. Une annonce inconnue, une mauvaise date, une heure non réglée ou une annonce trop ancienne ne déclenche pas la recharge. Un changement de couleur publié est pris en compte au prochain rafraîchissement.

Une sauvegarde de reprise précède les écritures. Après redémarrage, l’écran peut retrouver les consignes précédentes. Une écriture partielle ou incertaine n’est pas rejouée pour démarrer la charge : l’écran tente de revenir aux valeurs mémorisées, puis suspend les nouveaux départs pour cette nuit.

Si le TOU a été modifié ailleurs, ou si le logger/modèle a changé, l’écran signale le problème sans écraser le tableau ni restaurer les consignes vers un autre équipement. Vérifier et rétablir les valeurs voulues sur le LCD du Deye, puis utiliser **Reprise après intervention manuelle > Vérification manuelle terminée** sur le Web, ou le bouton de suivi sur la page de l’écran. Terminer ce suivi n’envoie aucune consigne au Deye.

Pour revenir à un ancien firmware, désactiver d’abord l’option et vérifier le rétablissement du TOU sur le Deye. Les nouveaux paramètres et le journal de reprise sont conservés séparément en mémoire de l’écran ; l’export JSON général ne les contient pas. Les noter pour un remplacement d’écran.

## Validation et mise à jour

Compilation ESP32-S3 et tests logiciels : limites 22h/06h, dates avant/après minuit, prévisions inconnues/périmées, cache daté, SOC/TOU/jours, absence d’écriture avant sauvegarde, accusé perdu, redémarrage, désactivation, changement de fuseau ou de logger, modification externe, formulaires et affichage clair/sombre.

**Aucun essai physique sur un écran relié à un onduleur n’a été réalisé pour cette option.** Vérifier le premier cycle de nuit et le retour aux consignes habituelles sur le LCD du Deye.

[Mise à jour de l’écran](MISE_A_JOUR_ECRAN_GITHUB.md) · [Lecture directe Sossen en essai (4.8.7)](NOTES_4.8.7.md).

Références : [heures pleines et creuses Tempo chez EDF](https://particulier.edf.fr/fr/historique-tempo.html), [format daté de l’API Couleur Tempo](https://www.api-couleur-tempo.fr/).

Les sources restent dans le dépôt privé. La publication publique contient uniquement le binaire OTA, son SHA-256 et les guides utilisateur.
