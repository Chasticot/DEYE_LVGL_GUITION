# Validation Vetronic V3 — 2 octobre 2026

Version : `4.3.4-vetronic-v3`, dédiée au 12K-SG02LP1 et à la WB01.

## Résultats

- Compilation PlatformIO `vetronic_v3` réussie avec les bibliothèques locales.
- RAM statique : 60 384 / 327 680 octets (18,4 %).
- Application : 1 522 973 / 1 966 080 octets (77,5 %).
- Binaire OTA : 1 523 344 octets.
- Tests de profils, blocs, conversions, migration et isolation NVS réussis.
- Tests horaires, tarifs, relais, veille et énergie GEN réussis.
- Tests WB01 : courant 6–32 A, limites API, fraîcheur, NaN/infini,
  courant invalide, conversion en watts, SOC/hystérésis et saisies strictes réussis.
- Pause/reprise tarifaire : charge manuelle locale seulement, abandon après
  retour à la borne, changement externe détecté, redémarrage et réponse POST
  incertaine vérifiés.
- Tests JSON de configuration et de statut WB01 réussis : libellé de retour
  à la borne, estimation, mesures indisponibles et état tarifaire.
- Tests et rendus LVGL 480 × 480 réussis : menus, réglages +/- sans envoi,
  brouillons SOC, enregistrement explicite, modes occupé/hors ligne, saisie
  réseau et bouton Rendre la main sur deux lignes dans ses limites.
- Rendus PNG sombre et clair examinés visuellement.
- Script Web de production exécuté avec DOM et transport simulés : retour à
  la borne via `legacy`, jeton CSRF, absence de POST à l'ouverture, boutons
  selon tarifs/occupation/liaison, mesures et validation SOC vérifiés.

Commandes :

```powershell
pio run -e vetronic_v3
./firmware/DEYE_VETRONIC_V3/tests/run_checks.ps1
node ./firmware/DEYE_VETRONIC_V3/tests/web_test.cjs
```

Les chemins du compilateur Zig peuvent être fournis par `-Zig` au script.
Les journaux locaux sont `checks.log` et `build.log`.

## Publication GitHub — 3 octobre 2026

Les mêmes sources ont été recompilées avec les dépendances PlatformIO fixées
du dépôt GitHub. Compilation réussie : RAM 60 392 octets, application
1 522 557 octets (77,4 %), binaire OTA 1 522 928 octets. Les tests hôtes
et le script Web de cette copie de publication passent également.

Pack Windows contrôlé sans port série : archive ZIP, empreintes SHA-256,
validation seule de l'installateur, refus d'un firmware corrompu et d'un
bootloader manquant. Les deux packs précédents passent aussi ces vérifications.

### Portée matérielle

Aucun téléversement et aucune commande réelle de charge n'ont été effectués.
Le firmware de la passerelle a seulement été lu pour confirmer l'API actuelle.
Ses fichiers ne sont pas modifiés par cette version de l'écran.

Restent à vérifier sur le matériel : courant mesuré versus passerelle,
confirmation WB01, charge manuelle faible/arrêt/retour à la borne, solaire,
SOC, pause/reprise tarifaire, relais, veille et compteurs Deye/GEN.

Le rendu Web est testé par simulation ; il n'a pas été servi par un ESP32 réel.
