# Validation 4.4.0 — récupération de l’IP du logger — 11 octobre 2026

- Projet principal VS Code / PlatformIO : `src/deye_unified`, environnement par défaut `deye_unified`, compilation réussie. Application 1 558 149 octets, RAM statique 61 224 octets.
- Sources exactes du dépôt de publication : compilation `deye_unified` réussie, application 1 557 837 octets (79,2 % de la partition 1 966 080), RAM statique 61 224 octets (18,7 %).
- Binaire OTA publié : 1558208 octets ; SHA-256 `02b0bc9daab8403ddfd851c099f969606afbbf81e748959e7af321d460fdb4bd`. Les compilations locales et de publication peuvent produire des binaires différents ; le fichier livré est celui du dépôt de publication.
- Treize tests C++ de fonctions communes/parsing, tests du service réseau et de la récupération automatique, trois scripts JavaScript de production, deux rendus LVGL : réussis. Assertions activées avec `-UNDEBUG`.
- Pack Windows : ZIP et manifest vérifiés, contrôle sans port série réussi ; fichiers corrompus et absents refusés.
- esptool 4.5.1 : checksum et hash incorporé du binaire ESP32-S3 valides.

## Découverte du logger

Le service de production est exécuté avec UDP/TCP simulés. Les tests couvrent les réponses broadcast/unicast, l’identification V5, les réponses retardées de deux secondes, la fenêtre de dix secondes, les réponses tronquées, les tampons bornés, les coupures Wi-Fi et les recherches simultanées. Les enveloppes V5 de mauvais numéro de série, séquence ou checksum sont refusées.

Le simulateur UDP conserve les octets non lus comme WiFiUDP ESP32. Un paquet de 200 octets suivi d’une réponse valide reproduit le blocage sans vidage ; le service corrigé trouve la seconde réponse. Les messages UDP IP/MAC/SN sont validés et le numéro fourni filtre les autres loggers.

## Récupération automatique et sauvegarde

Le contrôleur de production et la fonction exacte de sauvegarde sont testés ensemble avec transports et Preferences simulés. Les tests vérifient :

- Plusieurs échecs et délai de 60 secondes ; suspension hors Wi-Fi, pendant l’OTA ou pendant une autre recherche ; reprise après perte au démarrage avec paramètres enregistrés.
- Une réponse valide du logger remet le compteur de perte à zéro ; les erreurs Modbus ne sont pas assimilées à une absence de réponse V5 par le lecteur.
- Attente de cinq minutes après une recherche infructueuse, nouvelle tentative au terme du délai et passage de `millis()` par zéro.
- Identification du même numéro de série, modification de la seule clé NVS `deye_host`, lecture de contrôle, conservation du numéro de série. Le simulateur n’expose aucune méthode d’écriture du numéro de série à la fonction de récupération.
- Aucun redémarrage demandé pour une IP identique, un résultat manuel, un autre numéro, une coupure Wi-Fi avant application, une erreur NVS ou des paramètres modifiés pendant la recherche.
- Suspension du lecteur entre découverte et application, maintenue après sauvegarde jusqu’au redémarrage ; absence d’application pendant l’OTA. Le redémarrage passe par le mécanisme existant de la boucle principale après 1,5 seconde.

## Interfaces et documentation

Le script Web de production est testé pour CSRF, phases, remplissage manuel unique, conservation d’un formulaire ouvert pendant un résultat automatique, indication de redémarrage et échec de sauvegarde. Les statuts tactiles sont rendus dans les deux thèmes, y compris recherche automatique, nouvelle IP sauvegardée et sauvegarde refusée ; les mesures des libellés vérifient l’absence de chevauchement.

Le README, le guide 4.4.0, les instructions d’installation/compilation et les notes de version décrivent le numéro de série à enregistrer auparavant, les délais, la conservation du numéro et le redémarrage uniquement pour une IP différente sauvegardée. Les guides des releases précédentes sont conservés.

## Limite matérielle

L’utilisateur a confirmé que la découverte manuelle de la variante précédente trouvait bien l’IP avec son numéro de série saisi. Le changement DHCP complet avec sauvegarde automatique et redémarrage n’a pas été essayé sur écran physique pour cette release ; il reste à confirmer sur l’installation. Aucun appareil n’a été flashé et aucune commande de recharge n’a été envoyée pour la publication.

La découverte suit le masque pour les sous-réseaux /24 ou plus petits ; pour les réseaux plus grands, elle est limitée au /24 local de l’écran. Le réseau doit autoriser l’accès au logger. Sans numéro de série réel enregistré, l’automatisme est désactivé. Pendant le balayage, les lectures Deye sont suspendues ; la passerelle WB01 conserve sa tâche propre.

[Guide 4.4.0](GUIDE_UNIFIE_4.4.0.md) · [Release](RELEASE_v4.4.0.md) · [Validation précédente](VALIDATION_4.3.10.md)
