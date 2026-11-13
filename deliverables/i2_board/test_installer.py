"""Linux-only isolated fixtures; never modifies the host board configuration."""
from pathlib import Path
import hashlib
import os
import re
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent

def run(args, ok=True, **kw):
    r = subprocess.run(args, text=True, capture_output=True, **kw)
    assert (r.returncode == 0) == ok, (args, r.stdout, r.stderr)
    return r

def snapshot(root):
    return {str(p.relative_to(root)): p.read_bytes() for p in root.rglob('*') if p.is_file()}

with tempfile.TemporaryDirectory(prefix='cctv-install-test-') as tmp:
    root = Path(tmp)
    for d in ['pkg', 'bin', 'oem/usr/share', 'userdata', 'etc', 'tmp']:
        (root/d).mkdir(parents=True)
    pkg = root/'pkg'
    # Prefix absolute board paths in copies of the scripts for isolation.
    for name in ['install.sh', 'network-fix.sh', 'ntp-off.awk']:
        text = (HERE/name).read_text()
        text = re.sub(r'/(?:userdata|oem|etc|tmp)/', lambda m: str(root)+m[0], text)
        (pkg/name).write_text(text)
    (root/'bin/id').write_text('#!/bin/sh\necho 0\n')
    (root/'bin/id').chmod(0o755)
    env = dict(os.environ, PATH=str(root/'bin')+':'+os.environ['PATH'])
    (pkg/'SHA256SUMS').write_text(''.join(hashlib.sha256((pkg/n).read_bytes()).hexdigest()+'  '+n+'\n' for n in ['install.sh','network-fix.sh','ntp-off.awk']))
    for args in [['--bad'], ['-dry-run'], ['--codec'], ['--codec','jpeg']]:
        before=snapshot(root)
        run(['sh',str(pkg/'install.sh'),*args],ok=False,env=env)
        assert snapshot(root)==before
    for target in ['oem/usr/share/camera.ini','userdata/rkipc.ini']:
        (root/target).write_text('[video.0]\nenable=1\n[network.ntp]\nenable = 1\n[video.1]\nenable=1\n')
    (root/'tmp/rkipc-factory-config.ini').symlink_to(root/'oem/usr/share/camera.ini')
    (root/'etc/dhcpcd.conf').write_text('hostname\ninterface wlan0\nmetric 200\n')
    before=snapshot(root)
    r=run(['sh',str(pkg/'install.sh'),'--dry-run','--codec','h264','--fix-network'],env=env)
    assert 'no board files or services changed' in r.stdout
    assert snapshot(root)==before
    backup=root/'userdata/cctv-i2-backup-test';backup.mkdir()
    run(['sh',str(pkg/'network-fix.sh'),'--apply',str(backup)])
    for target in ['oem/usr/share/camera.ini','userdata/rkipc.ini']:
        assert '[network.ntp]\nenable =0' in (root/target).read_text()
        assert (root/target).read_text().count('enable=1')==2
        assert (backup/('network'+str(root/target))).read_bytes()==before[target]
    assert (root/'etc/dhcpcd.conf').read_text().startswith('denyinterfaces wg*\nhostname')
    # Second application to another backup must not duplicate the directive.
    backup2=root/'userdata/cctv-i2-backup-test2';backup2.mkdir()
    run(['sh',str(pkg/'network-fix.sh'),'--apply',str(backup2)])
    assert (root/'etc/dhcpcd.conf').read_text().count('denyinterfaces wg*')==1
    # Unknown INI layout fails before modifying anything.
    (root/'userdata/rkipc.ini').write_text('[network.ntp]\nenable=1\nenable=0\n')
    before=snapshot(root)
    run(['sh',str(pkg/'network-fix.sh'),'--dry-run'],ok=False)
    assert snapshot(root)==before
print('PASS installer: invalid arguments, zero-write dry-run, scoped NTP change, network backup, idempotence, malformed INI rejection')
