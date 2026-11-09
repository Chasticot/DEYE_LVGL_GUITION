# Deye Monitor 4.8.12 — réglages plus simples

Publication du 9 novembre 2026.

## Activer ou désactiver PV5 à PV8

Dans **Installation > Sources PV5 à PV8**, utilisez la case **Activer cette source PV**. Le changement est appliqué immédiatement, même si d’autres champs sont vides ou incorrects.

- **Décocher** arrête la lecture et retire la source des affichages et du total solaire. La dernière configuration enregistrée est conservée pour la prochaine activation.
- Les modifications non enregistrées sont abandonnées. Si aucune configuration n’avait été enregistrée, l’emplacement reste vide.
- **Recocher** reprend les réglages enregistrés. Pour une nouvelle source, renseignez la connexion puis choisissez **Enregistrer les réglages**.

Les réglages des autres sources restent conservés. Les anciennes configurations sont reprises automatiquement après la mise à jour.

## Prévision solaire et aides dans les réglages

- Une puissance de **9,1 kWc** garde un affichage lisible : la longue suite de décimales n’apparaît plus. Virgule ou point acceptés à la saisie.
- Des exemples et des aides courtes sont placés près des champs : coordonnées, puissance des panneaux, orientation, réseau, logger, horaires, recharge et écran.
- La page Web et l’écran rappellent que la **prévision météo est une indication moyenne, peu précise localement**. Nuages, ombres et conditions réelles peuvent créer des écarts importants ; la production annoncée n’est pas garantie.
- Le contraste du formulaire solaire en thème clair est corrigé.

## Mise à jour et vérifications

Installer **DEYE_V3_4.8.12_OTA.bin** depuis la page de mise à jour de l’écran. Le fichier **SHA256SUMS-4.8.12.txt** permet de vérifier le téléchargement. Les réglages déjà enregistrés sont conservés par l’OTA. Les brouillons laissés dans une page Web ne sont pas des réglages enregistrés.

Compilation et tests logiciels réussis, avec vérification des cycles activation/désactivation, du redémarrage simulé, des anciennes configurations et des affichages sur téléphone et écran 480 × 480. Aucun essai sur écran physique n’a été effectué pour cette version.

[Télécharger la 4.8.12](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.8.12) · [Guide de mise à jour](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/MISE_A_JOUR_ECRAN_GITHUB.md)
