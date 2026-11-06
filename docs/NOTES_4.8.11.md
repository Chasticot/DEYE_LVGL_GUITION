# Deye Monitor 4.8.11 — compatibilité Time of Use

Publication du 6 novembre 2026. Compilation et tests logiciels réussis ; la comparaison sur un AI-W5.1 P1 physique reste à réaliser.

## Tableau TOU sur AI-W5.1 P1

Les profils AI-W5.1 P1 peuvent désormais lire le tableau **Time of Use**, notamment les références 3,6 / 5 / 6 / 7 / 7,6 / 8 / 10 kW. Le firmware bloquait cette lecture à cause d'une condition liée au profil du port GEN. L'activation du TOU sur le Deye ne suffisait donc pas à remplir le tableau de l'écran.

Le profil historique « AI-W5.1 ESS (P1) », sans puissance nominale précise, permet la consultation. Pour modifier les consignes, sélectionner la référence exacte dans **Deye / Solarman > Mon onduleur**.

## Lecture et affichage

- Si l'appareil refuse le grand bloc de lecture, l'écran essaie automatiquement de petits blocs ; aucun réglage supplémentaire n'est nécessaire.
- Une lecture impossible affiche un **état inconnu**, au lieu de laisser croire que le TOU est inactif. Les erreurs de lecture sont précisées.
- Les seuils de tension haute tension sont affichés avec leur échelle propre ; leur édition reste indisponible.
- Les confirmations, limites du modèle et relectures des consignes restent en place. La simple consultation n'envoie aucune consigne.

## Périmètre et premier contrôle

Les tests logiciels couvrent les 106 profils disposant d'une cartographie TOU de famille, dont les huit profils ESS P1. Cela ne constitue pas une validation sur 106 onduleurs physiques. Les six profils SG06 expérimentaux ou incomplets restent exclus.

Après installation, comparer les **six plages, puissances, seuils batterie, jours et autorisations de charge** au LCD du Deye avant toute première modification. La cartographie de famille P1 doit encore être confirmée sur les révisions matérielles et firmwares concernés. Il n'est pas nécessaire de désactiver puis réactiver le TOU pour consulter le tableau.

Les fonctions précédentes, dont la correction de l'exemple Tempo 4.8.10, sont conservées. Les mises à jour OTA conservent les réglages ; exporter sa configuration avant une mise à jour reste conseillé.

## Mise à jour OTA

Installer uniquement **DEYE_V3_4.8.11_OTA.bin** depuis la page Web locale de l'écran. Le fichier **SHA256SUMS-4.8.11.txt** permet de vérifier le téléchargement. Ce binaire n'est pas une image de première installation USB.

[Téléchargements 4.8.11](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.8.11) · [Guide de mise à jour](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/MISE_A_JOUR_ECRAN_GITHUB.md)
