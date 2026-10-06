# Firmware unifié 4.3.8 — retour des indicateurs de recharge

Le tableau de bord retrouve le **soleil jaune** lorsque la borne confirme le mode solaire et la **coche verte** lorsque la WB01 signale le véhicule branché, même si la charge attend du surplus. Le soleil indique le mode solaire ; il peut donc rester affiché à 0 W.

Une croix rouge indique un véhicule débranché. Si les mesures WB01 deviennent indisponibles, le branchement affiche un tiret gris ; un mode périmé masque le soleil. La ligne conserve la voiture et la puissance estimée sans chevauchement, en thème clair comme sombre.

Avec Deye LoRa, le soleil suit le registre de mode récent. R489/R490 ne fournissent pas l’état de branchement : aucune coche de câble n’est inventée.

Le cumul **GEN MO + PV** en puissance instantanée et en kWh du jour ainsi que la correction de la vérification GitHub des anciennes versions suffixées restent inclus.

## Télécharger et mettre à jour

- [Firmware OTA 4.3.8](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.8/DEYE_V3_4.3.8_OTA.bin).
- [Pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.8/DEYE_V3_4.3.8_Installation_Windows.zip).
- [Guide 4.3.8](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.8/GUIDE_UNIFIE_4.3.8.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.8/SHA256SUMS-4.3.8.txt).

Sauvegarder la configuration, puis sélectionner le fichier OTA dans la page Web de l’écran. Depuis 4.3.5 à 4.3.7, une mise à jour compatible sans effacement conserve les choix. Depuis V3 ou Vetronic V3, sélectionner explicitement la borne après la migration initiale. Le firmware et le calcul solaire de la passerelle WB01 restent séparés.

Compilation PlatformIO, neuf tests C++, tests Web/versions GitHub, rendu LVGL et vérification du pack Windows réussis. Aucun flash ni essai réel de charge effectué.
