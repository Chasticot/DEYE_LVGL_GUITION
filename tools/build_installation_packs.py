"""Assemble les packs Windows depuis des compilations PlatformIO existantes.

Ne compile pas et ne communique jamais avec un appareil.
Exemple : python tools/build_installation_packs.py --esptool C:/chemin/esptool.exe
"""
import argparse
import hashlib
import json
import re
import shutil
import struct
import subprocess
import zipfile
from pathlib import Path

SRC = Path(__file__).resolve().parents[1]
PROJECT = SRC
TEMPLATES = SRC / 'installation'
OUTPUT = SRC / 'dist'
VARIANTS = (
    ('V3', '12KSG02LP1_v3', 'DEYE_V3'),
    ('VETRONIC_V3', 'vetronic_v3', 'DEYE_VETRONIC_V3'),
    ('UNIFIED', 'deye_unified', 'DEYE_UNIFIED'),
)


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def check_partitions(path):
    raw = path.read_bytes()
    found = {}
    for start in range(0, len(raw), 32):
        record = raw[start:start + 32]
        if record[:2] != b'\xaa\x50':
            break
        _, kind, subtype, offset, size, label, _ = struct.unpack('<HBBII16sI', record)
        found[label.rstrip(b'\x00').decode()] = (kind, subtype, offset, size)
    expected = {
        'nvs': (1, 2, 0x9000, 0x5000),
        'otadata': (1, 0, 0xe000, 0x2000),
        'app0': (0, 0x10, 0x10000, 0x1e0000),
        'app1': (0, 0x11, 0x1f0000, 0x1e0000),
        'spiffs': (1, 0x82, 0x3d0000, 0x20000),
        'coredump': (1, 3, 0x3f0000, 0x10000),
    }
    if found != expected:
        raise ValueError(f'Partitionnement inattendu : {path}: {found}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--esptool', required=True, type=Path)
    parser.add_argument('--platformio-home', type=Path, default=Path.home() / '.platformio')
    parser.add_argument('--variant', action='append', choices=[variant[0] for variant in VARIANTS])
    args = parser.parse_args()
    packages = args.platformio_home / 'packages'
    boot_app = packages / 'framework-arduinoespressif32/tools/partitions/boot_app0.bin'
    license_file = packages / 'tool-esptoolpy/LICENSE'
    for path in (args.esptool, boot_app, license_file):
        if not path.is_file():
            raise FileNotFoundError(path)
    tool_version = subprocess.run([str(args.esptool.resolve()), 'version'], check=True,
                                  capture_output=True, text=True).stdout
    if 'esptool.py v4.5.1' not in tool_version:
        raise ValueError('Ce générateur attend esptool 4.5.1, comme les packs Arduino existants')
    if boot_app.stat().st_size != 0x2000:
        raise ValueError('Taille boot_app0.bin inattendue')
    OUTPUT.mkdir(exist_ok=True)
    release_files = []
    for variant, env, name in VARIANTS:
        if args.variant and variant not in args.variant:
            continue
        firmware_dir = SRC / 'firmware' / name
        config = (firmware_dir / 'config.h').read_text(encoding='utf-8-sig')
        version = re.search(r'^#define FIRMWARE_VERSION "([^"]+)"', config, re.M)[1]
        build = PROJECT / '.pio' / 'build' / env
        check_partitions(build / 'partitions.bin')
        if (build / 'firmware.bin').stat().st_size > 0x1e0000:
            raise ValueError('Firmware trop grand pour app0')
        if (build / 'bootloader.bin').stat().st_size > 0x8000:
            raise ValueError('Bootloader trop grand')
        if version.encode() not in (build / 'firmware.bin').read_bytes():
            raise ValueError(f'Version {version} absente du binaire : recompiler {env}')
        prefix = (f"DEYE_12KSG02LP1_VETRONIC_V3_{version.split('-')[0]}" if variant == 'VETRONIC_V3'
                  else f'DEYE_V3_{version}' if variant == 'UNIFIED' else f'DEYE_{variant}_{version}')
        folder = OUTPUT / f'{prefix}_Installation_Windows'
        folder.mkdir(exist_ok=True)
        for name in ('firmware.bin', 'bootloader.bin', 'partitions.bin'):
            shutil.copy2(build / name, folder / name)
        shutil.copy2(boot_app, folder / 'boot_app0.bin')
        shutil.copy2(args.esptool, folder / 'esptool.exe')
        shutil.copy2(license_file, folder / 'LICENSE_ESPTOOL.txt')
        for name in ('INSTALLER.bat', 'installer.ps1'):
            encoding = 'utf-8-sig' if name.endswith('.ps1') else 'ascii'
            (folder / name).write_text((TEMPLATES / name).read_text(encoding='utf-8'), encoding=encoding, newline='\r\n')
        extra = (f'Firmware unifié {version} : choisir le modèle Deye et, indépendamment, Aucune borne, Deye LoRa ou VE TRONIC / WB01 dans les réglages. Pour WB01, configurer l’IP de sa passerelle ESP32. Le choix de borne prend effet après redémarrage. Guide inclus : GUIDE_UNIFIE_{version}.md.'
                 if variant == 'UNIFIED' else
                 'Le menu Modèle permet de choisir votre onduleur. Consulter le guide PDF et le récapitulatif inclus.'
                 if variant == 'V3' else
                 'VEtronic V3 pour 12K-SG02LP1 : menus V3, WB01, relais, veille et tarifs. Le bouton Rendre la main a la borne la laisse autonome. Configurer l’IP de sa passerelle ESP32. Guide Markdown inclus : GUIDE_VETRONIC_V3.md.')
        erase_notice = ('Choisir O pour une remise à zéro complète ou un écran neuf : tous les réglages et les données enregistrées sont supprimés. Depuis V3/Vetronic V3 avec le même partitionnement, N permet de conserver les zones de données ; la migration 4.3.5 ne nécessite pas à elle seule un effacement. Sauvegarder avant toute opération et choisir explicitement la borne après migration.'
                       if variant == 'UNIFIED' else
                       'Choisir O pour effacer la mémoire sur un écran neuf ou lors d’un changement de variante/partitionnement. Cela supprime tous les réglages et les données enregistrées. N conserve les zones de données, sans garantie de compatibilité avec les anciennes versions. Exporter avant toute opération.')
        notice = f'''INSTALLATION DEYE MONITOR — {variant} — {version}

Pour Windows 10/11 avec Windows PowerShell 5.1, sans Arduino IDE ni Python à installer.
Matériel : écran ESP32-S3 GUITION prévu par ce projet. Ces binaires dépendent du matériel de l’écran ; ils ne conviennent pas à toutes les cartes ESP32-S3.

1. Extraire TOUT le ZIP dans un dossier du PC (ne pas lancer depuis le ZIP).
2. Sauvegarder les réglages de l’écran existant avec l’export JSON si disponible.
3. Brancher l’écran avec un câble USB de données. Débrancher les autres cartes ESP pour éviter une erreur de port.
4. Fermer le moniteur série et les logiciels utilisant le port COM.
5. Double-cliquer sur INSTALLER.bat. Aucun droit administrateur n’est normalement nécessaire.
6. Si un seul CH340 est détecté, son port COM est sélectionné automatiquement et affiché. Si plusieurs CH340 sont présents, ou aucun, saisir le port COM de l’écran dans la liste proposée. Pour le repérer : Gestionnaire de périphériques > Ports (COM et LPT), puis débrancher/rebrancher l’écran et repérer le port qui apparaît.
7. Accepter 460800 bauds avec Entrée. En cas de connexion instable, relancer et choisir 115200.
8. {erase_notice}
9. Vérifier la variante, le port et le choix d’effacement, puis saisir INSTALLER.
10. Attendre le message de réussite. Ne pas débrancher pendant l’écriture. L’écran redémarre ; au besoin appuyer sur RESET.
11. Configurer le Wi-Fi, l’adresse IP locale du logger et son numéro de série dans les menus de l’écran.

{extra}

DÉPANNAGE
- Aucun port : essayer un câble de données, un autre port USB, puis vérifier le Gestionnaire de périphériques. Si un pilote manque, installer le pilote correspondant au convertisseur USB identifié par Windows (par exemple CH340 sur les écrans qui en sont équipés).
- Connexion impossible : maintenir BOOT, appuyer puis relâcher RESET, relâcher BOOT et relancer. Le numéro COM peut changer. Si BOOT/RESET sont inaccessibles, consulter la notice matérielle de l’écran.
- Accès refusé : fermer tout logiciel qui utilise le port série.
- Fichier absent ou endommagé : télécharger/extraire de nouveau le ZIP complet.
- PowerShell bloqué par une politique d’entreprise : contacter l’administrateur du PC. Le BAT ne modifie pas la politique permanente de Windows.
- Écran noir malgré un flash réussi : redémarrer puis vérifier que le matériel correspond au projet. Le succès d’esptool confirme l’écriture, pas le fonctionnement de l’écran ou la communication avec l’onduleur.

CONTENU ET INFORMATIONS TECHNIQUES
esptool.exe : outil Espressif, version 4.5.1, repris du pack Arduino existant.
Projet et code source de cet outil : https://github.com/espressif/esptool/tree/v4.5.1
Licence de l’outil incluse dans LICENSE_ESPTOOL.txt.
INSTALLER.bat lance installer.ps1 avec Windows PowerShell.
manifest.json contient la variante, la version et les empreintes SHA-256 des fichiers.
Le contrôle des empreintes détecte une corruption ou un mélange de fichiers ; ce n’est pas une signature numérique.

Binaires issus de PlatformIO, environnement {env}, espressif32 6.10.0, Arduino ESP32 2.0.17, partitionnement min_spiffs 4 Mo :
  0x00000  bootloader.bin
  0x08000  partitions.bin
  0x0E000  boot_app0.bin (réinitialise la sélection OTA pour démarrer sur l’application écrite par USB)
  0x10000  firmware.bin
La carte peut disposer de plus de mémoire physique ; ce pack utilise un partitionnement de 4 Mo.
Ne pas mélanger ces fichiers avec ceux d’un ancien pack ou de l’autre variante.
Pour une mise à jour OTA compatible, utiliser uniquement firmware.bin dans la page de mise à jour de l’écran, jamais les trois autres .bin.

Vérification sans écran, depuis PowerShell dans ce dossier :
  powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\\installer.ps1 -VerificationSeule
Cette commande contrôle les fichiers sans ouvrir de port série.
'''
        (folder / 'LISEZ_MOI.txt').write_text(notice, encoding='utf-8-sig', newline='\r\n')
        if variant == 'V3':
            for name in (() if args.variant and 'V3' not in args.variant else ('GUIDE_UTILISATEUR_V3.pdf', 'RECAP_UTILISATEURS_V3.pdf')):
                shutil.copy2(SRC / 'docs/pdf' / name, folder / name)
        if variant == 'VETRONIC_V3':
            shutil.copy2(SRC / 'docs/GUIDE_VETRONIC_V3.md', folder / 'GUIDE_VETRONIC_V3.md')
            (folder / 'images').mkdir(exist_ok=True)
            shutil.copy2(SRC / 'docs/images/vetronic-v3.png', folder / 'images/vetronic-v3.png')
        if variant == 'UNIFIED':
            guide_name = f'GUIDE_UNIFIE_{version}.md'
            shutil.copy2(SRC / 'docs' / guide_name, folder / guide_name)
        manifest = {'variant': variant, 'version': version, 'environment': env,
                    'chip': 'esp32s3', 'flash_size': '4MB', 'esptool_version': '4.5.1',
                    'files': [{'name': p.relative_to(folder).as_posix(), 'bytes': p.stat().st_size, 'sha256': sha256(p)}
                              for p in sorted(folder.rglob("*")) if p.is_file() and p.name != 'manifest.json']}
        (folder / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        archive = folder.parent / (folder.name + '.zip')
        with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as z:
            for file in sorted(folder.rglob("*")):
                if file.is_file():
                    z.write(file, f'{folder.name}/{file.relative_to(folder).as_posix()}')
        with zipfile.ZipFile(archive) as z:
            if z.testzip() is not None:
                raise ValueError(f'Archive corrompue : {archive}')
        print(f'{archive.name}: {archive.stat().st_size} octets; SHA256 {sha256(archive)}')
        ota = OUTPUT / f'{prefix}_OTA.bin'
        shutil.copy2(build / 'firmware.bin', ota)
        if variant == 'UNIFIED':
            guide = OUTPUT / guide_name
            shutil.copy2(SRC / 'docs' / guide_name, guide)
            (OUTPUT / f'SHA256SUMS-{version}.txt').write_text(
                ''.join(f'{sha256(p)}  {p.name}\n' for p in (archive, ota, guide)), encoding='ascii')
        else:
            release_files.extend([archive, ota])
    for name in (() if args.variant and 'V3' not in args.variant else ('GUIDE_UTILISATEUR_V3.pdf', 'RECAP_UTILISATEURS_V3.pdf')):
        target = OUTPUT / name
        shutil.copy2(SRC / 'docs/pdf' / name, target)
        release_files.append(target)
    if release_files:
        (OUTPUT / ('SHA256SUMS-VETRONIC_V3.txt' if args.variant == ['VETRONIC_V3'] else 'SHA256SUMS.txt')).write_text(''.join(f'{sha256(p)}  {p.name}\n' for p in release_files), encoding='ascii')


if __name__ == '__main__':
    main()
