# Registres Deye par variante

Ce tableau compare les **adresses configurées par défaut dans le code**, pas les éventuelles valeurs personnalisées enregistrées dans l'écran. *L'italique signifie que l'adresse reste à confirmer sur le modèle concerné.* Le tiret signifie que la fonction n'est pas prévue dans cette variante.

| Mesure ou fonction | 12K-SG02LP1 | 12K-SG05LP3 | 25K-SG01HP3 | AI-W5.1 ESS¹ | VETRONIC² |
| --- | ---: | ---: | ---: | ---: | ---: |
| Puissance PV1 | 186 | 672 | *672* | *672* | 186 |
| Puissance PV2 | 187 | 673 | *673* | *673* | 187 |
| Puissance PV3 | 188 | *674*³ | *674* | — | 188 |
| Puissance PV4 | — | — | *675* | — | — |
| Production PV du jour | 108 | *529* | *529* | *529* | 108 |
| Batterie : SOC | 184 | 588 | *588* | *588* | 184 |
| Batterie : tension | 183 | 587 | *587* | *587* | 183 |
| Batterie : puissance | 190 | 590 | *590* | *590* | 190 |
| Batterie : température | 182 | *586* | *586* | *586* | 182 |
| Réseau : puissance | 169 | *625* | *625* | *607* | 169 |
| Réseau : état | 194 | *552* | *552* | *552* | 194 |
| Achat réseau du jour | 76 | *520* | *520* | *520* | 76 |
| Vente réseau du jour | 77 | *521* | *521* | *521* | 77 |
| Consommation : puissance | 178 | 653 | *653* | *637* | 178 |
| Consommation du jour | 84 | *526* | *526* | *526* | 84 |
| Puissance du port GEN | 166 | *667* | *667* | — | 166 |
| Température DC | 90 | *540* | *540* | *540* | 90 |
| Température AC | 91 | *541* | *541* | *541* | 91 |
| SmartLoad : état | 195, bit 0 | *552, bit 3* | *552, bit 3* | — | 195, bit 0 |
| Charge VE : mode | 489⁴ | Désactivé | Désactivé | Désactivé | 489⁴ |
| Charge VE : puissance maximale | 490⁴ | Désactivé | Désactivé | Désactivé | 490⁴ |

**Niveau de validation.** Pour le 12K-SG05LP3, les adresses PV1 (672), PV2 (673), tension batterie (587), SOC (588), puissance batterie (590) et puissance de consommation (653) sont recoupées avec un [relevé réalisé sur un SUN-12K-SG05LP3-EU-SM2](https://www.photovoltaikforum.com/thread/255900-der-deye-sun-12k-sg05lp3-eu-sm2-mit-modbus-gateway-auslesen-so-funktioniert-es-b/). Le relevé lit à partir de R580 : les positions des données correspondent à ces six adresses. Cela confirme les **adresses**, pas à lui seul tous les coefficients ni les signes de puissance. Les autres valeurs SG05 restent en italique, notamment le réseau, GEN et SmartLoad.

Pour le profil SG01HP3, **l'utilisateur confirme que les registres du projet 25K fonctionnent avec un SUN-20K-SG01HP3**. Le [manuel Deye regroupe les puissances 5 à 25 kW de cette famille](https://www.deyeinverter.com/deyeinverter/2024/02/03/rand/1391/instructions_sun-5-25k-sg01hp3-eu-am2_240203_en.pdf), ce qui appuie la compatibilité, mais il ne démontre pas chaque adresse. Le détail registre par registre et le fonctionnement sur un **25K** n'ont pas été testés ici : les valeurs de cette colonne restent donc en italique. Pour l'AI-W5.1 ESS, la cartographie est explicitement provisoire : **aucune adresse n'est encore confirmée pour ce profil**. Les adresses non italiques du 12K-SG02LP1 correspondent au profil indiqué comme validé dans le code ; la variante VETRONIC reprend exactement cette cartographie Deye.

¹ Le projet AI-W5.1 ESS existe actuellement en local, mais son firmware n'est pas encore présent dans ce dépôt GitHub. Cette colonne documente son état de développement sans annoncer une version publiée.

² La variante VETRONIC ajoute une intégration distincte, mais utilise les mêmes registres **Deye** que le 12K-SG02LP1 ; les données propres à la borne VETRONIC ne figurent pas dans ce tableau.

³ Le registre PV3 appartient à la cartographie générique, alors que le SG05LP3 est décrit dans le code comme ayant deux MPPT. Son utilité sur ce modèle reste donc à vérifier.

⁴ Les registres VE 489/490 ne sont interrogés que si l'option charge VE est activée ; elle est désactivée par défaut.

Les adresses proviennent des fichiers `registres.h` des variantes [12K-SG02LP1](firmware/DEYE_LVGL_GUITION_12KSG02LP1/registres.h), [12K-SG05LP3](firmware/DEYE_LVGL_GUITION_12KSG05LP3/registres.h), [25K-SG01HP3](firmware/DEYE_LVGL_GUITION_25KSG01HP3/registres.h) et [VETRONIC](firmware/DEYE_LVGL_GUITION_VETRONIC/registres.h). Le profil AI-W5.1 ESS est relevé du projet local et devra être revérifié lors de sa publication.
