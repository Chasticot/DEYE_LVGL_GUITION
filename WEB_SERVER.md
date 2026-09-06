# Serveur Web et OTA

Ouvrir http://ADRESSE_IP_DE_LECRAN/ depuis le reseau local.
Le serveur demarre avec le firmware. Aucun service externe n'est requis.

## Authentification

- Desactivee par defaut lorsque la configuration Web est absente.
- Menu Web « Authentification serveur Web » : saisir un identifiant et un mot de passe, cocher l'activation et sauvegarder.
- Un mot de passe laisse vide conserve celui deja enregistre.
- Decocher l'activation desactive la protection et conserve les identifiants pour une prochaine activation.
- Sur l'ecran : Configuration > Wi-Fi > « Reset authentification serveur Web » > SAUVEGARDER.
  Cette operation efface l'enregistrement des identifiants Web, desactive l'authentification et redemarre.
  Elle fonctionne sans scan Wi-Fi et conserve la connexion Wi-Fi enregistree ; les modifications Wi-Fi du formulaire sont ignorees dans ce mode de recuperation.
- La case est decochee chaque fois que le menu Wi-Fi est ouvert.
- Une erreur d'effacement affiche un message et empeche le redemarrage.

La protection utilise l'authentification HTTP du serveur ESP32 et couvre les pages, les sauvegardes et l'OTA. Les formulaires utilisent un jeton de session contre les requetes provenant d'autres sites. Le serveur est prevu pour le reseau local ; HTTP ne chiffre pas les echanges.

## Mise a jour

Installer initialement ce firmware par USB avec un partitionnement OTA (compilation : min_spiffs).
Sur la page Web, ouvrir « Mise a jour OTA » et choisir le fichier applicatif `.ino.bin` produit pour cet ecran (pas le bootloader, la table de partitions ni une image fusionnee).
Le serveur ecrit dans la partition OTA inactive et redemarre uniquement lorsque Update confirme la reussite. Les parametres NVS restent conserves.

## Verification sur appareil

1. Premier demarrage sans configuration Web : acces sans identifiants.
2. Activer la protection ; verifier dans une fenetre privee que la page et l'OTA demandent les identifiants.
3. Redemarrer ; verifier que la protection persiste.
4. Desactiver sur le Web et verifier l'acces libre.
5. Reactiver, puis effectuer le reset depuis le menu Wi-Fi sans scan : verifier le redemarrage, l'acces libre et le champ identifiant vide.
6. Apres reset, tenter d'activer avec un mot de passe vide : l'activation doit etre refusee (ancien mot de passe efface).
7. Verifier qu'un firmware invalide est refuse et qu'un firmware compatible est installe avec conservation des reglages.

La compilation ne remplace pas ces essais sur l'ecran et le reseau reels.

## Adressage reseau

Menu Web « Reseau : DHCP / IP statique » : choisir le mode et sauvegarder.
DHCP reste le mode par defaut, y compris lors de la mise a jour d'un ancien firmware.
En IP statique, renseigner l'adresse IPv4 de l'ecran, le masque et une passerelle du meme sous-reseau. Le serveur refuse les adresses reseau/broadcast et les masques non contigus. Le DNS est facultatif : vide, il utilise la passerelle.
Les reglages sont sauvegardes ensemble en NVS, verifies apres ecriture, puis appliques au redemarrage. Le retour au DHCP ignore les champs statiques et conserve leurs valeurs pour une utilisation ulterieure.
Apres le changement, ouvrir la nouvelle adresse indiquee dans la reponse Web ou lire « IP Serveur Web/Ecran » sur l'ecran dans WIFI / RESEAU.

Verification : `tests/run_network_config_test.ps1` evalue les fonctions de validation du code de production par assertions de compilation. Sur appareil, verifier DHCP -> statique -> DHCP, conservation apres redemarrage, reconnexion apres scan et resolution DNS pour NTP/Tempo.
