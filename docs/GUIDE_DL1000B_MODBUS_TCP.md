# DL1000B-WIFI - Activer Modbus TCP

**Deye Monitor - mode opératoire du 29 octobre 2026.** [Retour au guide](MISE_A_JOUR_ECRAN_GITHUB.md) · [Notice en PDF](pdf/GUIDE_DL1000B_MODBUS_TCP.pdf).

**Fonctionnement du DL1000B confirmé avec notre écran par un retour utilisateur.** Prévoir un DL1000B-WIFI avec firmware du logger **1.7.2 minimum**, accès local **Modbus TCP / 502** et bon profil d'onduleur. La version 1.7.2 concerne le logger, pas l'écran ni l'onduleur.

## 1. Ouvrir la page du logger

1. Sur le Wi-Fi de la maison, retrouver **l'IP du logger** dans la box. Dans le navigateur, ouvrir `http://` suivi de cette IP.
2. IP inconnue ? Rejoindre le Wi-Fi **D suivi du numéro du logger**, avec le **PWD de l'étiquette**. Accepter « sans Internet » puis ouvrir **http://10.10.100.254**.
3. Créer les identifiants Web demandés à la première connexion ; sinon utiliser ceux déjà configurés. Ils peuvent différer du mot de passe Wi-Fi de l'étiquette.

![Connexion Web : création des identifiants puis bouton Login. Extrait de la notice Deye [S1].](images/dl1000b/01-connexion-web.png)

## 2. Vérifier le firmware et l'IP

Dans **Device Status**, relever **Device serial number**, **Firmware version** et **Wireless STA mode > IP address**. L'adresse STA est celle à utiliser sur le réseau de la maison. Réserver cette IP dans la box si possible.

![Numéro et firmware en haut, IP STA en bas. Valeurs fictives de la notice Deye [S1].](images/dl1000b/02-version-ip.png)

<!-- pagebreak -->

**Firmware ancien ou inconnu, option Modbus absente ?** Utiliser la demande à Deye en dernière page, puis reprendre à l'étape 3. Minimum demandé : **1.7.2** ; des retours existent aussi en 1.7.1 [S2].

## 3. Activer le service local

Ouvrir **Advanced Settings**. **Link1 fonctionne selon le retour utilisateur transmis ; Link2 est l'exemple illustré ci-dessous.** Le numéro du lien n'est pas un réglage à saisir dans Deye Monitor.

**Si Modbus TCP fonctionne déjà sur Link1, conserver ce choix.** Si un lien sert au cloud, conserver ses réglages et utiliser l'autre s'il propose Modbus TCP [S4]. Il n'est pas nécessaire d'activer les deux liens en Modbus TCP.

1. Dans le lien retenu (**Link1 ou Link2**, selon la configuration), cocher **Enable**.
2. Dans **Protocol** (écrit **Portocol** sur la capture), choisir **Modbus Tcp**.
3. Dans **Mode**, choisir **Default**. Sur la capture, **IP/URL address** et **Port** sont grisés : ne rien y saisir. Le port **502** se renseignera dans Deye Monitor.
4. Appuyer sur **Confirm**. Le logger redémarre automatiquement ; attendre son retour puis se reconnecter.

![Capture réelle de l'interface Link2 publiée par Henris [S2] : Enable, Modbus Tcp, Default, puis Confirm. Recadrage sans modification des réglages affichés ; firmware illustré 1.7.1_crypt, présentation susceptible de varier en 1.7.2.](images/dl1000b/03-link2-modbus-tcp.png)

Rouvrir **le lien configuré** et contrôler que les choix sont conservés, puis vérifier les lectures sur l'écran et le cloud s'il est utilisé. L'absence de Link2 seule ne justifie pas une mise à jour si Modbus TCP fonctionne sur Link1.

<!-- pagebreak -->

## 4. Relier l'écran et vérifier les mesures

Reconnecter le téléphone au Wi-Fi de la maison si l'accès AP a été utilisé. L'écran et le logger doivent pouvoir communiquer sur le même réseau local.

1. Sur l'écran, ouvrir **DEYE / SOLARMAN > MON ONDULEUR**, choisir la puissance et la référence exacte, puis appliquer et laisser redémarrer si nécessaire.
2. Ouvrir **DEYE / SOLARMAN > CONNEXION / LOGGER** et renseigner :

| Champ | Valeur |
| --- | --- |
| Type de logger | **DL1000B - Modbus TCP** |
| Adresse IP | **IP STA du logger** relevée à l'étape 2, sans `http://` |
| Numéro de série | Celui du **logger**, lettres comprises ; différent du numéro de l'onduleur |
| Port | **502** |
| Unité Modbus | Adresse configurée sur l'onduleur ; souvent 1, à vérifier sur l'installation |

3. Saisir directement l'IP connue, puis **SAUVEGARDER** et attendre le redémarrage. La recherche est facultative. Si elle indique **numéro non vérifié**, identifier l'appareil dans **RESULTATS** avant de le choisir.
4. Vérifier l'arrivée et l'actualisation des mesures. Comparer SOC, tension et puissances à l'afficheur de l'onduleur. Contrôler que les compteurs de lectures OK progressent pour les blocs utilisés.

**Installation vérifiée :** réglages du lien choisi conservés après redémarrage, mesures actualisées et cohérentes sur l'écran, puis reprise des lectures après un redémarrage normal de l'écran. Les fonctions de commande doivent être vérifiées séparément si elles sont utilisées.

### Si les mesures n'arrivent pas

| Symptôme | Action |
| --- | --- |
| Link2 absent | Vérifier si **Link1** propose Modbus TCP ou fonctionne déjà. Son utilisation est rapportée par un utilisateur. |
| Aucun lien ne propose Modbus Tcp | Demander à Deye un firmware adapté, **1.7.2 minimum**, et l'ouverture de l'accès local. |
| Cloud OK, écran sans mesures | Revoir le lien choisi, **Confirm**, IP STA et port **502**. Le cloud et Modbus local sont distincts. |
| IP introuvable dans la recherche | Saisir l'IP connue de la box ; la découverte par numéro n'est pas une condition de lecture. |
| Liaison perdue après changement d'IP | Relever la nouvelle IP et créer une réservation DHCP dans la box. |
| Logger joignable, mesures absentes ou fausses | Contrôler unité Modbus et référence exacte de l'onduleur. |

La procédure illustrée concerne le **DL1000B-WIFI**. Ne pas l'étendre au DL1000B-4G ; pour un DL1000B-ETH, vérifier sa notice et son interface propres.

<!-- pagebreak -->

## 5. Demander la mise à jour à Deye

Si le logger est trop ancien ou si l'accès local manque, envoyer ce message à **service@deye.com.cn**, adresse figurant dans la notice Deye [S1], ou passer par son installateur. Remplacer les champs entre crochets ; joindre une photo de l'étiquette et de la version actuelle **en privé**. Pour une intervention à distance, laisser le logger alimenté et connecté à Internet et suivre les consignes du support.

**Objet : DL1000B-WIFI - Request logger firmware 1.7.2 or newer and local Modbus TCP on port 502**

Hello Deye Support,

I use a Deye inverter with a DL1000B-WIFI data logger and would like to read its measurements locally with a Deye Monitor display using standard Modbus TCP on port 502.

Please remotely update the data logger to firmware **DYDA_WiBLE 1.7.2 or a newer version compatible with my hardware**, and enable or unlock local Modbus TCP access if required. Please confirm which link to use (Link1 or Link2) with Enable, Protocol = Modbus Tcp and Mode = Default, while preserving Deye Cloud if used.

My request concerns the data logger firmware. Please let me know before any inverter firmware update is needed.

- Country: [country]
- Data logger model: DL1000B-WIFI
- Data logger serial number: [logger SN]
- Current logger firmware, including suffix: [full version]
- Inverter model: [full model]
- Inverter serial number: [inverter SN]
- Contact email: [email]

Please confirm the firmware installed and any local activation steps after the update. The logger can be kept online for the remote update.

Thank you for your help.

Best regards,

[name]

**Après l'intervention :** relever la version réellement affichée, reprendre **le lien retenu > Enable > Modbus Tcp > Default > Confirm**, puis vérifier les mesures dans Deye Monitor. Une demande envoyée ne signifie pas que la mise à jour a été effectuée.

### Sources et captures

- **[S1]** [Deye, DL1000B-WIFI Quick Guide V1.0, page 2](https://www.deyeinverter.com/deyeinverter/2026/05/20/DeyeDataLoggerQuickGuideDL1000B-WIFIV10.pdf) : accès Web, identifiants, Device Status, IP et contact. Images 01 et 02 : extraits de l'interface illustrée dans cette notice, pas des captures de notre installation.
- **[S2]** [Henris, témoignage et capture Link2, 16 septembre 2026](https://community.home-assistant.io/t/home-assistant-deye-inverters-data-loggers/942625/18) : activation et lecture sur 502 avec firmware 1.7.1_crypt. Image 03 : sa capture, recadrée en bas pour retirer une note tronquée.
- **[S3]** [Retours après mise à jour 1.7.2](https://github.com/davidrapan/ha-solarman/discussions/1159) : Home Assistant. Retour transmis le **29 octobre 2026** : fonctionnement avec **notre écran**, et fonctionnement rapporté sur **Link1**, sans relevé détaillé du firmware.
- **[S4]** [adyannu, 9 octobre 2026](https://community.home-assistant.io/t/connecting-to-the-new-deye-logger-to-home-assistant-using-esphome/997672/21) : Modbus TCP sur Link2 et Deye Cloud conservé sur Link1, firmware 1.7.2. C'est un exemple de répartition, pas une obligation.
