# Deye Monitor 4.8.4 — les micro-onduleurs dans la vue solaire

Les productions des passerelles apparaissent désormais à côté des MPPT sur l’écran et dans le tableau de bord Web.

- **PV1 à PV4** restent réservés aux entrées de l’onduleur principal.
- **PV5, PV6, PV7 et PV8** correspondent aux installations 1, 2, 3 et 4 de la configuration des micro-onduleurs. Un PV représente ici la production totale d’une passerelle, pas chaque micro-onduleur individuellement.
- Les numéros restent stables si une installation est désactivée, si un MPPT est masqué ou si l’onduleur ne possède que deux MPPT.
- Jusqu’à six sources sont visibles ensemble sur l’écran principal. Avec sept ou huit sources, les valeurs alternent toutes les quatre secondes. Les sources indisponibles affichent `-- W` ; une production réellement nulle affiche `0 W`.

Toucher **PRODUCTION PV >** ouvre **PRODUCTION PAR SOURCE**. La liste défilante affiche le numéro, le nom, la puissance et le statut : inclus au total, affiché séparément, déjà compté via GEN, doublon exclu ou indisponible. Les mesures s’actualisent pendant la consultation. Le port GEN garde son nom GEN pour ne pas entrer en collision avec PV5.

Le choix de comptage enregistré est conservé : rendre PV5 ou PV6 visible n’ajoute pas automatiquement sa production au total. Les calculs de recharge, de relais et d’historique conservent leurs règles de la 4.8.3. Le tableau de bord Web montre les mêmes sources, et la page de configuration des passerelles rappelle leur numéro PV.

## Validation

Compilation ESP32-S3 et tests logiciels. Les contrôles couvrent la correspondance avec les mesures réelles du service, les profils Deye/Sofar, les doublons, les modes séparé/GEN, les numéros stables, les données périmées et les sources masquées. Les aperçus LVGL clair/sombre vérifient six et huit sources, le toucher de la tuile, le défilement, le retour, les mises à jour et l’absence de chevauchement. Les scripts Web sont testés, notamment avec un nom contenant du HTML, affiché comme du texte.

Aucun écran ni passerelle physique n’a été connecté pendant cette validation. Les limites de compatibilité de la 4.8.3, notamment Sofar expérimental en lecture seule, restent applicables.

[Guide micro-onduleurs, microSD et Sofar](GUIDE_MICRO_SD_SOFAR.md) · [Fonctions de la 4.8.3](NOTES_4.8.3.md).
