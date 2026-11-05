# Deye Monitor 4.8.6 — Hoymiles, Deye micro et mesure Sossen

Les nouvelles sources utilisent les emplacements **PV5, PV6, PV7 et PV8**, avec les choix **Afficher séparément**, **Ajouter au total solaire** ou **Déjà mesurée par le port GEN**. Une passerelle regroupant plusieurs micro-onduleurs représente une seule source PV. Le Deye principal et les tableaux de bord LCD/Web restent conservés à l’identique ; les six tuiles de réglages restent présentes.

## Connexions disponibles

| Marque | Connexion proposée | Conditions et limites |
|---|---|---|
| Hoymiles | OpenDTU | API locale de lecture accessible ; jusqu’à 16 micro-onduleurs, tous avec des mesures récentes |
| Hoymiles | AhoyDTU | API `/api/inverter/list` et `/api/inverter/id/…` ; jusqu’à 16 micro-onduleurs ; heure de l’écran correcte |
| Hoymiles | DTU-Pro Modbus TCP | Ethernet, port 502, carte de registres `0x1000`, jusqu’à 32 canaux et 16 micro-onduleurs ; adresse Modbus 1 par défaut |
| Deye micro | Solarman V5 | Profil micro-onduleur V118 ; IP et numéro numérique du module Wi-Fi ; port 8899 |
| Deye micro | Modbus TCP | Profil micro-onduleur V118 ; service TCP réellement disponible au port 502 ; adresse Modbus 1 par défaut |
| Sossen | Compteur Shelly EM dédié | Lecture d’un canal 0 ou 1 mesurant uniquement la production solaire |
| Sossen | Compteur Shelly Pro EM / Pro 3EM dédié | API locale Shelly RPC ; canal EM1 sélectionné, ou total triphasé EM sur le canal 0 d’un Pro 3EM |

Les profils APsystems, Enphase et Sofar de la 4.8.5 restent disponibles. Toutes les variantes commerciales et tous les firmwares ne sont pas couverts. Le choix dépend de la connexion locale réellement présente, pas seulement de la marque.

## Mise en service commune

1. Ouvrir **Réglages > Installation > Sources solaires / PV5 à PV8**, puis scanner le QR code. Sur le Web : **Installation > Sources PV5 à PV8**.
2. Ouvrir un emplacement libre, saisir un nom et sélectionner la marque avec sa connexion locale. Saisir l’IP ou le nom local de la passerelle, sans URL ni chemin.
3. Garder **Afficher séparément**, puis enregistrer. Attendre une lecture réussie ; les demandes de relecture sont espacées d’au moins 30 secondes.
4. Comparer la puissance à l’application ou à l’écran du fabricant. Les valeurs absentes ou périmées affichent `-- W`.
5. Si la production doit entrer dans le total, choisir **Ajouter au total solaire** et confirmer qu’elle n’est déjà comptée ni dans les entrées PV ni par GEN. Si GEN la mesure déjà, choisir **Déjà mesurée par le port GEN**.

Les puissances ajoutées concernent l’affichage et l’historique ; les automatismes Deye conservent leurs mesures habituelles. Une configuration ajoutée reste liée à l’identité vérifiée lors du test. Changer de matériel ou de connexion impose de refaire un essai séparé avant addition.

## Hoymiles

Le numéro de chaque micro-onduleur est lu pour identifier l’installation. Si deux sources Hoymiles ajoutées partagent un même micro-onduleur, la seconde est exclue en entier comme doublon, même si les deux passerelles ne regroupent pas exactement les mêmes appareils. Il faut alors corriger les périmètres de mesure. Cette protection complète la confirmation demandée à l’utilisateur ; elle ne remplace pas la vérification du comptage GEN.

Une source est refusée si une de ses liaisons radio ou mesures est indisponible, trop ancienne, incohérente ou au-delà des limites prises en charge. Une passerelle joignable ne suffit pas à rendre actuelle une ancienne valeur. La nuit, certains micro-onduleurs s’éteignent : leur production peut être indiquée indisponible, plutôt que supposée nulle.

OpenDTU doit autoriser l’API en lecture sur le réseau local. AhoyDTU utilise ici son API `inverter`, pas les anciennes variantes `record/live`. DTU-Pro nécessite la carte `0x1000` avec des blocs de 40 adresses ; les variantes `0x4000` ne sont pas lues par ce profil. Les modèles Wi-Fi propriétaires utilisant Protobuf, et les firmwares chiffrés sans cette API, ne sont pas couverts par ces choix.

Les kWh du jour sont ajoutés seulement si la même journée locale est confirmée. OpenDTU doit également avoir une heure synchronisée. Le nombre de sources autorisé reste quatre ; une DTU peut regrouper plusieurs micros dans un seul de ces emplacements.

## Deye micro-onduleur

Les profils lisent la puissance AC et l’énergie du jour de la cartographie micro V118. Le type d’appareil et son numéro de série sont vérifiés : le profil ne doit pas servir à relire l’hybride principal comme une source supplémentaire.

En V5, utiliser le numéro du **module Wi-Fi**, pas le numéro de série du micro-onduleur. En TCP natif, vérifier que le service correspondant est disponible. Un appareil qui n’expose que le protocole V5 ne devient pas compatible avec le port 502 par simple changement du choix dans l’écran. Aucune limitation de puissance ni commande de marche/arrêt n’est envoyée.

## Sossen : ce que fait cette version

**La lecture directe du micro-onduleur Sossen n’est pas intégrée dans cette version.** La référence exacte n’a pas été fournie. Un projet communautaire existe pour certains modèles Tuya, avec identifiant et clé locale, mais cette voie n’a pas été portée ni validée ici. Choisir le profil Sossen de l’écran ne suffit donc pas à lire l’IP du micro-onduleur lui-même.

L’option proposée lit un **compteur Shelly existant, dédié à la production Sossen**. Saisir l’IP du compteur et confirmer que le canal sélectionné mesure uniquement cette production. Ne pas utiliser le compteur général de consommation de la maison. Le modèle Shelly EM de première génération utilise le canal 0 ou 1 ; le Pro EM utilise les composants EM1, et le Pro 3EM son total EM sur le canal 0.

La puissance doit être positive lorsque les panneaux produisent. Si le sens du compteur est inversé, cocher **Inverser le sens du compteur** après comparaison des mesures. Une puissance négative importante reste refusée plutôt que transformée automatiquement en production. Les défauts du compteur bloquent la mesure. L’API locale doit être accessible sans authentification pour ce profil ; aucun relais n’est commandé.

Cette lecture fournit les **watts**, sans inventer de kWh quotidiens à partir d’un compteur cumulé. Si elle est ajoutée au total, le total quotidien peut donc être indisponible. Les autres équipements ne nécessitent pas de compteur externe lorsqu’ils utilisent un des profils directs compatibles.

## Vérification et mise à jour

Compilation ESP32-S3 et tests logiciels réussis : réponses HTTP et Modbus simulées, unités et ordre des mots, identités, fraîcheur, doublons, refus d’ajout avant essai, confirmations, sauvegarde des nouveaux paramètres et affichage sur téléphone. Les fonctions des versions précédentes restent présentes. Aucun écran, micro-onduleur, DTU ou compteur physique n’a été testé pendant cette validation.

Les réglages restent conservés lors d’une OTA. Comme en 4.8.5, l’export JSON général ne contient pas les configurations des sources supplémentaires ; les noter pour un remplacement d’écran. Les sources du firmware restent privées ; seuls l’OTA, son SHA-256 et les guides sont publiés.

## Références de protocole

- [API du projet OpenDTU](https://github.com/tbnobody/OpenDTU-docs/blob/master/docs/firmware/web_api.md).
- [API du projet AhoyDTU](https://github.com/lumapu/ahoy/blob/main/src/web/RestApi.h).
- [Lecteur Hoymiles DTU-Pro](https://github.com/wasilukm/hoymiles_modbus/blob/main/hoymiles_modbus/client.py) et [structure des données](https://github.com/wasilukm/hoymiles_modbus/blob/main/hoymiles_modbus/datatypes.py).
- [Protocole Deye V118](https://www.photovoltaikforum.com/core/attachment/449240-deye-modbus-protocol-v118-pdf/).
- [API Shelly EM](https://shelly-api-docs.shelly.cloud/gen1/#shelly-em-status), [EM1](https://shelly-api-docs.shelly.cloud/gen2/ComponentsAndServices/EM1/) et [EM](https://shelly-api-docs.shelly.cloud/gen2/ComponentsAndServices/EM/).
- [Projet Sossen Tuya étudié, non intégré](https://github.com/Loudramin/sossen-ha).
