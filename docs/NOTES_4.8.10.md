# Version 4.8.10 — correction des seuils Tempo

L’exemple de recharge anticipée utilise désormais **« moins de »**, avec les règles dans cet ordre :

| Règle | Production solaire prévue | Charger jusqu’à |
|---|---|---|
| 1 | Moins de 10 kWh | 90 % |
| 2 | Moins de 20 kWh | 60 % |
| 3 | Moins de 30 kWh | 30 % |

**La première règle remplie est prioritaire dans cet exemple.** Ainsi, une prévision de 5 kWh sélectionne 90 %, même si les trois conditions sont remplies. À 15 kWh, la cible est 60 % ; à 25 kWh, elle est 30 %.

Les comparaisons sont strictes : à exactement 10 kWh, la deuxième règle donne 60 % ; à exactement 20 kWh, la troisième donne 30 %. À partir de 30 kWh, cet exemple conserve le TOU habituel.

## Appliquer l’exemple corrigé

Après la mise à jour, ouvrir sur la page Web **Pilotage → Time of Use · batterie**, puis :

1. Appuyer sur **Exemple Tempo**. Le bouton remplit le brouillon avec les trois signes « inférieur à », les seuils 10/20/30 kWh et les cibles 90/60/30 %.
2. Vérifier **Première règle remplie** dans le choix de priorité.
3. Vérifier l’interrupteur d’activation et sauvegarder si ce comportement est souhaité.

Le bouton d’exemple n’active pas l’option et n’enregistre rien automatiquement. Il décoche le repli vers une cible Tempo fixe : si aucun seuil ne correspond, le TOU habituel s’applique. Il reste possible de choisir explicitement une cible fixe de repli en mode Tempo.

Les mêmes seuils et la même priorité sont proposés par **Exemple sans Tempo** : la condition porte alors sur la prévision solaire et la charge reste limitée aux heures creuses personnelles. Le choix de priorité est aussi disponible sur l’écran, dans **Réglages → Pilotage → Batterie / anticipation → Si plusieurs règles**.

## Conservation des anciens réglages

Les configurations déjà enregistrées en **4.8.9** gardent leurs signes, leurs valeurs et leur ancienne priorité à la **dernière règle remplie**. La mise à jour ne transforme pas silencieusement un programme de charge personnalisé. Pour remplacer l’ancien exemple par celui corrigé, utiliser le bouton **Exemple Tempo**, vérifier puis sauvegarder.

Les anciens exports JSON sans choix de priorité sont relus avec leur comportement d’origine. Les nouveaux exports contiennent la priorité. Les installations provenant de la 4.8.8 conservent leur activation et leur cible fixe. La migration ne réécrit pas la mémoire à chaque démarrage.

La conservation du TOU initial, sa restauration, les limites de trois démarrages par jour préparé et les protections des mémoires restent en place. Le tableau de bord et la gestion SD ne changent pas. [Guide de ces fonctions en 4.8.9](NOTES_4.8.9.md).

## Validation

Tests logiciels : 0, 5, 9,9, 10, 15, 20, 25, 30 et 35 kWh, priorité des règles, migration 4.8.9, anciens/nouveaux exports JSON, application et retour TOU simulés, formulaires Web et menus LVGL. Compilation ESP32-S3 vérifiée. Aucun essai physique d’un cycle de charge n’a été réalisé pendant cette correction.
