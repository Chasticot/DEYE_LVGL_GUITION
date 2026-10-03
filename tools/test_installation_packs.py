"""Vérifie les ZIP et le refus de fichiers absents/corrompus, sans port série.

Sans option, cible le pack unifié de la version courante. --archive sélectionne un ZIP précis ;
--variant sélectionne les packs d'une variante, même avec des archives historiques.
"""
import argparse
import hashlib
import json
import re
import subprocess
import tempfile
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def verify(folder, expected_code, message):
    result = subprocess.run(['powershell.exe', '-NoProfile', '-ExecutionPolicy', 'Bypass',
                             '-File', str(folder / 'installer.ps1'), '-VerificationSeule'],
                            capture_output=True, timeout=30)
    if result.returncode != expected_code:
        raise AssertionError(f'{message}: code {result.returncode}; {result.stdout!r}; {result.stderr!r}')
    if expected_code and message.encode('ascii') not in result.stdout:
        raise AssertionError(f'Cause attendue absente : {message}; {result.stdout!r}')


def select_archives(args):
    if args.archive:
        archives = [path.resolve() for path in args.archive]
    elif args.variant:
        archives = []
        for path in sorted((ROOT / 'dist').glob('DEYE_*_Installation_Windows.zip')):
            with zipfile.ZipFile(path) as archive:
                manifest = json.loads(archive.read(f'{path.stem}/manifest.json'))
            if manifest['variant'] in args.variant:
                archives.append(path)
    else:
        config = (ROOT / 'firmware/DEYE_UNIFIED/config.h').read_text(encoding='utf-8-sig')
        version = re.search(r'^#define FIRMWARE_VERSION "([^"]+)"', config, re.M)[1]
        archives = [ROOT / 'dist' / f'DEYE_V3_{version}_Installation_Windows.zip']
    if not archives:
        raise FileNotFoundError('Aucun pack correspondant à la sélection')
    for archive in archives:
        if not archive.is_file():
            raise FileNotFoundError(archive)
    return sorted(set(archives))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument('--archive', action='append', type=Path,
                           help='Archive précise à vérifier (option répétable)')
    selection.add_argument('--variant', action='append', choices=['V3', 'VETRONIC_V3', 'UNIFIED'],
                           help='Variante du manifest.json à vérifier (option répétable)')
    archives = select_archives(parser.parse_args())
    with tempfile.TemporaryDirectory(prefix='deye-pack-test-') as temporary:
        test_root = Path(temporary).resolve()
        assert test_root.parent == Path(tempfile.gettempdir()).resolve()
        assert test_root.name.startswith('deye-pack-test-')
        for archive in archives:
            with zipfile.ZipFile(archive) as z:
                assert z.testzip() is None
                for name in z.namelist():
                    assert (test_root / name).resolve().is_relative_to(test_root)
                z.extractall(test_root)
            folder = test_root / archive.stem
            manifest = json.loads((folder / 'manifest.json').read_text(encoding='utf-8'))
            for entry in manifest['files']:
                path = (folder / entry['name']).resolve()
                assert path.is_relative_to(folder.resolve())
                data = path.read_bytes()
                assert len(data) == entry['bytes']
                assert hashlib.sha256(data).hexdigest() == entry['sha256']
            verify(folder, 0, 'Pack complet')
            firmware = folder / 'firmware.bin'
            original = firmware.read_bytes()
            firmware.write_bytes(b'CORROMPU' + original[8:])
            verify(folder, 1, 'firmware.bin')
            firmware.write_bytes(original)
            (folder / 'bootloader.bin').unlink()
            verify(folder, 1, 'bootloader.bin')
            print(f'{archive.name}: ZIP, empreintes, validation seule, refus corruption et fichier absent : OK')


if __name__ == '__main__':
    main()
