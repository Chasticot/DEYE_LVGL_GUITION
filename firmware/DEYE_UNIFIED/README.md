# Deye LVGL — firmware unifié 4.3.8

Un seul firmware pour l'écran Guition ESP32-S3 480 × 480, avec choix du modèle
d'onduleur et du système de recharge. Il réunit le catalogue multi-onduleurs
de la V3 et le client VE TRONIC WB01 de la variante Vetronic V3.

Les versions précédentes restent dans leurs dossiers. Le projet PlatformIO
à la racine du dépôt compile désormais `deye_unified` par défaut.

## Choix des équipements

Le modèle Deye et la recharge sont deux réglages indépendants :

| Recharge | Utilisation |
|---|---|
| Aucune | Mesures Deye et fonctions communes, sans communication avec une borne. |
| Deye LoRa | Lecture et commandes des registres VE, uniquement avec le profil SUN-12K-SG02LP1-EU-AM2. |
| VE TRONIC WB01 | Lecture et commandes HTTP via la passerelle ESP32 WB01, avec tous les profils Deye sélectionnables sur l'écran. |

Une seule recharge est active à la fois. Le modèle et le système de recharge
sont chargés au démarrage : enregistrer une nouvelle sélection nécessite un
redémarrage. Le système choisi détermine les pages, les communications et les
automatismes tarifaires utilisés.

Le catalogue contient treize références, dont douze sélectionnables :

- SUN-12K-SG02LP1-EU-AM2
- SUN-12K-SG05LP3-EU-SM2
- SUN-25K-SG01HP3-EU-AM2
- AI-W5.1 ESS (P1)
- SUN-8K-SG01LP1-EU
- SUN-6K-SG03LP1-EU
- SUN-8K-SG05LP1-EU
- SUN-10K-SG04LP3-EU
- SUN-20K-SG01HP3-EU-AM2
- SUN-5K-SG05LP1-EU-AM2-P
- SUN-6K-SG05LP1
- SUN-12K-SG02LP1-EU-AM3

Le SUN-6K-SG06LP1 reste affiché comme indisponible : sa cartographie complète
des registres n'est pas établie. La sélection WB01 ne rend pas les registres
Deye LoRa compatibles avec un autre modèle.

## Installation et migration

1. Sauvegarder la configuration JSON de l'écran avant la mise à jour.
2. Installer le firmware 4.3.8 sur l'écran, puis vérifier le modèle Deye dans
   les réglages. Le modèle mémorisé par la V3 est conservé.
3. Dans les réglages du véhicule, choisir explicitement Aucune, Deye LoRa ou
   VE TRONIC WB01, enregistrer et laisser l'écran redémarrer.
4. Pour WB01, renseigner l'adresse IP de sa passerelle. Pour Deye LoRa,
   vérifier les registres VE avant de débloquer leurs écritures depuis le Web.

L'ancienne activation VE était ambiguë : elle désignait LoRa dans la V3 et
WB01 dans la variante Vetronic. Elle n'est donc pas convertie automatiquement
en système de recharge. Sans nouveau choix explicite, la recharge reste
désactivée. Les anciens exports sans ce choix ne doivent pas réactiver une
borne à partir de leur seul champ d'activation VE. Un ancien export avec
VE activé est refusé avec une indication explicite ; un export avec VE
désactivé reste importable pour le même modèle. Les nouveaux exports
contiennent `ev_backend` pour distinguer sans ambiguïté les trois choix.

Les registres et options propres à chaque modèle conservent leurs espaces de
sauvegarde V3. Les paramètres communs Wi-Fi, logger, heure, tarifs, affichage
et authentification Web sont repris. Comme les espaces NVS sont partagés
avec les anciennes versions, certains changements se retrouvent aussi si
l'on revient à un ancien firmware.

Lors de la première migration, les adresses VE Deye personnalisées sont
reprises, mais leurs écritures restent verrouillées. L'utilisateur les
débloque explicitement dans cette nouvelle version après vérification.

Le redémarrage n'envoie aucune consigne de charge. La désactivation ou le changement du
système dans l'écran n'arrête pas une charge déjà en cours sur l'équipement.
Utiliser ses commandes d'arrêt avant un changement si un arrêt est souhaité.

La restriction tarifaire Deye LoRa concerne uniquement une charge Libre
lancée et confirmée depuis l'écran tactile pendant le démarrage courant.
Une charge autonome ou déjà présente au démarrage n'est pas reprise par
l'écran. Une commande locale vers un autre mode, une modification externe
détectée ou un résultat incertain annulent la propriété tarifaire. Le
redémarrage repart sans propriété ni reprise automatique.

## VE TRONIC WB01

Les commandes disponibles sont Arrêt, Charge immédiate, Solaire et Rendre la
main à la borne. Le courant demandé respecte le plafond annoncé par la
passerelle et une limite locale de 32 A. Les seuils SOC solaire sont sauvegardés
sur la passerelle quand son API les prend en charge.

Le courant mesuré, la consigne et sa confirmation sont affichés séparément.
La puissance précédée de `~` est une estimation à 230 V, pas une mesure de
puissance active.

Le firmware de la passerelle WB01 reste distinct de celui de l'écran. Son
calcul solaire utilise ses propres réglages Deye et coefficients : changer
le modèle sur l'écran ne les met pas à jour. Vérifier et configurer la
passerelle pour le modèle réellement utilisé avant une régulation solaire.
La disponibilité du client WB01 avec tous les profils de l'écran ne constitue
pas une validation matérielle de toutes ces combinaisons.

La restriction tarifaire WB01, lorsqu'elle est activée, concerne les charges
manuelles lancées et confirmées depuis cet écran ou son Web pendant le
démarrage courant. Une pause peut reprendre après le retour des tarifs
autorisés. Arrêt manuel, Solaire ou Rendre la main annulent cette reprise.
Une réponse perdue ne provoque aucun renvoi automatique du POST. Le
redémarrage ne récupère aucune ancienne pause ni propriété de charge.

## Fonctions communes

Avec **GEN MO** et **Cumuler GEN MO + PV**, GEN agit comme un PV5 virtuel :
sa puissance de production s'ajoute aux PV visibles et ses kWh au total du jour.
Les deux signes après calibration sont acceptés. Le compteur journalier GEN
reste utilisé s'il est configuré ; l'estimation utilise la même puissance positive.
SmartLoad et les profils sans GEN sont exclus du cumul.

Tableau de bord PV, réseau, consommation et batterie ; historique ; réglages
des registres et coefficients ; GEN/SmartLoad selon le profil ; tarifs et
Tempo ; relais GPIO40 ; veille ; thèmes et luminosité ; configuration Web,
diagnostic, export/import JSON et mise à jour OTA.

Les pages de la recharge inactive ne permettent pas de commander celle-ci.
En mode WB01, les lectures automatiques et écritures VE Deye LoRa sont
bloquées, même si un ancien réglage autorisait les écritures. En mode LoRa ou
Aucune, le client WB01 n'envoie pas de commandes à la passerelle.

## Compilation

Depuis la racine du dépôt, compiler les sources firmware/DEYE_UNIFIED/ :

```powershell
pio run -e deye_unified
```

Le binaire applicatif OTA est .pio/build/deye_unified/firmware.bin. Les livrables sont disponibles dans la [release v4.3.8](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.8). Voir le [guide PlatformIO](../../docs/COMPILATION_PLATFORMIO.md) et la [validation de publication](../../docs/VALIDATION.md).

Configuration matérielle : ESP32-S3, PSRAM OPI, écran 480 × 480, USB CDC actif,
flash 4 MB et partition Minimal SPIFFS avec OTA.

## Vérification

Les tests hôte se trouvent dans `tests`. Depuis la racine du dépôt :

```powershell
./firmware/DEYE_UNIFIED/tests/run_checks.ps1
node ./firmware/DEYE_UNIFIED/tests/web_test.cjs
```

Avant utilisation quotidienne, vérifier sur le matériel les mesures du
profil choisi, la sauvegarde et le redémarrage des sélections, l'absence de
commandes de l'autre système de recharge, puis les commandes et restrictions
tarifaires de la borne. Avec WB01, vérifier aussi le courant, le retour à la
borne, le solaire et les seuils SOC. Une compilation ou un test logiciel ne
valide pas à lui seul la communication avec l'installation réelle.
