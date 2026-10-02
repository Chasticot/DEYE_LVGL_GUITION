# Historique

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
