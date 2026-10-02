"""Vérifie les ZIP et le refus de fichiers absents/corrompus, sans port série."""
import hashlib
import json
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


archives = sorted((ROOT / 'dist').glob('DEYE_*_Installation_Windows.zip'))
assert len(archives) == 2, 'Deux variantes attendues'
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
            data = (folder / entry['name']).read_bytes()
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
