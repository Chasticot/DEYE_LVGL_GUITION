# Historique

## Retrait de l’ancienne VEtronic — 3 octobre 2026

- Ancienne VEtronic 4.2 retirée des téléchargements et des sources de la branche principale.
- Deux variantes actuelles : V3 générale et VEtronic V3 pour 12K-SG02LP1.
- Guides, environnements de compilation et outils de création des packs actualisés.
- La release v4.3.3 conserve les fichiers de la V3 générale ; ses empreintes et sa description sont actualisées.

## Publication v4.3.4-vetronic-v3 — 3 octobre 2026

- Variante VEtronic V3 dédiée au Deye 12K-SG02LP1 et à la WB01 avec passerelle ESP32 compatible.
- Menus V3, production PV/GEN, relais GPIO40, veille horaire, tarifs, sauvegardes et OTA.
- Arrêt, charge manuelle 6–32 A selon le plafond annoncé par la passerelle, solaire et **Rendre la main à la borne**.
- Retour à la borne autonome avec possibilité de reprendre le pilotage ; abandon de toute reprise tarifaire locale lors de cette action.
- Protection SOC solaire réglable et vérifiée par relecture ; puissance estimée à partir du courant mesuré, mesures périmées signalées.
- Commandes WB01 disponibles sur le Web de l'écran, avec authentification et jeton CSRF existants.
- Tarifs optionnels appliqués aux charges manuelles lancées localement pendant ce démarrage, abandon après modification externe détectée ou résultat incertain.
- Blocage des anciennes commandes VE Deye LoRa dans cette édition.
- Guide dédié, pack Windows et binaire OTA ; compilation et tests logiciels validés. Vérification matérielle encore à effectuer.

## Publication v4.3.3 — 2 octobre 2026

- Dépôt réorganisé autour de deux variantes : V3 et VEtronic. Historique conservé et ancien état repéré par un tag d'archive.
- V3 **4.3.3** : AI-W5.1 ESS P1 passe aux registres monophasés confirmés par retour utilisateur, avec état réseau R194 et relais SOC R184.
- Migration de l'ancien profil AI-W5.1 vers ses nouvelles clés NVS ; les anciennes adresses sont remplacées, les coefficients et temporisations valides conservés. Aucun changement des autres profils.
- Packs Windows complets : sélection automatique du port d'un CH340 unique, choix manuel dans les autres cas, contrôle des fichiers et effacement facultatif.
- Sources compilables sous PlatformIO avec dépendances fixées et préparation Arduino IDE portable.
- Guides utilisateur, compilation et installation, PDF V3 actualisés.
- Variante VEtronic **4.2-vetronic-32A** conservée et distribuée avec son pack et son binaire OTA propres.

## V3 4.3.1–4.3.2

- Catalogue de modèles Deye, sélection écran/Web et réglages séparés par profil.
- Blocs et conversions adaptés ; PV4 pour les profils concernés, fonctions absentes masquées.
- GEN journalier R62 sur LP1, R536 sur LP3/HP3 ; migration des anciennes adresses LP1 sans écraser les sources personnalisées ou l'estimation choisie.
- Références supplémentaires issues des retours utilisateurs, dont SG05LP1 et SG02LP1 AM3.

## Fonctions V2 reprises dans la V3

Menus regroupés, sources PV/GEN configurables, coefficients de mesures, estimation GEN persistante, tarifs et heures creuses, règles du relais écran, veille horaire, configuration Web et export/import. Le [récapitulatif utilisateur](docs/RECAP_UTILISATEURS_V3.md) distingue ces apports des fonctions déjà présentes dans les versions de base.
