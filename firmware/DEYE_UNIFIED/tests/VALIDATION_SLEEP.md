# Réveil spontané en veille — 6 octobre 2026

Retour utilisateur : firmware 4.3.7, rallumage aléatoire pendant environ une minute sans toucher l'écran. Le code de veille concerné est également présent dans la 4.3.8.

## Cause reproduite

La version installée d'Arduino-ESP32 (2.0.17) initialise un compteur dans `getLocalTime`, puis vérifie le délai avant de lire l'heure. Avec un délai de zéro, un changement de milliseconde entre ces deux opérations fait retourner `false` sans consulter l'heure système, même si celle-ci est correcte.

`display_sleep_due()` interprétait ce résultat comme une sortie de la plage de veille. Le rétroéclairage se rallumait immédiatement. La lecture suivante retrouvait la plage de veille et `V2SleepState` accordait une nouvelle période de réveil, de 60 secondes par défaut. La synchronisation NTP peut rester valide pendant toute cette séquence.

`clock_local_time.h` lit désormais l'horloge système et convertit l'heure locale une seule fois, sans délai. Il conserve le contrôle d'année d'Arduino-ESP32 et les conditions de synchronisation NTP des appelants. Les lectures non bloquantes de luminosité, tarifs et énergie utilisent également cette fonction pour éviter la même erreur.

## Protection tactile complémentaire

Pendant la veille uniquement, le réveil demande au moins trois trames tactiles valides sur au moins 80 ms, sans intervalle supérieur à 120 ms entre trames. Une interrogation sans nouvelle trame ne confirme pas un contact et ne vaut pas un relâchement. Un relâchement explicite, une trame invalide ou une erreur I2C annulent la confirmation. Un tap très bref peut donc nécessiter un second appui légèrement plus long pour réveiller l'écran.

Le pilote rejette les nombres de contacts supérieurs à cinq et les coordonnées hors du panneau 480 × 480, au lieu de les transformer en contacts sur les bords. Une trame dont l'acquittement I2C échoue n'est pas transmise comme un nouveau contact. Le geste de réveil reste absorbé pour éviter d'actionner une commande. Les commandes sur l'écran déjà allumé conservent leur traitement immédiat.

Les traces série `[DISPLAY]` distinguent le réveil tactile confirmé, la sortie de plage de veille ou l'heure indisponible, et l'expiration de la durée de réveil.

## Vérification

- `clock_sleep_test.cpp` reproduit le défaut de délai zéro et la nouvelle minute de réveil. Il vérifie ensuite la veille stable, la sortie horaire, le réveil manuel et les horloges invalides avec le code de lecture corrigé.
- `touch_wake_test.cpp` vérifie les trames GT911, les coordonnées limites et corrompues, les contacts isolés, le relâchement, les erreurs, les interrogations sans données, le réveil confirmé, sa durée et le débordement de `millis()`.
- Les onze exécutables C++ et les deux tests Web réussissent avec `tests/run_checks.ps1 -SkipPreview`.
- Compilation PlatformIO du firmware unifié : SUCCESS. RAM statique : 60 520 / 327 680 octets ; application : 1 540 441 / 1 966 080 octets.

Le défaut logiciel est reproduit en test, mais sa responsabilité dans le retour de cet utilisateur et le confort du filtre tactile restent à vérifier sur son écran. Aucun flash effectué. Ce correctif est intégré à la publication 4.3.9 ; les résultats des livrables sont consignés dans `docs/VALIDATION_4.3.9.md` du dépôt de publication.
