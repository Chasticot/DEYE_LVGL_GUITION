# Deye Monitor 4.9.2 - notes utilisateur du 16 novembre 2026

La **4.9.2 est publiée** : [firmware OTA et empreinte SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.9.2). Les menus et fonctions de la 4.9.1 sont conservés.

## Ce qui change

- Aucune licence ni activation commerciale n'est requise pour la série 4.
- La sauvegarde JSON inclut maintenant les quatre passerelles PV5-PV8, leurs clés et jetons, ainsi que la famille Sofar et les inversions de signe.
- Les configurations de passerelles sont vérifiées ensemble avant enregistrement. Les anciens exports restent acceptés ; l'absence de ces nouveaux champs ne remplace pas les réglages déjà présents.
- Le fonctionnement du Time of Use au démarrage est corrigé.
- Les pages Web sont davantage compressées, avec les mêmes fonctions. Le partitionnement OTA reste inchangé.

## Sauvegarder et installer

1. Exporter la configuration depuis la page Web avant une mise à jour ou un changement important.
2. **Garder ce fichier privé** : il peut contenir les clés et jetons des passerelles. Il ne contient ni le mot de passe Wi-Fi ni les identifiants Web, ni les CSV de la carte SD.
3. Sur l'écran : **Réglages > Système > Mise à jour > Vérifier sur GitHub > Installer**. Sinon, télécharger `DEYE_V3_4.9.2_OTA.bin` et l'envoyer depuis le formulaire OTA de la page Web de l'écran.
4. Après redémarrage, contrôler la version, les mesures, la borne et la veille.

Une OTA compatible conserve les réglages et ne demande aucun effacement. Le fichier OTA ne remplace pas une première installation USB. Ne pas envoyer ce firmware à la borne ou à l'onduleur.

## Vérifications et limites

Compilation et tests logiciels complets réussis : sauvegardes, migrations, protocoles, navigation Web et écrans tactiles simulés. Aucun appareil réel n'a été flashé pendant cette préparation ; les nouvelles intégrations et la veille restent à confirmer sur les révisions matérielles des utilisateurs.

OTA : **1 949 136 octets**. SHA-256 : `1722fcd4a60c3977519ca5ae1c14b052e44364556164b0bc072808e1a182a691`.

[Guide illustré](GUIDE_UTILISATEUR_V3.md) · [Mémo](RECAP_UTILISATEURS_V3.md) · [V2C Trydan / Pro](GUIDE_V2C_TRYDAN_BETA.md) · [Tesla Gen 3 Wi-Fi](GUIDE_TESLA_GEN3_WIFI.md) · [Veille et réveil tactile](GUIDE_VEILLE_HORAIRE.md).
