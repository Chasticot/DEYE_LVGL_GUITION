# Firmware unifié 4.3.6 — GEN MO comme PV5

Lorsque **GEN MO** et **Cumuler GEN MO + PV** sont sélectionnés, GEN s'ajoute aux PV visibles pour la puissance instantanée et la production du jour.

La 4.3.5 ignorait la puissance GEN corrigée lorsqu'elle était négative. Le correctif utilise sa valeur positive de production après calibration, avec les deux conventions de signe. L'écran, le Web et l'historique partagent le même total. Le compteur journalier GEN reste utilisé lorsqu'il est configuré ; son estimation par la puissance utilise aussi cette correction.

Exemple : PV 2 000 W + GEN 750 W = **2 750 W** ; PV 12,3 kWh + GEN 4,5 kWh = **16,8 kWh**. SmartLoad, le cumul désactivé et les profils sans GEN excluent GEN des deux totaux. Aucun registre PV5 physique n'est ajouté.

## Téléchargements

- [Firmware OTA 4.3.6](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.6/DEYE_V3_4.3.6_OTA.bin).
- [Pack d'installation Windows](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.6/DEYE_V3_4.3.6_Installation_Windows.zip).
- [Guide unifié 4.3.6](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.6/GUIDE_UNIFIE_4.3.6.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.6/SHA256SUMS-4.3.6.txt).

Sauvegarder la configuration avant la mise à jour. Depuis 4.3.5, le modèle, la borne et les réglages sont conservés avec une mise à jour compatible sans effacement. Vérifier **GEN MO** et **Cumuler GEN MO + PV**, enregistrer si nécessaire et laisser redémarrer. La migration initiale depuis V3/Vetronic V3 demande toujours un choix explicite de borne.

Le firmware concerne l'écran GUITION du projet. La passerelle WB01 garde son propre firmware et sa propre régulation solaire.

Compilation PlatformIO et huit exécutables hôte, dont le nouveau test GEN MO/PV5, ainsi que les tests Web réussis. L'image ESP32-S3 et le pack Windows ont été contrôlés. **Aucun flash ni essai matériel n'a été effectué** ; comparer les deux totaux aux mesures de l'installation.

Les sources sont dans `firmware/DEYE_UNIFIED/`, environnement `deye_unified`. La [release 4.3.5](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/tag/v4.3.5) reste accessible.
