# Validation de la publication du 2 octobre 2026

## Sources et versions

- V3 : **4.3.3**, incluant le profil AI-W5.1 ESS P1 mis à jour.
- VEtronic : **4.2-vetronic-32A**.
- Les fichiers C++ et en-têtes des deux variantes sont repris à l'identique des sources de travail. La refonte ajoute les entrées Arduino et la configuration du dépôt, sans introduire un troisième firmware.

## Contrôles réalisés

- Compilation PlatformIO des deux environnements depuis la nouvelle structure, avec téléchargement des versions déclarées des bibliothèques : réussite.
- Compilation Arduino CLI (mêmes sources et options que le guide Arduino IDE), ESP32 2.0.17 : réussite pour les deux variantes. V3 : 1 488 789 octets d'application, 59 588 octets de RAM statique ; VEtronic : 1 468 201 octets d'application, 58 144 octets de RAM statique.
- Tests V3 : profils, limites des blocs Modbus, conversions, migration GEN, migration AI-W5.1 vers P1, priorités NVS, conservation des autres profils et erreurs de sauvegarde : réussite.
- Tests des horaires, tarifs, relais, veille, énergie GEN et validation des paramètres : réussite.
- JSON : aller-retour, types, champs absents, limites et entrées mal formées : réussite.
- Rendus LVGL à 480 × 480, dimensions des contrôles, saisies horaires et boutons VE : réussite.
- Packs Windows extraits : empreintes de tous les fichiers, vérification sans écran et refus d'un binaire corrompu ou manquant : réussite.
- Sélection COM simulée : CH340 unique, plusieurs CH340, aucun CH340, enumeration indisponible, périphérique déconnecté, doublon et port invalide : réussite.
- Guides PDF actualisés en 4.3.3 : 15 pages pour le guide, 3 pour le récapitulatif, avec contrôle visuel.
- Liens locaux, exclusion des fichiers de compilation et des chemins propres au poste : contrôlés avant publication.

## Portée de la validation

Le fonctionnement des nouveaux registres AI-W5.1 P1 est confirmé par le retour utilisateur du 2 octobre 2026 sur la variante autonome, puis intégré à la V3 avec tests de migration. Cette confirmation ne couvre pas toutes les révisions AI-W5.1 ni les variantes P3.

Aucun téléversement sur écran, aucune commande d'onduleur ni de charge n'a été effectué pour préparer cette publication. La réussite des compilations et d'esptool ne remplace pas les essais sur le matériel réel : choix du modèle, comparaison des mesures au LCD, redémarrage, sauvegardes et fonctions de pilotage.
