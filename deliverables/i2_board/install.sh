#!/bin/sh
set -eu
dry_run=0
fix_network=0
codec=h265
while [ "$#" -gt 0 ]; do
    case "$1" in
        --dry-run) dry_run=1; shift ;;
        --fix-network) fix_network=1; shift ;;
        --codec)
            [ "$#" -ge 2 ] || { echo 'Missing --codec value'; exit 2; }
            codec=$2
            case "$codec" in h264|h265) ;; *) echo 'Codec must be h264 or h265'; exit 2 ;; esac
            shift 2 ;;
        -h|--help) echo 'Usage: sh install.sh [--dry-run] [--codec h264|h265] [--fix-network]'; exit 0 ;;
        *) echo "Unknown option: $1"; exit 2 ;;
    esac
done
cd "$(dirname "$0")"
[ "$(id -u)" = 0 ] || { echo 'Run as root'; exit 1; }
sha256sum -c SHA256SUMS || exit 1
if [ "$dry_run" = 1 ]; then
    echo "DRY_RUN: install binaries/services, select $codec for both channels, back up and patch RkLunch, remove old heartbeat.conf"
    [ ! -e /userdata/cctv-i2.enabled ] || echo 'BLOCKER for real install: already enabled; recorded rollback required first'
    for name in rkipc cctv-dual-rtsp; do
        if pidof "$name" >/dev/null 2>&1; then echo "BLOCKER for real install: stop $name first"; fi
    done
    [ "$fix_network" = 0 ] || sh network-fix.sh --dry-run
    echo 'DRY_RUN complete: no board files or services changed'
    exit 0
fi
[ "$fix_network" = 0 ] || sh network-fix.sh --dry-run
for name in rkipc cctv-dual-rtsp; do
    if pidof "$name" >/dev/null 2>&1; then
        echo "Stop $name before installing. No changes made."
        exit 1
    fi
done
[ ! -e /userdata/cctv-i2.enabled ] || { echo 'Already enabled; use the recorded rollback first'; exit 1; }
launch=/oem/usr/bin/RkLunch.sh
[ -f "$launch" ] && [ ! -L "$launch" ] || exit 1
for target in /usr/bin/cctv-dual-rtsp /usr/bin/cctv-check-health /etc/init.d/S59cctv /usr/bin/i1-heartbeat /etc/init.d/S60heartbeat /etc/heartbeat.conf /etc/cctv-codec; do
    [ ! -L "$target" ] || { echo "Refusing symlink: $target"; exit 1; }
done
staged=$(mktemp /tmp/RkLunch.cctv.XXXXXX)
trap 'rm -f "$staged"' EXIT
awk -f patch-rklunch.awk "$launch" > "$staged" || { echo 'Unrecognized RkLunch: no changes made'; exit 1; }
sh -n "$staged"
backup=$(mktemp -d /userdata/cctv-i2-backup-XXXXXX)
for relative in oem/usr/bin/RkLunch.sh usr/bin/cctv-dual-rtsp usr/bin/cctv-check-health etc/init.d/S59cctv usr/bin/i1-heartbeat etc/init.d/S60heartbeat etc/heartbeat.conf etc/cctv-codec; do
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
printf '%s\n' "$codec" > /etc/cctv-codec
[ "$fix_network" = 0 ] || sh network-fix.sh --apply "$backup"
touch /userdata/cctv-i2.enabled
sync
echo 'Installed. Reboot manually, then verify both streams and heartbeat.'
