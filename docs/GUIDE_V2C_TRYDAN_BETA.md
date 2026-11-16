# V2C Trydan / Trydan Pro — version 4.9.1 sans licence

Cette intégration en essai ajoute le pilotage local d'une Trydan ou Trydan Pro depuis l'écran GUITION et sa page Web. Elle doit être essayée sur des bornes réelles avant d'annoncer une compatibilité validée. Les vérifications logicielles ne remplacent pas ces essais. Le même firmware 4.9.1 ajoute le [suivi Tesla Gen 3 Wi-Fi](GUIDE_TESLA_GEN3_WIFI.md), sélectionnable à la place d'une V2C.

Elle regroupe également les **8 expositions solaires**, les **thèmes sombre/clair avec horaires** et l'**indicateur orange de mise à jour à gauche du Wi-Fi**. Le réglage de luminosité de l'écran a été retiré ; la veille horaire est conservée. Les réglages de luminosité LED de la borne V2C concernent la borne et restent disponibles.

La [veille de l'écran](GUIDE_VEILLE_HORAIRE.md) propose un ou deux créneaux : LCD et rétroéclairage éteints, tactile actif pour le réveil. Le rétroéclairage est commandé en tout-ou-rien, sans PWM ni variation. Les communications et la régulation de charge restent actives.

## Installer et connecter

1. Depuis la page Web de l'écran, exporter sa configuration et conserver le fichier OTA de sa version actuelle.
2. Dans **Mise à jour OTA**, sélectionner `DEYE_V3_4.9.1_OTA.bin` puis installer. C'est le firmware de l'écran, pas celui de la borne. La configuration est conservée. Si une bêta de la série 5 est déjà installée, utiliser ce formulaire Web manuel pour revenir à la série 4.
3. Ouvrir **Réglages → Pilotage → Borne de recharge → Activation / tarifs**, choisir **V2C Trydan / Pro**, enregistrer et laisser l'écran redémarrer.
4. Ouvrir **Réglages → Pilotage → Borne de recharge → V2C Trydan / Pro → Réseau**. Laisser la recherche automatique trouver et enregistrer l'adresse de la borne ; le port est **80** par défaut. Une saisie manuelle reste possible avec **Enregistrer / tester**. Pour un autre port HTTP, le renseigner avant de relancer la recherche.
5. Vérifier l'état, l'identifiant et la version de la borne. Le test est en lecture seule. L'écran et la borne doivent pouvoir communiquer sur le même réseau ; une réservation d'adresse dans la box facilite la connexion.

Le symbole de voiture du tableau de bord ouvre également la page de recharge. Depuis un téléphone connecté à la maison, la page `http://ADRESSE_DE_L_ECRAN/v2c` propose les mêmes commandes et les diagnostics.

La communication entre écran et borne est locale. Elle ne demande ni Home Assistant, ni broker MQTT, ni compte V2C supplémentaire. L'installation et la configuration énergétique de la borne restent celles de V2C. **Cette version 4 ne demande aucune licence ni activation** : le pilotage V2C, Deye, WB01 et les automatismes restent soumis à leurs réglages habituels, sans verrou commercial.

## Retrouver l'adresse automatiquement

Après dix secondes sans lecture valide, l'écran cherche la V2C sur le sous-réseau local avec le port HTTP enregistré. Il vérifie l'identifiant, le firmware, l'état, la puissance et l'énergie renvoyés ; zéro reste une mesure valide pour une borne au repos. Le parcours doit être complet. Une seule borne éligible est ensuite relue deux fois avant enregistrement vérifié de son adresse.

L'identifiant est mémorisé pour retrouver la même borne après changement DHCP. Si plusieurs bornes répondent sans correspondance unique, ou si l'identité diffère, choisir dans la liste **Réseau → Vérifier / utiliser cette borne** ; le panneau est disponible en faisant défiler la page. Le même choix existe sur le Web. **Rechercher automatiquement** permet une relance manuelle. Les recherches échouées sont espacées de cinq minutes.

Une recherche interrompue ou dépassant le délai, un changement manuel de configuration, une coupure Wi-Fi, une opération OTA, une commande V2C en cours ou un échec de sauvegarde empêchent l'adoption. Les sous-réseaux de plus de 1 022 adresses utilisables demandent une saisie manuelle. La découverte ne commande pas la charge et ne reprend pas la propriété d'une session tarifaire.

## Prévision, affichage et mises à jour

- Dans **Prévision solaire**, préparer jusqu'à huit expositions à la même position. Chaque exposition possède sa puissance, son inclinaison et son orientation. Une puissance de 0 kWc laisse le groupe inutilisé. Les prévisions sont additionnées ; les anciens réglages sont conservés dans l'exposition 1.
- Dans **Réglages → Écran → Thème / horaires**, choisir sombre ou clair, avec une plage de thème sombre facultative, 22 h–7 h par défaut. En dehors de cette plage, le thème est clair. Sans heure valide, le choix manuel est utilisé. Le rétroéclairage est fixe pendant l'éveil et s'éteint en veille horaire.
- Dans **Réglages → Système → Mise à jour**, l'interrupteur **Recherche automatique** active ou désactive la recherche des mises à jour. Le même choix existe sur la page Web de configuration. **Il est désactivé par défaut**, s'applique sans redémarrage et reste mémorisé après une coupure ou une mise à jour.
- Lorsque l'option est activée, après connexion Wi-Fi et réglage de l'heure, l'écran vérifie les mises à jour en arrière-plan après environ une minute, puis toutes les douze heures. Une vérification manuelle repousse la suivante de douze heures. Une collecte solaire ou une opération en cours reporte la vérification.
- Deux flèches orange fixes apparaissent à gauche du Wi-Fi lorsque l'option est activée et qu'une version plus récente avec un OTA compatible et une empreinte a été détectée. **Option désactivée : aucune recherche automatique et aucune flèche, même après une vérification manuelle.** Le bouton **Vérifier sur GitHub** et l'installation manuelle restent disponibles. Toucher l'icône, lorsqu'elle est visible, ouvre la page de mise à jour ; **l'installation reste toujours manuelle et confirmée**.
- La série 4 ne propose pas automatiquement de passer à la série 5. Les URL GitHub existantes sont conservées. La détection automatique nécessite Internet ; le pilotage local et la prévision déjà enregistrée restent distincts de cette vérification.

## Les quatre pages de l'écran

- **Charge** : état, puissance réelle, énergie de session, courant proposé, pause, reprise, solaire seul, solaire avec minimum, réseau + solaire et retour aux profils V2C.
- **Réglages** : sélectionner une fonction, choisir sa valeur puis appliquer. Les réglages d'installation demandent une confirmation.
- **Mesures** : durée de session, puissances disponibles, courants et tensions par phase, état brut et erreur du contrôle dynamique.
- **Réseau** : adresse, port, test de lecture, identification, firmware et compteurs de connexion.

Le signe `--` désigne une mesure ou une fonction indisponible. Les fonctions absentes de la réponse V2C sont désactivées. Une fonction lisible peut encore être refusée en écriture par certaines versions de la borne : relever le résultat et transmettre le diagnostic.

## Commandes intégrées

| Réglage | Fonctionnement |
| --- | --- |
| Courant proposé | Consigne entre les limites relues sur la borne, dans la plage 6–32 A. La régulation dynamique peut ensuite l'adapter. |
| Pause / reprendre | Agit sur la pause de session ; ne déverrouille pas la borne et ne désactive pas ses horaires. |
| Borne autorisée / verrouillée | Autorisation de charger. Ce réglage ne désigne pas le verrouillage mécanique du câble. |
| Horaires V2C | Active ou désactive les horaires déjà préparés dans l'application V2C. |
| Modes solaires / profils | Profils V2C, solaire + minimum, solaire seul, réseau + solaire, stop solaire. |
| Phases | Monophasé, triphasé ou mixte, selon les possibilités de la borne et de l'installation. |
| Régulation dynamique | Active ou désactive la modulation V2C déjà configurée. |
| Suspendre la modulation | Suspend ou rétablit cette modulation. |
| Courants minimum / maximum | Modifie les limites de la borne dans la plage de l'écran, 6–32 A. |
| Puissance souscrite | Budget de la régulation V2C, de −22 000 à 44 000 W dans cette interface. La borne peut imposer une plage plus étroite. |
| Tension d'installation | Paramètre de configuration V2C, 180–460 V dans cette interface ; il est distinct des tensions mesurées par phase. |
| Luminosité LED / logo | Luminosité 0–100 % et activation du logo lumineux, si exposées par le firmware de la borne. |

Avant de modifier phases, régulation, limites, puissance ou tension, vérifier les paramètres de l'installation. Désactiver la régulation ou suspendre la modulation peut retirer le délestage logiciel V2C. L'écran affiche ce point dans la confirmation.

## Solaire et profils V2C

Pour les boutons solaires, le contrôle dynamique V2C doit déjà être configuré et actif, avec une source de mesure opérationnelle. L'écran sélectionne le mode et laisse la borne assurer sa modulation. Il ne remplace pas le compteur, les pinces ou la liaison à l'onduleur.

Les boutons solaires règlent le mode, rétablissent la modulation et demandent la reprise. Ils conservent le verrouillage et les horaires : une borne verrouillée ou une programmation peut donc empêcher la recharge malgré un mode correctement relu.

**Profils V2C** remet le choix du mode aux profils de la borne et rétablit la modulation. Ce bouton ne modifie ni la pause, ni le verrouillage, ni l'activation des horaires. Si la charge était en pause, utiliser ensuite **Reprendre** pour demander une reprise. Le bouton ne restaure pas un ancien jeu complet de réglages.

Le solaire exclusif, la priorité batterie et le délestage dépendent des mesures et réglages natifs V2C. Vérifier leur comportement réel sur l'installation. L'écran ne garantit pas une protection de la batterie pendant une perte de réseau.

## Confirmation des commandes

- **En attente** : la commande est en cours de traitement.
- **Réglages relus sur la borne** : les paramètres demandés ont été relus. Vérifier la puissance pour savoir si le véhicule charge effectivement.
- **Refusée** : la borne a refusé la demande.
- **Annulée** : l'état, la liaison, les tarifs ou l'autorisation ont changé avant l'application.
- **Résultat incertain ou partiel** : une réponse ou une relecture manque, ou seule une partie d'une commande solaire a été appliquée. Vérifier la borne et l'application. L'écran ne renvoie pas automatiquement la commande.

Une modification faite depuis l'application V2C reste visible à la prochaine lecture. Au démarrage, l'écran lit la borne sans imposer de consigne. Une coupure de l'écran ou du réseau ne constitue pas une commande d'arrêt de la borne. Choisir une autre borne dans les réglages de l'écran n'arrête pas celle qui charge déjà.

## Tarifs de l'écran

Les restrictions HC / Tempo sont facultatives dans **Activation / tarifs**. Elles ne prennent la main que sur une charge demandée et confirmée depuis l'écran ou sa page Web, avec un véhicule branché, pendant le démarrage courant.

Une charge pouvant utiliser le réseau peut être mise en pause lors d'une période interdite, puis reprise lorsque les tarifs l'autorisent, si les réglages restent ceux attendus. Le solaire exclusif reste disponible. Une modification externe, un débranchement, un défaut, un résultat incertain ou un redémarrage annule ce suivi. Les profils V2C rendent également la main à la borne.

Désactiver les tarifs ne reprend pas une charge déjà mise en pause. Utiliser **Reprendre** si souhaité. Ces restrictions nécessitent une liaison fonctionnelle et ne remplacent pas les programmations ou protections de la borne.

## Parcours d'essai et retour à transmettre

Noter d'abord : Trydan ou Trydan Pro, firmware V2C, monophasé ou triphasé, véhicule, moyen de mesure énergétique et éventuelle liaison Deye/Solarman. Noter les réglages d'installation avant de les modifier.

| Essai | Vérification attendue |
| --- | --- |
| Véhicule débranché, branché, en attente, en charge, en pause puis en fin de charge | État cohérent avec la borne et l'application ; puissance, durée et kWh plausibles. |
| Pause puis reprise | Paramètre relu ; comportement réel du véhicule ; horaires et verrouillage conservés. |
| Courant 6 A puis une valeur autorisée par l'installation | Consigne relue ; comparer à la mesure par phase, en tenant compte de la modulation V2C. |
| Solaire seul, solaire + minimum, réseau + solaire | Mode réel dans l'application, modulation, prélèvement réseau et comportement de la batterie. |
| Retour aux profils V2C | Profils de la borne actifs ; pause et horaires inchangés. |
| Verrouillage, horaires, éclairage | Réglages cohérents avec l'application et effet visible lorsque pertinent. |
| Réglages d'installation, seulement si approprié | Valeur relue et comportement attendu ; rétablir la valeur initiale après l'essai. |
| Coupure Wi-Fi puis reconnexion, redémarrage de l'écran | Mesures indisponibles pendant la coupure puis rétablies ; aucune nouvelle consigne de charge au redémarrage. |
| Modification depuis l'application V2C | Affichage actualisé ; pas de lutte entre deux automatismes. |
| Tarifs HC / Tempo activés | Pause/reprise uniquement pour la charge pilotée par cet écran ; pas de reprise après redémarrage. |
| Écran et V2C lisant le même logger Deye | Lectures stables des deux appareils et régulation correcte. |

En cas de problème : relever l'action, l'heure et le message exact, puis ouvrir **Diagnostic pour l'assistance** sur la page Web V2C. Le JSON exporté contient les versions, capacités, mesures et résultats ; il retire l'identifiant de la borne et l'adresse IP. Il ne contient pas les identifiants Wi-Fi.

## Périmètre de cette bêta

Les 14 réglages documentés par l'API HTTP locale Trydan sont intégrés. La création détaillée des profils horaires, la gestion RFID, la facturation, OCPP et les mises à jour du firmware de la borne restent dans les outils V2C : l'API locale utilisée ici ne les expose pas dans sa table de commandes. Les autres familles V2C ne sont pas validées par cette intégration.

Sources constructeur : [intégrations locales V2C](https://v2charge.com/support/integrations-api-mcp/), [table HTTP officielle](https://docs.google.com/spreadsheets/d/e/2PACX-1vQGA_7Z4YaSMZeHRTnAP6z_82dVPmM33NxJhvsDBEFn8LyWjX-RX_fkR7KCErqAE4aGFvPrUufooHoM/pubhtml?gid=1147522182&single=true), [liaison Deye](https://v2charge.com/support/power-control/dynamic/photovoltaic/turbo-energy-deye-inverter/).
