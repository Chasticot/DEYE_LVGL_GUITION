# Deye Monitor 4.8.7 — lecture directe Sossen en essai

Cette mise à jour ajoute la lecture locale des micro-onduleurs Sossen Wi-Fi Tuya 3.5, sans compteur externe ni Home Assistant. Une installation correspond à une source PV5, PV6, PV7 ou PV8. Le tableau de bord actuel et les menus en tuiles sont conservés.

## Compatibilité

Profils proposés : 1in1 500 W, 2in1 600/800/1000 W et 4in1 2400 W utilisant le protocole Tuya 3.5 et le format de mesures Sossen. Le choix d’une puissance ne rend pas compatible un autre protocole. La lecture est expérimentale : les essais de cette release sont logiciels, sans validation sur un écran relié à un vrai Sossen. Les confirmations communautaires concernent les projets de référence, pas ce portage ESP32.

## Mise en service

1. Mettre le Sossen et l’écran sur le même réseau local. Faire le premier essai en journée, avec le micro-onduleur alimenté. Réserver son adresse IP dans la box.
2. Ouvrir **Réglages > Installation > Sources solaires / PV5 à PV8**, puis scanner le QR code de l’écran pour ouvrir sa page Web. Sélectionner une installation libre et **Sossen > Connexion directe Tuya 3.5 (essai)**.
3. Récupérer le Device ID et la Local Key depuis une configuration existante, la plateforme Tuya ou l’[assistant Tuya Local Key](https://github.com/vineetchoudhary/tuya-local-key). Ce dernier permet une association par QR code sur ordinateur. L’écran ne réalise pas lui-même la connexion au compte Tuya. La récupération initiale demande le compte et Internet ; la lecture par l’écran reste ensuite locale.
4. Saisir l’identifiant, la clé locale de **16 caractères**, l’adresse IP et le modèle. Le formulaire accepte aussi un fichier JSON de paramètres : choisir le bon appareil dans la liste, puis vérifier le modèle et l’adresse. Le fichier est lu dans le navigateur ; seul l’appareil sélectionné est envoyé lors de l’enregistrement. Une saisie manuelle reste possible, notamment si l’outil fournit un autre format de fichier.
5. Garder **Afficher séparément**, enregistrer et attendre une lecture réussie. Le démarrage du micro-onduleur peut demander deux à trois minutes. Comparer les watts à l’application du fabricant.
6. Si nécessaire, choisir **Ajouter au total solaire**, après avoir confirmé que la production n’est déjà comptée ni par les entrées PV ni par GEN. Sinon conserver l’affichage séparé, ou choisir **Déjà mesurée par le port GEN**.

L’écran lit la puissance **AC totale** du micro-onduleur. Un 4in1 occupe un emplacement PV supplémentaire, pas quatre. Cette release n’affiche pas les quatre panneaux Sossen séparément. Les automatismes de recharge et de relais conservent leurs mesures habituelles.

## Énergie du jour

Le Sossen fournit ici un compteur cumulé, au pas de **0,1 kWh**, et non un compteur quotidien. L’écran conserve une référence par appareil et journée locale. Confirmer l’option de même journée locale et synchroniser l’heure.

Le premier jour reste incomplet. Les kWh quotidiens sont indiqués complets seulement si le passage à minuit a été observé avec une continuité de compteur et d’horloge. Si le micro-onduleur s’éteint toute la nuit, ou si minuit est manqué, ces kWh peuvent rester indisponibles : les watts continuent à fonctionner. Un reset, un retour arrière du compteur ou de l’heure et un changement de journée lié au fuseau invalident la référence concernée. Les références enregistrées sont conservées après redémarrage et OTA.

## Dépannage

- **Session refusée** : vérifier la clé et le protocole 3.5. La clé change après suppression et nouvel appairage du Sossen dans l’application. Désactiver les autres logiciels qui lisent le même appareil en local pendant l’essai.
- **Injoignable** : vérifier alimentation solaire, Wi-Fi et IP. La nuit ou pendant une coupure réseau, l’écran indique une source indisponible ; il ne suppose pas une production nulle.
- **Aucun flux récent / trame invalide** : vérifier le modèle et conserver le mode séparé. Une variante non reconnue est refusée. Les nouvelles tentatives de connexion sont espacées de 30 secondes.
- Une modification du modèle, de l’adresse ou des identifiants impose un nouvel essai séparé avant addition.

Le lecteur utilise une session chiffrée, arme le flux de télémesure et entretient la connexion. Il ne transmet aucune limite de puissance, commande de relais ou réglage réseau au Sossen. La clé reste enregistrée localement, n’est pas renvoyée par l’API de suivi et n’est pas incluse dans l’export général des réglages. Conserver ses paramètres pour un remplacement d’écran.

Les profils Shelly dédiés de la 4.8.6 restent disponibles en alternative. Les fonctions APsystems, Enphase, Hoymiles, Deye, Sofar, UTC/heure d’été, TOU et historique mémoire/microSD restent présentes.

## Projets de référence

- [sossen-ha](https://github.com/caveman2024/sossen-ha), [SOSSEN Direct](https://github.com/ritonbrunis-lab/sossen-direct), [travaux sur le 2400 W](https://github.com/Loudramin/sossen-ha).
- [TinyTuya](https://github.com/jasonacox/tinytuya), protocole Tuya 3.5.
- [Crédits et licences des références Sossen](MENTIONS_SOSSEN.md).

Les sources de l’écran restent privées. Seuls le binaire OTA, son empreinte SHA-256 et les guides utilisateur sont publiés.
