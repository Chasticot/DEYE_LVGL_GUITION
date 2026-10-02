# Protocoles Modbus Deye — recherche du 1er octobre 2026

Copies de documents constructeur distribues publiquement par des tiers.
Le nom du fichier ne garantit ni sa revision interne ni son application
a chaque suffixe commercial. Les numeros de page ci-dessous commencent a 1.

## Documents conserves

| Fichier local | Source | Contenu |
|---|---|---|
| [Deye_Modbus_V118.pdf](Deye_Modbus_V118.pdf) | [PDF source](https://github.com/user-attachments/files/16597960/Deye.Modbus.protocol.V118.pdf) | 43 pages ; sections hybrides monophases, string et micro-onduleurs. Historique visible jusqu'a V117. |
| [Deye_Modbus_V105.4_20240814.pdf](Deye_Modbus_V105.4_20240814.pdf) | [PDF source](https://github.com/user-attachments/files/22990113/MODBUS.RTU.V105.4-20240814.pdf) | 48 pages ; triphase BT/HT. Attention : historique interne jusqu'au 13 janvier 2025, malgre le nom date 20240814. |
| [Deye_SG01HP3_V104.3.1.pdf](Deye_SG01HP3_V104.3.1.pdf) | [PDF source SG01-HP3-AM2](https://github.com/user-attachments/files/16597916/MODBUSRTU.V104.3.1.1111_SG01-HP3-AM2.pdf) | 52 pages ; version partagee pour SG01HP3, distinction L/H dans les tableaux. |
| [Deye_Single_Phase_TPE_2025.pdf](Deye_Single_Phase_TPE_2025.pdf) | [PDF source](https://tpenergy.com.vn/wp-content/uploads/2025/10/Single-Phase-Inverter-Modbus-RTU-Protocol.pdf) | 49 pages ; autre edition monophasée, date du chemin d'hebergement distincte de la revision du protocole. |

Les trois premiers liens sont repertories dans la [documentation ha-solarman](https://github.com/davidrapan/ha-solarman/wiki/Documentation).
Le distributeur publie aussi un [protocole triphase de 48 pages](https://tpenergy.com.vn/wp-content/uploads/2025/10/Triple-Phase-Inverter-Modbus-RTU-Protocol.pdf), avec une pagination differente du fichier V105.4 conserve ici.

## Releves utiles pour la v3

| Mesure | Protocole monophasé V118 | Protocole triphasé V105.4 |
|---|---|---|
| Energie GEN du jour | R62, 0,1 kWh, p. 10 | R536, 0,1 kWh, p. 31 |
| Consommation du jour | R84, 0,1 kWh, p. 12, ligne Hybrid | R526, 0,1 kWh, p. 30 |
| Puissance PV | R186–189, 1 W, p. 20 | R672–675, L : 1 W / H : 10 W, p. 37 |
| Tension batterie | R183, 0,01 V, p. 20 | R587, L : 0,01 V / H : 0,1 V, p. 33 |

V118 : R169 (reseau) et R178 (charge) sont en watts directs, p. 19–20.
Cela confirme les coefficients 0,1 des nouveaux profils LP1 apres le
facteur historique x10 du decodeur. Le SG02 conserve actuellement le
comportement v2 : sa calibration doit etre confrontee aux valeurs LCD.

V104.3.1 : la page 36 confirme les adresses PV672–675 et les unites L/H.
La page 32 contient une ambiguite typographique pour R590 : H:1W et
H:10W. Elle ne suffit pas, seule, a justifier une conversion batterie.
Le profil HP3 conserve donc sa conversion deja utilisee dans le projet.

## Points encore ouverts

- **GEN Daily corrige** : v2 4.2.1 et v3 4.3.1 utilisent R62 en LP1,
  coefficient par defaut 0,1 kWh. Les anciens R536 sauvegardes sont corriges
  sur les profils LP1 disponibles ; R536 reste valide pour LP3/HP3.
  L'estimation (adresse 0) et les autres adresses personnalisees sont conservees.
- **SUN-6K-SG06LP1** : aucun des PDF de protocole consultes ne nomme
  explicitement cette reference. Les [manuels constructeur SG06LP1](https://www.deyeinverter.com/download/product-manual/)
  trouves decrivent installation et interfaces ; ils ne fournissent pas
  la correspondance complete des registres recherchee. R84 est un candidat
  de la famille LP1 pour Daily Load, pas une confirmation propre au SG06.
  Le profil reste indisponible dans la v3.
- **AI-W5.1 P1** : après absence de données avec les adresses 600,
  l’utilisateur confirme le 2 octobre 2026 le fonctionnement de la cartographie
  monophasée de la variante autonome 4.3.1. Elle est reprise dans la V3 4.3.3.
  Ce retour ne constitue pas une validation de toutes les révisions AI-W5.1.

Les PDF decrivent des cartographies, pas une liste exhaustive de modeles
compatibles. Pour lever le doute SG06, il manque une table correspondant
au suffixe exact et au firmware, ou des lectures comparees au LCD.
