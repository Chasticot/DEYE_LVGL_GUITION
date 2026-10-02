# VEtronic V3 — Deye 12K-SG02LP1 + WB01

Cette version réunit les menus et fonctions V3 avec le pilotage de la WB01
par sa passerelle ESP32 compatible.

- Arrêt, charge immédiate 6–32 A selon la limite annoncée par la passerelle,
  solaire et **Rendre la main à la borne**.
- Le retour à la borne laisse la WB01 autonome, annule la reprise tarifaire
  locale et permet de reprendre le pilotage à tout moment.
- Protection SOC solaire avec seuils MIN/MAX et vérification par relecture.
- Menus Deye/PV/GEN, relais GPIO40, veille horaire, Tempo/HC, configuration,
  sauvegardes JSON et commandes de recharge sur le Web de l'écran.
- Restrictions tarifaires optionnelles pour les charges manuelles lancées
  depuis l'écran ou son Web pendant le démarrage actuel.
- Courant mesuré et puissance estimée clairement distingués de la consigne.

## Téléchargements

- **Écran déjà équipé d'un partitionnement OTA compatible** :
  `DEYE_12KSG02LP1_VETRONIC_V3_4.3.4_OTA.bin` dans le formulaire OTA de l'écran.
- **Installation USB complète Windows** : extraire entièrement
  `DEYE_12KSG02LP1_VETRONIC_V3_4.3.4_Installation_Windows.zip`, puis lancer
  `INSTALLER.bat`. Le pack contient esptool, les quatre binaires, le guide et
  les empreintes ; aucun Arduino IDE ni Python à installer.
- Guide : `GUIDE_VETRONIC_V3.md`.
- Intégrité : `SHA256SUMS-VETRONIC_V3.txt`.

Ces binaires sont destinés à **l'écran GUITION ESP32-S3 480 × 480 du projet**,
avec PSRAM OPI et partition Minimal SPIFFS 4 Mo. Ils ne mettent pas à jour
la passerelle ESP32 de la borne. Sauvegarder les réglages avant installation.

Au premier lancement, activer **VE TRONIC / WB01** et régler l'adresse IP de
la passerelle. Cette édition utilise le profil SG02LP1 et ses réglages V3 ;
elle ne possède pas de sélecteur de modèles.

Compilation et tests logiciels réussis. Le pack USB est contrôlé sans port
série ; les commandes WB01 et les automatismes restent à vérifier sur le
matériel. Aucun téléversement ni essai de charge réel n'a été effectué lors
de cette publication.

[Guide complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/GUIDE_VETRONIC_V3.md)
· [Validation](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/main/docs/VALIDATION_VETRONIC_V3.md)
