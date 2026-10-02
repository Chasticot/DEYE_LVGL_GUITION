# Version avec VEtronic

La variante **4.2-vetronic-32A** associe le suivi Deye historique SG02LP1 au pilotage d'une **passerelle réseau VEtronic WB01 compatible**. Elle ne contient pas le sélecteur de modèles de la V3. L'écran ne dialogue pas directement avec la borne : il passe par l'API HTTP de la passerelle.

## Mise en route

1. Installer le pack Windows VEtronic, ou compiler `deye_vetronic` ([installation](INSTALLATION_WINDOWS.md)).
2. Configurer le Wi-Fi, puis l'adresse IP et le numéro de série du logger Deye. La méthode pour trouver l'IP du logger est dans le [chapitre 2 du guide V3](GUIDE_UTILISATEUR_V3.md#2-retrouver-lip-du-logger) ; les autres fonctions propres à la V3 ne s'appliquent pas automatiquement à cette variante.
3. Dans les réglages Tempo / VE, activer la page VEtronic et sauvegarder.
4. Toucher la voiture du tableau de bord, puis **RESEAU**, saisir l'IP de la passerelle et enregistrer. La valeur initiale 192.168.1.130 est un exemple à adapter.
5. Comparer les mesures avec la page Web de la passerelle avant le pilotage.

La passerelle doit être joignable sur le réseau local en HTTP, port 80. Le port XML Jeedom 9200 n'est pas utilisé par l'écran. Le firmware de la passerelle est un projet distinct et n'est pas inclus dans les deux binaires écran de cette release.

## Commandes de la page VE

| Commande | Effet |
| --- | --- |
| **− / +** | Prépare une intensité de 6 à 32 A, par pas de 1 A ; aucune commande n'est envoyée avant validation |
| **CHARGE IMMEDIATE** | Envoie la consigne manuelle préparée |
| **ARRET** | Demande le mode arrêt |
| **SOLAIRE** | Confie la régulation solaire à la passerelle |
| **BORNE / JEEDOM** | Rend la main au mode legacy (`$SC -1`) ; ne restaure pas les anciens réglages natifs |
| **RESEAU** | Modifie l'IP de la passerelle |

La plage affichée jusqu'à 32 A n'établit pas la capacité électrique de votre installation. La passerelle doit accepter cette consigne et conserver ses limites configurées. Une ancienne version de passerelle peut refuser plus de 16 A. La limite solaire reste configurée sur la passerelle, indépendamment du réglage manuel de l'écran.

## Protection SOC solaire

L'interrupteur SOC et les seuils **MIN** / **MAX** configurent l'arrêt et la reprise de la charge solaire selon la batterie Deye. Les valeurs sont réglables de 0 à 100 %, avec un écart minimum de 5 points. **ENREGISTRER SOC** applique les modifications ; changer les champs seuls ne les transmet pas. Le réglage ne change pas le mode de charge courant.

## Lire les mesures

La puissance à côté de la voiture est une estimation : courant mesuré `amps` × 230 V, arrondi au watt. Ce n'est pas la consigne `targetA` ni une mesure directe de puissance active. Un tore correctement installé et configuré est nécessaire.

Une erreur API, `wbValid=false`, une mesure incohérente ou une réponse trop ancienne provoque l'affichage `-- W`. La lecture est programmée environ toutes les trois secondes après la précédente ; une donnée locale âgée de plus de douze secondes est indisponible. Les mesures Deye restent indépendantes.

Un mode confirmé par l'API ne prouve pas une charge physique : vérifier le courant réel et le retour de la WB01. Le redémarrage de l'écran n'envoie aucune consigne. Masquer la page VE suspend ses lectures, mais n'arrête pas une charge déjà lancée.

## API attendue et validation

- `GET /api/status` fournit l'état et un jeton CSRF.
- `POST /api/mode`, en-tête `X-CSRF-Token`, accepte `mode=manual&amps=10`, `stop`, `solar` ou `legacy`.
- `POST /api/soc-guard` accepte `socGuard`, `socStop` et `socResume`.

Les commandes sont exécutées hors de la tâche d'affichage ; une seule peut être en cours. Une réponse perdue ne déclenche pas un second POST automatique. Les écritures de pilotage VE directement vers l'onduleur Deye sont désactivées dans cette variante.

Sur le matériel, vérifier le courant avec la passerelle, une faible consigne adaptée puis l'arrêt, les modes solaire/legacy et l'indication indisponible lors d'une coupure réseau. La compilation et les vérifications de fichiers de cette publication n'effectuent aucune commande de charge.
