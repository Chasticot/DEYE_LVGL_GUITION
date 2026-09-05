# Integration VE — SUN-12KSG02LP1-EU-AM3

## Etat de cette version

Cette version lit et modifie les **parametres VE de l'onduleur** via son logger Solarman V5 (TCP 8899, esclave 1). Elle fonctionne pour les essais de parametrage avant installation de la borne. Elle ne mesure pas la consommation reelle d'une borne et ne deduit pas sa presence d'une reponse de l'onduleur.

- Bloc 1 : calcul historique conserve.
- Bloc 2 : base historique etendue pour inclure 259 et 260 lorsque la page VE est active. Avec les valeurs par defaut : **169–260, 92 registres**.
- Bloc 3 : **709, 1 registre**, seulement lorsque VE est actif. La plage est ajustable dans `ve_deye.h` via `DEYE_EV_BLOCK3_START` et `DEYE_EV_BLOCK3_COUNT` ; elle doit contenir 709 et rester sous 126 registres. Un elargissement doit rester sur des adresses prises en charge par le firmware, sinon le bloc entier peut etre refuse.
- Une erreur du bloc 3 ne rend pas les parametres 259/260 invalides.
- VE desactive : retour au bloc 2 historique et cycle a deux blocs, sans redemarrage.
- Si des registres personnalises rendent l'extension du bloc 2 trop grande (>125), la base est conservee et les commandes VE restent indisponibles ; un message serie l'explique.

## Signification et limites du profil

| Registre | Usage dans cette version |
|---|---|
| 259 / 0x0103 | Mode : bits 1:0 = 1 solaire, 2 libre. Les autres bits sont conserves. Les valeurs 0/3 s'affichent comme non reconnues. |
| 260 / 0x0104 | Plafond regle dans l'onduleur. Unite du profil : **1 W par unite brute**. |
| 709 / 0x02C5 | Consigne de puissance destinee a la borne, en W. **Ce n'est pas une mesure de consommation.** |

Ce profil vient d'un travail public sur le protocole Smart Devices V105.4, principalement teste sur d'autres references Deye. La presence de la page VE sur le SUN-12KSG02LP1-EU-AM3 ne prouve pas a elle seule la correspondance de toutes les unites et de tous les champs. Comparer les valeurs brutes et les modes au LCD avant le premier essai d'ecriture. L'ecran et Serial0 exposent ces valeurs pour cela.

R260 est maintenant en unite 1 W, alors que l'ancienne ebauche multipliait par 10. Si le LCD affiche 3000 W et R260 vaut 300, regler `DEYE_EV_POWER_REGISTER_SCALE_W` a 10 dans `ve_deye.h`, puis recompiler avant d'utiliser APPLIQUER. R709 peut etre nul ou indisponible en l'absence de borne ; ce n'est pas a lui de confirmer l'enregistrement du plafond R260.

Le plafond utilisateur est independant de R260 : **7400 W pour l'installation monophasee actuelle**, configurable via `DEYE_EV_INSTALLATION_MAX_POWER_W`. Il ne constitue pas une detection automatique du calibre electrique ou du modele de borne. Le curseur propose des pas de 10 W ; la saisie utilise l'unite du profil. Les nouvelles consignes sont bornees a 1400–7400 W. Une valeur lue hors de cette plage reste affichee telle quelle et n'est jamais reecrite automatiquement.

## Utilisation

1. Activer **Activer la page VE** dans Tempo / VE et sauvegarder. Il n'est pas necessaire d'avoir installe la borne.
2. Ouvrir l'icone voiture du tableau de bord. `Lim:` y designe la consigne R709, pas une mesure de charge.
3. Comparer **PLAFOND REGLE** et **Mode lu** au LCD de l'onduleur. Les nombres bruts R259, R260 et R709 restent visibles.
4. Modifier le plafond ou choisir Solaire uniquement / Libre, puis **APPLIQUER**. Le changement du curseur seul n'envoie rien.
5. Attendre **Reglages confirmes par l'onduleur**. Le programme relit le bloc 2 pour verifier les valeurs. Le resultat ne confirme pas une charge physique.

Les commandes passent par une file d'une entree, executee par la tache Solarman. La puissance est appliquee avant le mode si les deux sont modifies. Le mode utilise une lecture recente, le remplacement des seuls bits 1:0 et une ecriture FC16. Il n'y a pas de repetition automatique d'une ecriture en cas d'ACK perdu ; une relecture peut toutefois confirmer sa prise en compte. Un resultat partiel est signale si un des reglages seulement a ete confirme. Les changements faits simultanement depuis le LCD ou une autre application peuvent encore modifier la valeur finale : les mesures suivantes font foi.

Les commandes attendues sont annulees si VE est desactive, si le reseau n'est pas disponible, ou si leur attente depasse 15 s. Les controles sont indisponibles lorsque les parametres sont invalides/perimes (45 s). Les transactions reseau ne gardent pas le mutex des donnees. L'intervalle de lecture configure reste respecte, y compris apres un echec.

Les preferences locales Tempo/VE restent sauvegardees dans NVS. Les reglages de charge sont conserves par **l'onduleur** : l'ecran les relit au demarrage et n'envoie aucune ancienne consigne locale.

## Verification sur l'onduleur, sans borne

- Avec VE desactive : verifier que les donnees historiques restent correctes et que seul le cycle de deux blocs apparait dans la console.
- Activer VE : attendre `VE bloc2 R169-R260 (92), bloc3 R709 (1)` si les registres sont ceux par defaut.
- Au LCD, changer le plafond par exemple de 3000 a 4000 W : verifier R260 et PLAFOND REGLE, meme si R709 reste nul.
- Au LCD, basculer Libre / Solaire : verifier Mode lu et les bits 1:0 de R259. Ne pas envoyer de mode si ce profil ne correspond pas aux valeurs observees.
- Depuis l'ecran, appliquer 3000 W puis 4000 W et verifier le LCD. Le curseur doit permettre de remonter au-dessus de la consigne precedente.
- Appliquer uniquement le mode : verifier que le plafond n'est pas reecrit et que le raccordement Grid/Load et le seuil SOC hors reseau restent identiques.
- Couper le Wi-Fi : les commandes doivent devenir indisponibles, puis revenir apres une lecture valide. Une valeur ancienne doit etre identifiee comme indisponible.
- Redemarrer seulement l'ecran : verifier la relecture des reglages sans ecriture spontanee.
- Verifier les deux themes, le clavier numerique, Annuler et Retour, ainsi que le message d'une saisie hors plage.

## Fonctions volontairement non actives faute de correspondance validee

- Mesures de puissance/courant/tension, etats du vehicule et defauts de la borne : necessitent la borne et son acces direct, avec le profil et l'esclave verifies.
- Programmation horaire et choix charge au branchement / programmee : registres distincts non confirmes pour cet onduleur. Ne pas les assimiler aux bits solaire/libre de R259.
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

Les tests couvrent limites des blocs, conservation des bits pour les 65536 valeurs de R259, trame FC16 connue, lectures 1/92/125 registres, troncatures, CRC, exceptions, ACK d'une autre adresse et enveloppes V5 de plus de 256 octets. Ils ne remplacent pas les essais de correspondance des registres sur l'onduleur.
