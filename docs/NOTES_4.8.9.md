# Version 4.8.9 — carte microSD et recharge anticipée de la batterie

Cette mise à jour conserve le tableau de bord actuel. Elle complète les menus de la carte microSD et la préparation de la batterie de la maison selon la couleur Tempo ou la production solaire prévue. Elle ne pilote pas la recharge de la voiture.

## Carte microSD

Sur l’écran : **Réglages → Énergie → Historique / carte SD**. Sur le téléphone ou l’ordinateur : **Énergie → Historique et carte SD**.

- État de la carte, capacité utile, espace libre et mesures en attente.
- Archivage activable, conservation de 7, 30, 90 ou 365 jours, choix d’un jour et téléchargement de son CSV depuis le Web.
- **Sauvegarder maintenant** écrit les mesures encore en attente.
- **Retirer en sécurité** sauvegarde, ferme les fichiers et libère la carte. Attendre le message de confirmation avant de la retirer. Après réinsertion, choisir **Reconnecter / vérifier**.
- **Effacer les archives** supprime uniquement les CSV datés du dossier `deye-history`, après confirmation.
- **Formater la carte** réalise un formatage rapide FAT32, après confirmation. **Tous les fichiers de la carte sont perdus**, y compris ceux d’autres appareils. Télécharger d’abord les archives utiles ; ne pas retirer la carte ni couper l’alimentation pendant l’opération. Ce formatage n’est pas un effacement sécurisé.

Une carte FAT32 de **4 à 32 Go** est recommandée. Une carte absente, incompatible ou en erreur laisse l’historique des dernières 24 h fonctionner en mémoire vive. Aucun formatage automatique n’est déclenché lors d’une insertion ou d’une erreur. Après un retrait sécurisé, reconnecter la carte avant de demander son formatage.

### Limiter l’usure des mémoires

Les mesures sont prises toutes les cinq minutes et regroupées par six avant écriture, soit environ **une sauvegarde toutes les trente minutes**. Jusqu’à trente minutes récentes peuvent être perdues en cas de coupure : utiliser « Sauvegarder maintenant » avant une coupure prévue. Une ouverture ou une actualisation de la page ne sauvegarde rien.

Les historiques ne sont jamais recopiés périodiquement dans la mémoire flash de l’ESP. La conservation est contrôlée au changement de jour. Les réglages de carte inchangés ne sont pas réécrits. La mémoire interne est gérée par le mécanisme NVS d’Espressif ; le journal de batterie n’est enregistré qu’aux transitions, pas à chaque lecture.

## Batterie de la maison : deux modes simples

Sur l’écran : **Réglages → Pilotage → Batterie / anticipation**. Sur le Web : **Pilotage → Time of Use · batterie**.

L’option reste **désactivée par défaut**. Les réglages de la version 4.8.8 sont conservés : une option déjà activée garde sa cible Tempo et ses trois nouvelles règles restent désactivées.

1. **Avant un jour rouge Tempo** : autorise temporairement la charge réseau entre 22h et 6h si le jour qui suit la nuit est annoncé rouge. Les horloges doivent être à l’heure de Paris, avec UTC+01:00 et heure d’été automatique Europe sur l’écran.
2. **Selon la prévision solaire — tous contrats** : utilise les heures creuses définies dans **Pilotage → Tarifs et Tempo**. Aucun abonnement Tempo n’est nécessaire. Au moins une règle solaire doit être activée. Une plage nocturne après minuit prépare la production du jour qui commence ; une plage d’après-midi ou de soirée prépare le lendemain.

Configurer d’abord **Prévision solaire** : emplacement et caractéristiques de l’installation. Une prévision absente, portant sur un autre jour ou datant de plus de six heures ne déclenche aucune règle solaire. Une annonce Tempo périmée ou inconnue ne déclenche pas le mode jour rouge.

### Trois règles facultatives

Chaque ligne comporte un interrupteur, « plus de » ou « moins de », un seuil en kWh et une cible de batterie de **5 à 100 %**. Les limites réelles de l’onduleur et du BMS restent prioritaires.

**La dernière règle activée dont la condition est remplie gagne.** Les comparaisons sont strictes : « plus de 20 » ne comprend pas exactement 20. Le formulaire Web propose deux boutons d’exemple ; ils remplissent le brouillon sans activer ni enregistrer l’automatisme.

Exemple Tempo demandé :

| Ordre | Production prévue pour le jour préparé | Cible |
|---|---|---|
| 1 | Plus de 10 kWh | 90 % |
| 2 | Plus de 20 kWh | 60 % |
| 3 | Plus de 30 kWh | 30 % |

À 35 kWh, les trois conditions sont remplies : la troisième sélectionne **30 %**. En mode Tempo, si aucun seuil ne correspond, on peut utiliser la cible Tempo par défaut ou conserver le TOU habituel. Si des règles solaires sont activées mais que la prévision est indisponible, aucune charge anticipée ne démarre, même avec cette cible de repli.

Exemple sans Tempo :

| Ordre | Production prévue | Cible |
|---|---|---|
| 1 | Moins de 30 kWh | 30 % |
| 2 | Moins de 20 kWh | 60 % |
| 3 | Moins de 10 kWh | 90 % |

À 5 kWh, la cible est **90 %**. À 15 kWh, elle est **60 %**. À 30 kWh ou davantage, aucune de ces règles ne correspond : le TOU habituel est conservé.

### Vérifier les plages du Deye

Avant d’activer l’option :

- TOU activé, batterie réglée en pourcentage SOC et jours concernés cochés sur le Deye.
- Horloges écran, onduleur et contrat cohérentes.
- Première plage à **00h00 ou 01h00 selon le modèle** ; les six débuts sont croissants. Il s’agit d’une précaution pour cet automatisme. Les notices officielles présentent les deux conventions ; elles ne permettent pas d’affirmer l’existence d’un bug universel pour tous les firmwares.
- **Au moins une plage TOU doit être entièrement comprise dans les heures creuses, début et fin inclus.** La fin d’une ligne correspond au début de la suivante ; la dernière se termine au début de la première le jour suivant. Une plage qui déborde en heures pleines est ignorée.

Exemple Tempo : `00:00 → 06:00` et `22:00 → 00:00` sont admissibles. `21:00 → 00:00` déborde en heures pleines et n’est pas utilisée. Il n’est plus obligatoire que toutes les bornes 22h/6h existent : au moins une plage complètement en HC suffit. L’écran ne déplace aucun horaire.

Les autorisations générales et limites de charge réseau/BMS se règlent sur le Deye. L’automatisme ajuste uniquement la cible SOC et l’autorisation de charge réseau des plages admissibles. Il ne change ni la puissance, ni les horaires, ni les autres plages, ni l’autorisation du générateur. Une cible plus basse ne constitue pas une commande de décharge forcée ; le fonctionnement habituel du TOU continue de s’appliquer.

Notices constructeur : [SUN-7.6/8K-SG01LP1-EU — exemple à 01h00](https://www.deyeinverter.com/deyeinverter/2024/08/13/instructions_sun-7.6-8k-sg01lp1-eu_240813_en.pdf) et [SUN-5–25K-SG01HP3-EU-AM2 — exemple à 00h00](https://www.deyeinverter.com/deyeinverter/2026/02/24/rand/7608/%E3%80%90b%E3%80%91manual_sun-5-25k-sg01hp3-eu-am2_20260224_en.pdf). Les horaires Tempo 22h–6h sont décrits par [EDF](https://particulier.edf.fr/fr/historique-tempo.html).

### Retour au programme habituel

Avant toute commande, l’écran sauvegarde et relit le programme TOU original dans un journal conservé après redémarrage et après OTA. Il restaure les valeurs précédentes dès que la condition disparaît, si une prévision nécessaire devient inutilisable, si l’option est désactivée ou à la fin des heures creuses. Une évolution de cible entraîne d’abord cette restauration ; une éventuelle nouvelle application est différée.

Pour limiter les écritures de l’écran et de l’onduleur : **trois démarrages maximum par jour préparé**, espacés d’au moins **trente minutes**, avec compteur conservé après redémarrage. Le retour au programme habituel reste prioritaire. Sans changement, les contrôles n’écrivent ni le journal ni les registres du Deye.

Si l’onduleur est inaccessible, la restauration attend le retour de la liaison ; les horaires TOU déjà en place limitent la charge ajoutée aux HC. Un résultat incertain n’est pas rejoué : l’écran conserve le journal et tente une restauration vérifiée. Si le TOU a été changé ailleurs ou si le logger a été remplacé, il demande une vérification manuelle et n’écrase pas les autres réglages. Le bouton « Vérification manuelle terminée » ne commande pas l’onduleur : l’utiliser après avoir contrôlé et rétabli les valeurs souhaitées sur son LCD.

L’export JSON inclut désormais les règles de recharge anticipée et les réglages de conservation SD. Le journal de restauration n’est jamais exporté ou importé sur un autre appareil. Les anciens exports restent acceptés sans effacer les nouvelles options absentes du fichier.

## Validation

Compilation ESP32-S3, essais logiciels des règles, passages de minuit, seuils exacts, retour TOU, panne de communication, journal après redémarrage, absence d’écriture en consultation, commandes SD simulées et interfaces Web/LVGL vérifiés. Le tableau de bord reste identique.

**Les essais physiques sur carte microSD et un cycle de recharge réel sur Deye restent à effectuer.** Aucun écran ni aucune carte n’a été formaté ou mis à jour pendant cette préparation. Une consigne confirmée par relecture ne garantit pas à elle seule le passage réel d’un courant de charge.
