# Mise à jour GitHub depuis l’écran — base 4.6.0

Dans **Configuration > Mise à jour**, toucher **Vérifier sur GitHub**, puis **Installer** et confirmer. La progression est affichée ; l’écran redémarre après validation du fichier. Les réglages sont conservés. L’écran reste éveillé pendant l’opération.

Connexion Wi-Fi avec accès Internet et heure correcte requises. Garder l’écran alimenté pendant l’installation. Les erreurs de connexion ou de fichier sont affichées et permettent de réessayer.

La dernière release stable publique de `Chasticot/DEYE_LVGL_GUITION` est consultée. Son tag doit être `vX.Y.Z`, avec un seul asset applicatif `DEYE_UNIFIED_X.Y.Z_OTA.bin` ou `DEYE_V3_X.Y.Z_OTA.bin`, sa taille et son `digest` SHA-256 dans l’API GitHub. Les préversions, versions anciennes, images USB et fichiers trop grands sont refusés. Le certificat HTTPS et le SHA-256 sont vérifiés avant activation du nouveau firmware. Aucun jeton GitHub n’est embarqué.

Installer cette fonctionnalité une première fois par la page web OTA existante ou par USB. Les prochaines versions compatibles pourront ensuite être installées depuis l’écran. La release publique était v4.4.1 lors de la vérification ; elle est plus ancienne que cette base 4.6.0 et ne sera pas proposée.

## Sources publiées

Les sources unifiées correspondent à la base locale 4.6.0 utilisée sur l’écran COM16 : elles incluent les évolutions de connexion/recherche des loggers et de réglage manuel de l’heure absentes de la publication 4.4.1, nécessaires à la reproduction du firmware installé.

## Validation du 17 octobre 2026

- Compilation PlatformIO du firmware : réussie ; flash utilisée 1 619 389 / 1 966 080 octets.
- Tests numériques de version, états du menu, boutons et limites des textes : réussis.
- Rendu LVGL en thèmes clair et sombre : vérifié.
- Tests existants des scripts web et de comparaison des releases : réussis.
- API GitHub publique : fichier OTA et empreinte SHA-256 présents sur v4.4.1.
- Installation sur COM16 à l’adresse 0x10000 : écriture et empreinte vérifiées ; mémoire de réglages non effacée.
- Après redémarrage : PSRAM détectée, écran principal chargé et interface LVGL prête confirmés par le journal série.

Le scénario complet d’installation depuis une nouvelle release, l’interruption réseau pendant le téléchargement et la vérification visuelle sur l’écran physique restent à tester. Les sauvegardes de mémoire et les journaux série du matériel ne sont pas publiés.
