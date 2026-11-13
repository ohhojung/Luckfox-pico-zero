#!/bin/sh
set -eu
cd "$(dirname "$0")"
[ "$(id -u)" = 0 ] || { echo 'Run as root'; exit 1; }
sha256sum -c SHA256SUMS || exit 1
for name in rkipc cctv-dual-rtsp; do
    if pidof "$name" >/dev/null 2>&1; then
        echo "Stop $name before installing. No changes made."
        exit 1
    fi
done
[ ! -e /userdata/cctv-i2.enabled ] || { echo 'Already enabled; use the recorded rollback first'; exit 1; }
launch=/oem/usr/bin/RkLunch.sh
[ -f "$launch" ] && [ ! -L "$launch" ] || exit 1
for target in /usr/bin/cctv-dual-rtsp /usr/bin/cctv-check-health /etc/init.d/S59cctv /usr/bin/i1-heartbeat /etc/init.d/S60heartbeat /etc/heartbeat.conf; do
    [ ! -L "$target" ] || { echo "Refusing symlink: $target"; exit 1; }
done
staged=$(mktemp /tmp/RkLunch.cctv.XXXXXX)
trap 'rm -f "$staged"' EXIT
awk -f patch-rklunch.awk "$launch" > "$staged" || { echo 'Unrecognized RkLunch: no changes made'; exit 1; }
sh -n "$staged"
backup=$(mktemp -d /userdata/cctv-i2-backup-XXXXXX)
for relative in oem/usr/bin/RkLunch.sh usr/bin/cctv-dual-rtsp usr/bin/cctv-check-health etc/init.d/S59cctv usr/bin/i1-heartbeat etc/init.d/S60heartbeat etc/heartbeat.conf; do
    if [ -e "/$relative" ]; then
        mkdir -p "$backup/$(dirname "$relative")"
        cp -p "/$relative" "$backup/$relative"
    fi
done
cp rollback.sh "$backup/rollback.sh"
echo "BACKUP=$backup"
echo 'If installation fails, run: sh BACKUP_PATH/rollback.sh'
[ ! -x /etc/init.d/S60heartbeat ] || /etc/init.d/S60heartbeat stop
cp cctv-dual-rtsp /usr/bin/cctv-dual-rtsp
cp check-health.sh /usr/bin/cctv-check-health
cp S59cctv /etc/init.d/S59cctv
cp heartbeat /usr/bin/i1-heartbeat
cp S60heartbeat /etc/init.d/S60heartbeat
chmod 755 /usr/bin/cctv-dual-rtsp /usr/bin/cctv-check-health /etc/init.d/S59cctv /usr/bin/i1-heartbeat /etc/init.d/S60heartbeat
# The old optional config is backed up; use the new compiled/service defaults.
[ ! -f /etc/heartbeat.conf ] || rm /etc/heartbeat.conf
cat "$staged" > "$launch"
touch /userdata/cctv-i2.enabled
sync
echo 'Installed. Reboot manually, then verify both streams and heartbeat.'
