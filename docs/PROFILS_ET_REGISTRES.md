# Profils Deye et registres — firmware unifié 4.3.5

Le firmware unifié reprend le catalogue et les conversions de la V3 4.3.3 décrits ci-dessous. Le modèle Deye et la borne se choisissent indépendamment. Deye LoRa reste réservé au SG02 AM2 ; le client VE TRONIC WB01 est disponible avec les douze profils sélectionnables. Voir le [guide unifié](GUIDE_UNIFIE_4.3.5.md) pour la sélection et la migration.

## Documents pour les utilisateurs

- [Recapitulatif des nouveautes](RECAP_UTILISATEURS_V3.md) - [PDF a partager](pdf/RECAP_UTILISATEURS_V3.pdf).
- [Guide utilisateur complet](GUIDE_UTILISATEUR_V3.md) - [PDF a partager](pdf/GUIDE_UTILISATEUR_V3.pdf).

Le guide couvre la mise en route, la recherche de l'IP du logger, les menus,
le Web, les sauvegardes, les mises a jour et le depannage.

Copie de `12KSG02LP1_v2`, avec selection du modele sur l'ecran et sur le Web.
La correction GEN Daily R62 est aussi appliquee a la v2. Les fonctions de la base sont decrites dans
[Récapitulatif V2 et V3](RECAP_UTILISATEURS_V3.md) ; les adaptations ci-dessous prennent le pas sur ce document.

## Utilisation

**Configuration > DEYE / SOLARMAN > MODELE DEYE** : choisir la reference dans
la liste, puis **ENREGISTRER**. L'ecran redemarre et charge les adresses,
blocs, conversions et fonctions disponibles pour ce modele. Le menu Web
propose le meme choix sous **Modele Deye**.

Le menu reprend les references de nos comparaisons precedentes, et non une
promesse de compatibilite avec l'ensemble du catalogue Deye.

| Reference | Blocs FC03 par defaut | PV affichables | Remarques |
|---|---|---:|---|
| SUN-12K-SG02LP1-EU-AM2 | 76–108 / 169–195 | 3 | Base v2, comportement historique conserve |
| SUN-12K-SG02LP1-EU-AM3 | 76–108 / 169–195 | 3 | Profil SG02, reglages propres a AM3 ; retours utilisateurs favorables |
| SUN-8K-SG01LP1-EU | 76–108 / 169–195 | 2 | SmartLoad a verifier |
| SUN-6K-SG03LP1-EU | 76–108 / 169–195 | 2 | SmartLoad a verifier |
| SUN-8K-SG05LP1-EU | 76–108 / 169–195 | 2 | SmartLoad a verifier |
| SUN-5K-SG05LP1-EU-AM2-P | 76–108 / 169–195 | 2 | Profil LP1 ; retours utilisateurs favorables |
| SUN-6K-SG05LP1 | 76–108 / 169–195 | 2 | Profil LP1 ; retours utilisateurs favorables, suffixe non precise |
| SUN-12K-SG05LP3-EU-SM2 | 520–541 / 552–673 | 2 | Profil du projet SG05LP3, PV3 absent |
| SUN-10K-SG04LP3-EU | 520–541 / 552–673 | 2 | Famille triphasee BT ; SmartLoad a verifier |
| SUN-25K-SG01HP3-EU-AM2 | 520–541 / 552–675 | 4 | Profil du projet HP3 |
| SUN-20K-SG01HP3-EU-AM2 | 520–541 / 552–675 | 4 | Meme profil HP3, fonctionnement rapporte par l'utilisateur |
| AI-W5.1 ESS (P1) | 76–108 / 169–194 | 2 | Registres valides par retour utilisateur ; sans GEN ni SmartLoad |
| SUN-6K-SG06LP1 | Aucun | — | Reference affichee, enregistrement indisponible : seule la question Daily Load avait ete examinee |

Les suffixes SG01/SG03/SG05LP1 ne sont pas assimiles au SG05LP3. Le 6K-SG06LP1
reste indisponible tant qu'une cartographie complete et sa revision ne sont
pas confirmees ; choisir cette ligne ne modifie pas le modele actif.

Le 1er octobre 2026, l'utilisateur rapporte un fonctionnement apparent,
sans retour negatif, pour neuf references : 8K-SG01LP1-EU, 12K-SG05LP3-EU,
25K-SG01HP3-EU-AM2, 5K-SG05LP1-EU-AM2-P, 6K-SG03LP1-EU, 6K-SG05LP1,
12K-SG02LP1-EU-AM3, 8K-SG05LP1 et 10K-SG04LP3. Les abreviations et doublons
sont regroupes dans les entrees ci-dessus ; « SG05LP1 » seul designe une
famille et ne cree pas une reference supplementaire sans puissance.
Le 12K-SG05LP3-EU utilise l'entree SM2 deja presente. Les retours ne precisent
pas les versions de firmware ni les fonctions individuellement testees.
Les reserves SmartLoad et les limites du pilotage VE restent donc indiquees.
Les trois nouvelles entrees possedent chacune leur sauvegarde ; AM3 reprend
les trois lignes PV et la calibration historique du SG02, sans activer la VE.

## Registres et conversions

`inverter_profiles.h` contient le catalogue unique. Les blocs sont calcules
a partir des adresses actives, controles a 125 registres maximum, et utilises
par le lecteur Solarman. GEN est lu separement (166 en LP1, 667 en LP3/HP3).
Les mesures absentes ne sont ni decodees ni interrogees individuellement.
Dans le menu expert, **65535 signifie absent** pour PV3, PV4, GEN ou SmartLoad.

Le HP3 conserve les facteurs du projet source : PV et batterie x10 W,
tension batterie x0,1 V. Les autres profils utilisent x1 W et x0,01 V.
Les puissances converties et historiques sont en 32 bits afin de ne pas
plafonner une mesure HP3 convertie a 32767 W.
Le decodeur des mots bruts reste 16 bits comme dans les variantes existantes :
les modeles de puissance superieure ne sont pas declares compatibles ici.

Reseau et charge utilisent le facteur historique x10 puis leur coefficient.
Le SG02 conserve ses valeurs et coefficients v2 ; les nouveaux profils LP1
ont un coefficient 0,1 pour des registres en watts directs. Une calibration
personnalisee reste possible. Batterie : charge positive, decharge negative.
Etat reseau : R194 == 1 en LP1 et AI-W5.1 P1 ; R552 bit 2 en LP3/HP3.

PV4 est inclus dans le total, les historiques et la visibilite configurable.
PV3 n'est plus affiche sur les modeles a deux entrees. Le profil SG02 garde
ses trois lignes historiques. SmartLoad conserve le bit configurable du
profil de base ; il ne devient pas automatiquement un etat confirme sur
SG01/SG03/SG05LP1 ou SG04LP3. Comparer ON/OFF avec le LCD sur ces appareils.

GEN Daily utilise R62 en LP1 (SG01/SG02/SG03/SG05) et R536 en LP3/HP3,
avec coefficient 0,1 kWh. Au chargement et a la sauvegarde, un ancien R536
sur un profil LP1 disponible est corrige en R62. La correction est
enregistree en NVS au chargement et s'applique aussi aux imports sauvegardes.
Une adresse 0 (estimation), une autre adresse personnalisee et le coefficient
personnalise sont conserves. Les anciens profils LP1 deja sauvegardes a 0
restent donc en estimation ; REINITIALISER dans Registres perso propose R62.

La recharge VE native Deye LoRa est disponible uniquement sur le profil SG02 AM2 deja valide.
Sur les autres profils, aucune lecture de bloc VE et aucune ecriture VE
n'est autorisee, meme si d'anciens reglages NVS activaient cette option.

## Sauvegarde

Le modele est enregistre par identifiant stable, applique uniquement au
redemarrage. Le lecteur ne peut donc pas decoder une reponse avec un profil
change pendant une requete.

Chaque modele dispose de ses propres registres, mode GEN, options PV,
tarifs, relais, veille et compteur GEN estime. Le Wi-Fi, logger, heure,
theme et autres reglages generaux restent communs. Au premier lancement,
SG02 est selectionne ; ses registres et options sont repris de la v2.
Les autres profils partent de leurs valeurs par defaut, avec relais inactif.
Revenir a un modele retrouve les reglages deja sauvegardes pour celui-ci.
REINITIALISER dans Registres perso propose les valeurs du modele actif ;
il faut ensuite sauvegarder pour les appliquer.

Le JSON exporte `inverter_model`, `pv4_power` et `pv4_visible`. Un import
exige que le modele correspondant soit deja selectionne, afin d'eviter
d'appliquer une cartographie avec les conversions d'une autre famille.
Un ancien JSON sans modele est accepte uniquement sur SG02. Les anciens
exports sans champs PV4 restent compatibles.

## Compilation et verification

Dans le projet Deye-LVGL : `pio run -e 12KSG02LP1_v3`.
Binaire : `.pio/build/12KSG02LP1_v3/firmware.bin`.
Le firmware unifié 4.3.5 est compilé par défaut dans le dépôt. Aucun televersement
sur l'ecran n'est effectue par cette commande.

`firmware/DEYE_V3/tests/run_checks.ps1` execute les tests des profils/blocs/conversions,
la migration et l'isolation NVS avec stockage simule, les cas d'echec de
sauvegarde, les tests de logique et JSON et les rendus LVGL a 480x480.
Les tests et la compilation ne remplacent pas une verification sur l'ecran
et les onduleurs reels. A tester : selection, reboot, comparaison au LCD,
retour au profil precedent, export/import et reglages du relais.

## Provenance

- [PDF des protocoles Modbus et releve des ecarts](protocols/README.md)
  (recherche du 1er octobre 2026). Le protocole monophasé indique notamment
  R62 pour GEN Daily en monophasé ; la correction est integree en 4.3.1.
- Variantes locales : `deye_12ksg02lp1`, `deye_12ksg05lp3/SG05LP3_REGISTERS.md`,
  `deye_25ksg01hp3/SG01HP3_25K_REGISTERS.md`, `deye_ai_w5_1_ess/README_AI_W5_1_ESS.md`.
- Echanges « Comparer registres 12KSG05LP3 » : references supplementaires,
  validation utilisateur du profil 20K/25K, reserve sur SmartLoad et AI-W5.1.
- [Famille LP1, implementation ha-solarman](https://github.com/davidrapan/ha-solarman/blob/main/custom_components/solarman/inverter_definitions/deye_hybrid.yaml).
- [Familles SG04LP3 et SG01HP3, implementation ha-solarman](https://github.com/davidrapan/ha-solarman/blob/main/custom_components/solarman/inverter_definitions/deye_p3.yaml).
- [Releve et conversions SG05LP1, installation 6K](https://github.com/tema-mazy/deye-modbus-test).

Ces sources documentent des familles et des revisions precises. Le 2 octobre 2026,
l'utilisateur confirme le fonctionnement des registres P1 de la variante AI-W5.1
4.3.1 ; ils sont repris en V3 4.3.3. Ce retour ne valide pas toutes les revisions
AI-W5.1 ni les variantes P3. Le SG06 reste sans profil exploitable confirme.

## AI-W5.1 P1 : mise à jour 4.3.3

Le profil conserve son identifiant `ai-w5.1-ess` et adopte les 15 adresses
validées dans la variante autonome : PV186/187, PV jour108, batterie184/183/190/182,
réseau169, état194, achat/vente76/77, consommation178, jour84, températures90/91.
Le SOC du relais devient R184. Les facteurs restent réseau/charge 0,1 (avec
×10 interne), batterie -1, tension 0,01 V et PV 1 W.

Au premier chargement de ce modèle, l'ancienne clé `v3-aiw51/registers` est
migrée vers `registers_p1` : toutes les adresses sont remplacées, temporisations
et coefficients valides conservés. Les options passent de `v3-aiw51-opt/config`
à `config_p1`, avec traduction des anciennes adresses de relais connues.
Une sauvegarde P1 existante reste prioritaire ; une erreur de persistance est
retentée au prochain démarrage. Les autres profils ne changent pas.

Ne pas réimporter un ancien JSON AI-W5.1 avec les adresses 600 : un import
explicite réappliquerait ces adresses. Faire un nouvel export après mise à jour.
Les guides Markdown et PDF incluent la mise à jour 4.3.3.
