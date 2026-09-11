# Integration VE — SUN-12KSG02LP1-EU-AM3

## Etat de cette version

La cartographie specifique au SUN-12K-SG02LP1-EU-AM3 et a la borne
SUN-EVSE22K01-EU en LoRa est confirmee par comparaison LCD/DeyeCloud :

| Registre | Fonction confirmee |
|---|---|
| 489 / `0x01E9` | Solaire/libre dans les bits 1:0 de la famille active : `0x5F01` solaire, `0x5F02` libre. Desactive est une valeur distincte : `0x5A00`. |
| 490 / `0x01EA` | Puissance maximale VE, entier non signe, **1 W par unite**. Exemple observe : `3050` -> `4000` W. |

Le profil historique R259/R260/R709 provenait d'un onduleur triphase different
et n'est plus lu ni ecrit par le firmware.

La version de diagnostic n'envoie que des lectures FC03. La page Web
`/ve-probe` enregistre R0-R1023 en trois temps : AVANT, TEMOIN sans aucune
modification, puis APRES une modification faite au LCD ou dans DeyeCloud.
Les registres qui changent deja entre AVANT et TEMOIN sont des mesures
dynamiques et sont ignores ; Serial0 affiche ensuite seulement les candidats
stables qui ont change APRES. Cette comparaison est le prerequis avant toute
nouvelle ecriture.

- Bloc 1 : calcul historique conserve.
- Bloc 2 : base historique inchangee.
- Bloc 3 : **489–490, 2 registres**, seulement lorsque VE est actif.
- Une erreur du bloc 3 rend les parametres VE indisponibles, sans affecter les mesures historiques.
- VE desactive : retour au bloc 2 historique et cycle a deux blocs, sans redemarrage.

## Limites de securite

Le plafond utilisateur reste borne a **1400–7400 W** par
`DEYE_EV_INSTALLATION_MAX_POWER_W`. Le curseur propose des pas de 10 W ; une
valeur lue hors de cette plage est affichee mais jamais reecrite
automatiquement.

Pour solaire/libre, le programme preserve tous les bits de R489 sauf les deux
bits de mode. Pour desactiver, il ecrit la valeur confirmee `0x5A00`; pour
reactiver depuis celle-ci, il ecrit `0x5F01` ou `0x5F02`. Un registre R489
inconnu reste verrouille. La puissance reste modifiable dans R490. Chaque
ecriture est relue et comparee ; aucun nouvel essai automatique n'est envoye
si l'ACK est perdu.

## Utilisation

1. Activer **Activer la page VE** dans Tempo / VE et sauvegarder. Il n'est pas necessaire d'avoir installe la borne.
2. Ouvrir l'icone voiture du tableau de bord. `VE:` designe le plafond lu dans R490, pas une mesure de charge physique.
3. Comparer **PLAFOND VE (LoRa)** et **Mode lu** au LCD de l'onduleur. Les valeurs brutes R489 et R490 restent visibles.
4. Modifier le plafond ou choisir Solaire uniquement / Libre, puis **APPLIQUER**. Le changement du curseur seul n'envoie rien.
5. Attendre **Reglages confirmes par l'onduleur**. Le programme relit R489-R490 pour verifier les valeurs. Le resultat ne confirme pas une charge physique.

Les commandes passent par une file d'une entree, executee par la tache Solarman. La puissance est appliquee avant le mode si les deux sont modifies. Le mode utilise une lecture recente, le remplacement des seuls bits 1:0 et une ecriture FC16. Il n'y a pas de repetition automatique d'une ecriture en cas d'ACK perdu ; une relecture peut toutefois confirmer sa prise en compte. Un resultat partiel est signale si un des reglages seulement a ete confirme. Les changements faits simultanement depuis le LCD ou une autre application peuvent encore modifier la valeur finale : les mesures suivantes font foi.

Les commandes attendues sont annulees si VE est desactive, si le reseau n'est pas disponible, ou si leur attente depasse 15 s. Les controles sont indisponibles lorsque les parametres sont invalides/perimes (45 s). Les transactions reseau ne gardent pas le mutex des donnees. L'intervalle de lecture configure reste respecte, y compris apres un echec.

Les preferences locales Tempo/VE restent sauvegardees dans NVS. Les reglages de charge sont conserves par **l'onduleur** : l'ecran les relit au demarrage et n'envoie aucune ancienne consigne locale.

## Verification sur l'onduleur, sans borne

- Avec VE desactive : verifier que les donnees historiques restent correctes et que seul le cycle de deux blocs apparait dans la console.
- Activer VE : attendre `VE bloc2 R169-R195 (27), bloc3 R489-R490 (2)` avec les registres par defaut.
- Au LCD, changer le plafond par exemple de 3000 a 4000 W : verifier R490 et **PLAFOND VE (LoRa)**.
- Au LCD, basculer Libre / Solaire : verifier `0x5F01` <-> `0x5F02` dans R489.
- Au LCD, desactiver puis reactiver la VE : verifier `0x5A00`, puis `0x5F01` ou `0x5F02`, dans R489.
- Depuis l'ecran, appliquer 3000 W puis 4000 W et verifier le LCD. Le curseur doit permettre de remonter au-dessus de la consigne precedente.
- Appliquer uniquement le mode : verifier que le plafond n'est pas reecrit et que le raccordement Grid/Load et le seuil SOC hors reseau restent identiques.
- Couper le Wi-Fi : les commandes doivent devenir indisponibles, puis revenir apres une lecture valide. Une valeur ancienne doit etre identifiee comme indisponible.
- Redemarrer seulement l'ecran : verifier la relecture des reglages sans ecriture spontanee.
- Verifier les deux themes, le clavier numerique, Annuler et Retour, ainsi que le message d'une saisie hors plage.

## Fonctions volontairement non actives faute de correspondance validee

- Mesure instantanee de puissance VE : le registre onduleur reste a cartographier. R490 est seulement un plafond et ne doit pas etre affiche comme une consommation.
- Courant, tension, etats du vehicule et defauts de la borne : necessitent la borne et son acces direct, avec le profil et l'esclave verifies.
- Programmation horaire et choix charge au branchement / programmee : registres distincts non confirmes pour cet onduleur. Ne pas les assimiler aux bits solaire/libre de R489.
- Pause/reprise : aucun faux bouton d'arret via une consigne a zero. Un arret propre reste a verifier avec la borne et le vehicule.
- Raccordement Grid/Load et seuil SOC : preserves lors des ecritures de mode ; pas de modification dans cette interface.

## Sources

- Profil onduleur, champs et unites : https://github.com/davidrapan/ha-solarman/pull/1065/files
- Profil direct borne, retro-ingenierie : https://github.com/davidrapan/ha-solarman/pull/1073
- Retours d'essais et probleme de l'arret a 0 W : https://github.com/davidrapan/ha-solarman/discussions/731
- Manuel Deye SUN-EVSE, section 7.1 : https://www.deyeinverter.com/deyeinverter/2026/04/17/BManualSUN-EVSE11-22K01-EU-AC20260415en.pdf

## Compilation et tests

Le firmware depasse la partition applicative par defaut de 1,25 Mio. Dans Arduino IDE, choisir **USB CDC On Boot = Enabled**, **PSRAM = OPI PSRAM** et **Partition Scheme = Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**. Ce schema garde NVS a 0x9000 (taille 0x5000) ; ne pas activer un effacement complet de la flash si les preferences doivent etre conservees. Aucun televersement n'est effectue par le script de compilation.

Commande PowerShell :

```powershell
& 'C:/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe' compile --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc,PartitionScheme=min_spiffs --build-property build.psram_type=opi --build-path build_ve_check .
```

Tests purs du meme codec que le firmware, avec un compilateur C99 (ou TinyCC `tcc -run tests/ve_modbus_codec_test.c`) :

```sh
cc -std=c99 -Wall -Wextra tests/ve_modbus_codec_test.c -o ve_codec_test
./ve_codec_test
```

Les tests couvrent limites des blocs, conservation des bits pour les 65536 valeurs d'un registre de mode, trame FC16 connue, lectures 1/92/125 registres, troncatures, CRC, exceptions, ACK d'une autre adresse et enveloppes V5 de plus de 256 octets. Ils ne remplacent pas les essais de correspondance des registres sur l'onduleur.
