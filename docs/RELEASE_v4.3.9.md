# Firmware unifié 4.3.9 — correction du réveil spontané en veille

Un défaut logiciel pouvait **rallumer l’écran pendant 60 secondes sans toucher le tactile**. Une lecture d’heure avec un délai de zéro échouait parfois au changement de milliseconde : l’écran quittait alors la veille, puis accordait une nouvelle minute de réveil.

La 4.3.9 lit directement l’heure système, sans cette course au délai. La même correction s’applique aux lectures d’heure de luminosité, de tarifs et d’énergie journalière GEN.

Le réveil tactile demande désormais un bref contact confirmé : au moins trois trames valides sur 80 ms. Un contact isolé, une coordonnée hors écran ou une erreur d’acquittement I2C ne suffit plus à réveiller l’écran. **Maintenir brièvement le doigt sur l’écran pour le réveiller** ; un tap très court peut nécessiter un second appui. Les commandes sur l’écran déjà allumé restent immédiates, et le geste de réveil n’actionne pas un bouton.

Les traces série indiquent le réveil tactile confirmé, la sortie de la plage de veille ou l’heure indisponible, ainsi que le retour en veille. Les indicateurs de recharge rétablis en 4.3.8, GEN MO/PV5 et la vérification des versions GitHub restent inclus.

## Télécharger et mettre à jour

- [Firmware OTA 4.3.9](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.9/DEYE_V3_4.3.9_OTA.bin).
- [Pack Windows complet](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.9/DEYE_V3_4.3.9_Installation_Windows.zip).
- [Guide 4.3.9](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.9/GUIDE_UNIFIE_4.3.9.md).
- [Empreintes SHA-256](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/download/v4.3.9/SHA256SUMS-4.3.9.txt).

Sauvegarder la configuration, puis sélectionner le fichier OTA dans la page Web de l’écran. Depuis 4.3.5 à 4.3.8, une mise à jour compatible sans effacement conserve les réglages et les choix de modèle et de borne. Depuis V3 ou Vetronic V3, sélectionner explicitement la borne après la migration initiale. Le firmware de la passerelle WB01 reste séparé.

Le défaut est reproduit en test logiciel. Sa responsabilité dans le retour utilisateur et le confort du filtre tactile restent à confirmer sur l’écran concerné. Aucun flash ni essai réel de charge effectué. Les résultats de compilation, des onze tests C++, des tests Web et du pack sont consignés dans [la validation](https://github.com/Chasticot/DEYE_LVGL_GUITION/blob/v4.3.9/docs/VALIDATION_4.3.9.md).
