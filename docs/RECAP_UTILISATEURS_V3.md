# Deye Monitor — mémo de démarrage

**Édition du 26 octobre 2026 — version publique de référence : 4.7.2.** Le nom de fichier V3 est conservé pour les anciens liens.

[Guide pas à pas](MISE_A_JOUR_ECRAN_GITHUB.md) · [Questions fréquentes](FAQ_ECRAN.md) · [Dernière version stable](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest).

## Avant de brancher

- Identifier le matériel : référence complète du DEYE, modèle et numéro du **logger**, version de l’écran.
- Utiliser l’alimentation prévue pour cette variante. USB **5 V**, ou entrée 230 V uniquement si le matériel la prévoit et si les bornes sont protégées.
- Pas de batterie intégrée. Powerbank possible en 5 V, sans autonomie garantie.
- Vérifier cotes, profondeur, fixation et protection des bornes avant encastrement. Boîte ronde ou boîte étanche : aucune compatibilité universelle validée.

## Les cinq étapes

1. **Wi-Fi** : Configuration > **RÉSEAU / HEURE > WIFI / RÉSEAU > SCAN**. Choisir le 2,4 GHz, saisir son mot de passe, sauvegarder et vérifier l’IP de l’écran.
2. **Modèle** : **DEYE / SOLARMAN > MON ONDULEUR**. Choisir la puissance et la référence exacte, suffixes compris, puis **APPLIQUER / REDÉMARRER**.
3. **Logger** : **CONNEXION / LOGGER**. Choisir LSW, LSE ou DL1000B, saisir son vrai numéro, rechercher puis sauvegarder le bon résultat. « Numéro non vérifié » exige une identification dans RESULTATS.
4. **Mesures** : attendre et comparer au LCD du DEYE. Contrôler modèle, unité et coefficients si une valeur est incohérente.
5. **Heure** : vérifier date, fuseau et NTP. Sans Internet, régler manuellement ; recommencer après coupure d’alimentation si nécessaire.

## Repères à garder sous la main

| Information | Usage |
| --- | --- |
| IP du logger | Lecture des données par l’écran. |
| IP de l’écran | Page Web, diagnostic, export JSON et OTA depuis un téléphone/PC. |
| Numéro du logger | Connexion/logger ; différent du numéro de l’onduleur. |
| Ports proposés | LSW 8899, LSE 8899, DL1000B 502, à confirmer pour la configuration réelle. |
| DL1000B-WIFI | Mode prévu dans le logiciel ; compatibilité réelle et Modbus TCP à valider avant installation. |
| Sans box | Possible avec un LSW/AP compatible. 10.10.100.254 est l’IP habituelle de certains AP LSW, pas une IP universelle à copier sur le réseau de la box. |

## Si ça ne fonctionne pas

| Symptôme | Première vérification |
| --- | --- |
| Wi-Fi absent | 2,4 GHz, nom/mot de passe, distance, alimentation. |
| Wi-Fi connecté mais DEYE injoignable | Même réseau joignable, bon logger, IP, numéro, port/unité et modèle. |
| Fonctionne près de l’onduleur seulement | Portée Wi-Fi ; tester plus près du point d’accès. |
| Coupures ou données cloud incomplètes | Qualité du réseau, autres lecteurs et intervalle de lecture ; voir la FAQ avant réglage. |
| IP changée après coupure | Réservation DHCP ; récupération automatique avec vrai numéro enregistré en LSW/V5 seulement. |
| Puissance ou kWh incohérents | Même instant de comparaison, bon modèle, unités, coefficients et heure. |
| Écran noir à certaines heures | Veille horaire, durée de réveil et heure. |

## Micro-onduleurs sur GEN

Dans **PRODUCTION SOLAIRE / GEN**, choisir **Production solaire supplémentaire** (ancien GEN MO), activer le cumul GEN + PV si souhaité et sauvegarder. Vérifier que PV Daily ne comprend pas déjà GEN. W et kWh sont distincts ; **~** indique une estimation et **--** une donnée indisponible. Le groupe électrogène n’est pas du solaire. Ces réglages adaptent l’affichage sans changer le mode du port sur l’onduleur.

## Mettre à jour

Exporter le JSON puis **CONFIGURATION > MISE À JOUR > VÉRIFIER SUR GITHUB > INSTALLER**, avec Internet et heure correcte. Ou télécharger le **_OTA.bin** adapté et l’envoyer depuis la page Web de **l’écran**. Garder l’alimentation jusqu’au redémarrage, même si l’affichage scintille.

Ni ZIP, ni archive Source code, ni image USB dans l’OTA. Une première installation/récupération USB demande un pack distinct et adapté. La 4.7.2 renforce la reconnexion Wi-Fi, y compris en liaison AP, sans garantir toutes les variantes et coupures de logger.

## Pour demander de l’aide

Indiquer modèle DEYE, logger/firmware, version de l’écran, type de réseau et symptôme exact. Ajouter une photo du LCD DEYE et de l’écran au même instant si les mesures diffèrent. Masquer mots de passe, QR d’accès et identifiants privés avant publication.

Les anciens [manuel détaillé](GUIDE_UTILISATEUR_V3.md) et [PDF du récapitulatif](pdf/RECAP_UTILISATEURS_V3.pdf) restent en édition **4.6.3**. Pour les procédures et limites actuelles, utiliser ce mémo, le [guide](MISE_A_JOUR_ECRAN_GITHUB.md) et la [FAQ](FAQ_ECRAN.md).
