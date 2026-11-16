# Tesla Wall Connector Gen 3 Wi-Fi — écran 4.9.0

Cette intégration affiche les données de la borne sur l'écran GUITION et sa page Web, par le réseau local. Elle ne demande ni Home Assistant, ni compte Tesla, ni licence de l'écran pour ces lectures. La borne doit déjà être installée et connectée au réseau Wi-Fi de la maison.

La [veille de l'écran](GUIDE_VEILLE_HORAIRE.md) peut couper le LCD et le rétroéclairage sur un ou deux créneaux, en gardant le tactile actif. Le suivi Tesla et l'accès Web continuent ; aucun réglage de variation du rétroéclairage n'est utilisé.

**Le Wi-Fi local permet le suivi uniquement.** Le démarrage/arrêt, les ampères, les horaires et la régulation solaire de la charge restent gérés par les équipements et applications habituels. Les règles de charge et tarifs de l'écran ne commandent pas cette borne. Cette intégration concerne la Gen 3 ; elle n'active pas le pilotage RS485 ou Bluetooth et ne lit pas le niveau de batterie du véhicule.

## Installation

1. Exporter la configuration de l'écran depuis sa page Web et conserver son précédent fichier OTA.
2. Installer `DEYE_V3_4.9.0_OTA.bin` depuis **Mise à jour OTA**. Il s'agit du firmware de l'écran ; ne pas l'envoyer à la borne.
3. Dans **Véhicule électrique → Activation / tarifs**, choisir **Tesla Gen 3 Wi-Fi**, enregistrer et laisser l'écran redémarrer. Un seul système de recharge est sélectionné à la fois ; les réglages des autres bornes restent conservés.
4. Ouvrir **Véhicule électrique → Tesla Gen 3 : suivi WiFi → Connexion**. Laisser la recherche automatique trouver et enregistrer l'adresse. Choisir l'alimentation **monophasée ou triphasée**, puis toucher **Enregistrer / tester**. L'adresse peut aussi être saisie manuellement. Le choix mono/tri adapte uniquement le calcul de puissance affichée ; il ne change aucun réglage électrique.
5. Vérifier l'état de la borne, sa version et ses mesures. Une réservation d'adresse dans la box évite que son IP change. Les réseaux invités avec isolation des appareils peuvent empêcher la connexion locale.

Le symbole de voiture du tableau de bord ouvre également la page de la borne. Depuis un téléphone sur le même réseau, ouvrir `http://ADRESSE_DE_L_ECRAN/tesla`. La page de configuration Web propose aussi le choix de borne et un lien vers ce suivi. Les protections Web déjà configurées sur l'écran restent actives.

## Recherche automatique de l'adresse

Après dix secondes sans lecture valide, l'écran parcourt le sous-réseau local, uniquement pour le type de borne sélectionné. Une borne doit répondre avec un état cohérent, une tension, un courant, une énergie de session et son numéro de série/version. Les valeurs nulles de charge au repos sont acceptées. Après le parcours complet, une seule réponse éligible est relue deux fois avant sauvegarde automatique ; les autres appareils HTTP sont ignorés.

Le numéro de série est mémorisé avec l'adresse. Après un changement DHCP, l'écran retrouve la même borne. Si plusieurs bornes répondent sans qu'une identité connue permette de les distinguer, ou si seule une autre borne répond, une liste d'adresses/identités est proposée : choisir, puis toucher **Vérifier / utiliser cette borne**. Le bouton **Rechercher automatiquement** permet de relancer la recherche ; le panneau est accessible en faisant défiler la page Connexion. Le Web présente le même parcours.

L'écran ne remplace pas une adresse après une recherche incomplète, une perte Wi-Fi, une mise à jour en cours, un changement de configuration ou une sauvegarde échouée. Les sous-réseaux de plus de 1 022 adresses utilisables demandent une saisie manuelle. Après un échec, les recherches automatiques sont espacées de cinq minutes. La charge n'est jamais démarrée ou interrompue par cette recherche.

## Données affichées

- État de la borne, véhicule branché et contacteur ouvert/fermé.
- Puissance **estimée** à partir des tensions et courants, courant du véhicule.
- Énergie et durée de la session ; énergie totale lorsque l'API correspondante est disponible.
- Courants et tensions L1/L2/L3, courant neutre, tension réseau et fréquence.
- Températures de la prise, de la carte et du microcontrôleur.
- Codes d'alerte et d'attente, version du firmware, numéro de série et compteurs de lectures.

En monophasé : puissance = tension réseau × courant véhicule. En triphasé : somme des produits tension × courant de chaque phase. L'écran indique `--` pour une mesure absente ou invalide, sans remplacer cette absence par zéro. Certaines valeurs et certains états dépendent du firmware Tesla.

La borne est interrogée toutes les **15 secondes**, uniquement quand elle est sélectionnée. Le bouton **Actualiser** demande une nouvelle lecture. La version est relue environ toutes les dix minutes et le compteur total environ toutes les minutes lorsqu'ils répondent. Une erreur de lecture rend l'état indisponible ; une lecture ancienne de plus de 45 secondes est également masquée. Une interruption des lectures ne commande ni arrêt ni démarrage de la charge.

L'adresse et l'alimentation sont mémorisées après redémarrage/OTA et incluses dans l'export de configuration de l'écran. Un ancien export sans réglages Tesla conserve le choix courant. Le diagnostic téléchargeable depuis la page Tesla retire l'adresse de la borne et son numéro de série ; il conserve les mesures et les codes utiles à l'assistance.

## Essais demandés aux testeurs

La validation logicielle locale est distincte d'un essai sur une borne réelle. Avant d'annoncer la compatibilité matérielle :

1. Comparer état, puissance estimée et énergie de session avec les informations de la borne pendant une charge réelle.
2. Vérifier les états véhicule débranché, branché, en charge et charge terminée, y compris la présence du câble sur le tableau de bord.
3. Vérifier le calcul mono/tri sur l'alimentation réelle, avec une voiture qui peut utiliser moins de phases que la borne.
4. Couper temporairement la connexion Wi-Fi de l'écran ou de la borne : les mesures doivent disparaître, puis revenir après reconnexion. La charge doit rester gérée par la borne et le véhicule.
5. Redémarrer l'écran, vérifier la conservation de l'IP et du choix mono/tri, puis vérifier le suivi depuis un téléphone.
6. En cas d'anomalie, transmettre le diagnostic anonymisé, le modèle/firmware de la borne et le caractère mono/triphasé de l'installation.
7. Vérifier la détection avec l'adresse initialement vide, puis après changement DHCP. Avec deux bornes, vérifier le choix proposé et l'absence de remplacement d'une identité connue par une autre.

Les autres fonctions de la version 4 sont conservées : V2C Trydan/Pro, huit expositions solaires, thèmes horaires, luminosité fixe, recherche de mises à jour facultative et désactivée par défaut. La version 5 avec licence reste séparée et inchangée.

## Références techniques

Lectures HTTP locales fixes : `/api/1/vitals`, `/api/1/version`, `/api/1/lifetime`. Aucun appel de commande ou de mise en service de la borne.

- [Intégration Tesla Wall Connector de Home Assistant](https://www.home-assistant.io/integrations/tesla_wall_connector/).
- [Calcul de puissance dans la bibliothèque utilisée par Home Assistant](https://github.com/einarhauks/tesla-wall-connector/blob/master/tesla_wall_connector/vitals.py).
- [États exposés par l'intégration Home Assistant](https://github.com/home-assistant/core/blob/dev/homeassistant/components/tesla_wall_connector/sensor.py).

Vérification documentaire : 15 novembre 2026. API locale non contractuelle, compatibilité à vérifier selon le firmware de la borne.
