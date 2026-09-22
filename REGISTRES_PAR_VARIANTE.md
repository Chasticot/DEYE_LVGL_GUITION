# Registres Deye par variante

Ce tableau compare les **adresses configurées par défaut dans le code**, pas les éventuelles valeurs personnalisées enregistrées dans l'écran. *L'italique signifie que la valeur n'a pas encore été validée en conditions réelles, ou que son usage est particulier (PV3 du SG05LP3).* Le tiret signifie que la fonction n'est pas prévue dans cette variante.

| Mesure ou fonction | 12K-SG02LP1 | 12K-SG05LP3 | 25K-SG01HP3 | AI-W5.1 ESS¹ | VETRONIC² |
| --- | ---: | ---: | ---: | ---: | ---: |
| Puissance PV1 | 186 | 672 | 672 | *672* | 186 |
| Puissance PV2 | 187 | 673 | 673 | *673* | 187 |
| Puissance PV3 | 188 | *674*³ | 674 | — | 188 |
| Puissance PV4 | — | — | 675 | — | — |
| Production PV du jour | 108 | 529 | 529 | *529* | 108 |
| Batterie : SOC | 184 | 588 | 588 | *588* | 184 |
| Batterie : tension | 183 | 587 | 587 | *587* | 183 |
| Batterie : puissance | 190 | 590 | 590 | *590* | 190 |
| Batterie : température | 182 | 586 | 586 | *586* | 182 |
| Réseau : puissance | 169 | 625 | 625 | *607* | 169 |
| Réseau : état | 194 | 552 | 552 | *552* | 194 |
| Achat réseau du jour | 76 | 520 | 520 | *520* | 76 |
| Vente réseau du jour | 77 | 521 | 521 | *521* | 77 |
| Consommation : puissance | 178 | 653 | 653 | *637* | 178 |
| Consommation du jour | 84 | 526 | 526 | *526* | 84 |
| Puissance du port GEN | 166 | 667 | 667 | — | 166 |
| Température DC | 90 | 540 | 540 | *540* | 90 |
| Température AC | 91 | 541 | 541 | *541* | 91 |
| SmartLoad : état | 195, bit 0 | 552, bit 3 | 552, bit 3 | — | 195, bit 0 |
| Charge VE : mode | 489⁴ | Désactivé | Désactivé | Désactivé | 489⁴ |
| Charge VE : puissance maximale | 490⁴ | Désactivé | Désactivé | Désactivé | 490⁴ |

**Validation en conditions réelles.** Selon le retour du mainteneur, les projets 12K-SG02LP1, 12K-SG05LP3, 25K-SG01HP3 et VETRONIC fonctionnent sur de vrais onduleurs. Les registres du projet 25K-SG01HP3 fonctionnent aussi sur un **20K-SG01HP3**. Les valeurs de ces quatre variantes sont donc affichées sans italique ; cela décrit le fonctionnement constaté du projet, sans prétendre qu'un essai indépendant a été réalisé pour chaque registre et chaque révision matérielle. Le [manuel Deye couvre les puissances 5 à 25 kW de la famille SG01HP3](https://www.deyeinverter.com/deyeinverter/2024/02/03/rand/1391/instructions_sun-5-25k-sg01hp3-eu-am2_240203_en.pdf).

**AI-W5.1 ESS n'a pas encore été testé en réel.** Toutes ses adresses restent en italique.

¹ Le projet AI-W5.1 ESS existe actuellement en local, mais son firmware n'est pas encore présent dans ce dépôt GitHub. Cette colonne documente son état de développement sans annoncer une version publiée.

² La variante VETRONIC ajoute une intégration distincte, mais utilise les mêmes registres **Deye** que le 12K-SG02LP1 ; les données propres à la borne VETRONIC ne figurent pas dans ce tableau.

³ Le registre PV3 appartient à la cartographie générique, alors que le SG05LP3 est décrit dans le code comme ayant deux MPPT. Son utilité sur ce modèle reste donc à vérifier.

⁴ Les registres VE 489/490 ne sont interrogés que si l'option charge VE est activée ; elle est désactivée par défaut.

Les adresses proviennent des fichiers `registres.h` des variantes [12K-SG02LP1](firmware/DEYE_LVGL_GUITION_12KSG02LP1/registres.h), [12K-SG05LP3](firmware/DEYE_LVGL_GUITION_12KSG05LP3/registres.h), [25K-SG01HP3](firmware/DEYE_LVGL_GUITION_25KSG01HP3/registres.h) et [VETRONIC](firmware/DEYE_LVGL_GUITION_VETRONIC/registres.h). Le profil AI-W5.1 ESS est relevé du projet local et devra être revérifié lors de sa publication.
