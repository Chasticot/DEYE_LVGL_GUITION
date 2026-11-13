# Deye Monitor - Les premières mesures

**Fiche de démarrage rapide - 13 novembre 2026.** Wi-Fi, logger, vérification : les seuls réglages nécessaires pour commencer. [Fiche PDF à imprimer](pdf/DEMARRAGE_RAPIDE.pdf) · [Pas à pas en images](DEMARRAGE_SIMPLE.md).

[Voir la fenêtre de premier démarrage](images/demarrage/premier-demarrage.png) : capture du simulateur avec configuration vierge, pas un essai sur installation réelle.

## 1. Connecter le Wi-Fi

Alimenter l’écran avec l’alimentation prévue pour sa variante. Dans la fenêtre **DÉMARRAGE RAPIDE**, toucher **1. WIFI** : **SCAN**, réseau de la maison en **2,4 GHz**, mot de passe, **SAUVEGARDER**. Attendre le redémarrage et l’état **Wi-Fi : connecté**.

La fenêtre est ajoutée à la préparation du firmware ; sa présence dépend de la version installée. Si elle n’apparaît pas, passer par **Réglages > Réseau / Heure > Wi-Fi / Réseau**. Le logger doit être joignable sur le même réseau local. Éviter le Wi-Fi invité isolé.

## 2. Relier le logger

Toucher **2. LOGGER** ou ouvrir **Installation > Connexion / logger** (ancien menu **DEYE / SOLARMAN**). Choisir le type du petit module fixé à l’onduleur et son **vrai numéro de série**, puis son **IP locale**. **12345678 et 123456789 sont des exemples à remplacer.**

| Logger | Port proposé | À vérifier |
| --- | --- | --- |
| LSW | 8899 | Numéro du logger, pas celui de l’onduleur. |
| LSE | 8899 | IP/port réels du service local. |
| DL1000B | 502 | Modbus TCP activé dans le logger. |

Conserver l’unité proposée **1** si l’installation utilise cette valeur ; sinon utiliser celle indiquée par l’installateur. **RECHERCHER** peut aider à trouver l’IP. Un résultat « numéro non vérifié » doit être identifié avant de le choisir. Puis **SAUVEGARDER** et attendre le redémarrage.

**DL1000B :** préparer son Modbus TCP selon le [guide illustré](GUIDE_DL1000B_MODBUS_TCP.md), sans supprimer la liaison cloud utilisée. Une application DeyeCloud fonctionnelle ne prouve pas l’accès local.

## 3. Choisir le modèle et vérifier

Dans **MON ONDULEUR**, choisir la référence complète de l’étiquette, suffixes compris, puis appliquer. Toucher **VOIR MESURES**. Comparer au même instant **PV, batterie, maison et réseau** avec l’afficheur du DEYE. **0,75 kW = 750 W.** Les valeurs doivent rester actualisées ; une connexion Wi-Fi seule ne suffit pas.

Avec le nouveau démarrage guidé, revenir par **Installation > Démarrage rapide**, cocher **Modèle choisi, mesures comparées au DEYE**, puis **TERMINER**. Le bouton attend des lectures récentes. **PLUS TARD** permet de continuer ; la fenêtre reviendra au prochain démarrage tant que la configuration n’est pas validée. Les écrans déjà configurés gardent leurs réglages lors d’une OTA compatible.

## Si les mesures n’arrivent pas

**Wi-Fi absent :** vérifier réseau 2,4 GHz, mot de passe et portée. **Wi-Fi connecté, DEYE injoignable :** vérifier type, numéro, IP du logger, port, unité et Modbus TCP pour DL1000B. **Valeur ×10 :** vérifier le profil, l’unité et le nom de la mesure ; ne pas modifier Load power pour corriger PV. **PV5 à PV8 ne sont pas nécessaires** pour les panneaux de l’onduleur principal.

**Deux IP à ne pas confondre :** IP du **logger** dans la connexion ; IP de **l’écran** dans le navigateur, précédée de **http://**. Sans box, suivre le [mode direct au logger](MISE_A_JOUR_ECRAN_GITHUB.md#connexion-directe-au-logger-sans-box).

Pour l’assistance : version écran, référence DEYE, modèle du logger, réseau choisi, message exact et photos simultanées des deux afficheurs. Masquer mots de passe et QR d’accès. [FAQ](FAQ_ECRAN.md).
