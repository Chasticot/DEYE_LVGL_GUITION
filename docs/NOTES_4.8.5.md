# Deye Monitor 4.8.5 — menus à tuiles et source Sofar supplémentaire

Les tableaux de bord existants de l’écran et du Web conservent leur apparence. Les productions PV5 à PV8 ajoutées en 4.8.4 restent visibles avec les MPPT, avec leur détail tactile.

## Se repérer dans les menus

L’accueil des réglages comporte six tuiles, toutes visibles sans défilement. Le Web reprend ces rubriques dans une barre latérale sur ordinateur et un sélecteur sur téléphone.

| Rubrique | Contenu |
|---|---|
| Installation | Onduleur, logger, sources et comptage, sources solaires PV5 à PV8 |
| Énergie | Production par source, prévision solaire, historique et microSD |
| Pilotage | Borne de recharge, tarifs et Tempo, Time of Use et batterie, relais |
| Réseau et heure | Wi-Fi, réseau, date et heure, fuseaux UTC, accès Web |
| Écran | Thème, luminosité, veille et réveil tactile |
| Système | Mise à jour, sauvegarde, diagnostic et réglages avancés |

Chaque sous-menu revient dans sa rubrique. Les pages Web spécialisées disposent également de cette navigation. Les adresses existantes et les QR codes restent valables. Sur le Web, chaque formulaire sauvegarde sa partie des réglages : modifier la luminosité conserve par exemple le choix de la borne et les options Tempo. Les réglages avancés restent repliés au départ.

Le tableau Time of Use et sa confirmation restent sur l’écran, dans **Réglages > Pilotage > Time of Use / Batterie**. L’onglet Web indique ce chemin ; il n’envoie pas de programmation à l’ouverture.

## Essayer un Sofar 3000TL-G3 en complément du Deye

Ce profil expérimental suit un **3000TL-G3 avec logger LSW3** comme source supplémentaire PV5, PV6, PV7 ou PV8. Il ne remplace pas le Deye principal et ne crée pas de mesures batterie pour cet onduleur solaire.

1. Sur l’écran, ouvrir **Réglages > Installation > Sources solaires / PV5 à PV8**, puis scanner le QR code. Sur le Web : **Installation > Sources PV5 à PV8**.
2. Ouvrir une installation libre et lui donner un nom, par exemple « Sofar garage ».
3. Choisir **Sofar 3000TL-G3 + LSW3**, saisir l’IP du LSW3 et son numéro numérique. Utiliser le numéro du **logger**, pas celui de l’onduleur.
4. Conserver **Afficher séparément**, enregistrer et attendre une lecture réussie. L’adresse Modbus vaut 1 par défaut ; elle peut être ajustée dans l’option de connexion Sofar. Le port LSW3 est 8899.
5. Comparer les watts et les kWh au LCD du Sofar. La source affiche sa **puissance de sortie AC**, en une seule ligne PV supplémentaire.
6. Pour additionner cette production, choisir **Ajouter au total solaire** et confirmer qu’elle n’est déjà mesurée ni par les entrées PV ni par GEN. Si GEN la mesure déjà, choisir **Déjà mesurée par le port GEN**.

Les kWh s’ajoutent uniquement si la même journée locale est confirmée. Un changement d’IP, de numéro de logger ou d’adresse Modbus demande un nouvel essai séparé avant l’ajout au total. Les commandes de recharge et de relais continuent d’utiliser leurs mesures Deye habituelles.

La lecture vérifie l’identité A3 de la famille TL et contrôle les réponses LSW3/Modbus. Elle utilise uniquement des lectures ; aucune commande n’est envoyée au Sofar. Les références **3000TLM-G3**, HYD et KTLX ne doivent pas être choisies avec ce profil. Un modèle inconnu reste refusé. Sans réponse récente, la source affiche `-- W` et le total ajouté devient partiel.

Références utilisées pour préparer ce profil : [guide Modbus Sofar, famille A3 et protocole 1…40KTL](https://www.photovoltaikforum.com/core/attachment/452254-sofar-modbus-user-guide-2023-07-23-pdf/) et [cartographie du projet Home Assistant Sofar, branche monophasée](https://github.com/wills106/homeassistant-solax-modbus/blob/master/custom_components/solax_modbus/plugin_sofar_old.py). La validation sur le matériel réel reste nécessaire.

## Installation et validation

Le binaire OTA conserve les réglages de l’écran. Les fonctions 4.8.4 et antérieures sont conservées : DL1000B et son aide Modbus TCP, fuseaux UTC et heure d’été, APsystems/Enphase, historique en mémoire et microSD. L’export JSON général ne contient pas les configurations des sources supplémentaires et leurs jetons : les noter pour un remplacement d’écran.

Compilation ESP32-S3, tests des sauvegardes partielles, parcours Web sur téléphone et ordinateur, rendus LCD clair/sombre et tests de réponses Sofar simulées vérifiés. Aucun flash ni essai sur un écran ou un Sofar physique n’a été effectué pendant cette validation.

Seuls le binaire OTA, son SHA-256 et les guides utilisateur sont publics. Les sources et leur historique restent privés.
