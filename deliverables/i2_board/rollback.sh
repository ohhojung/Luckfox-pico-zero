#!/bin/sh
set -eu
backup=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
case "$backup" in /userdata/cctv-i2-backup-*) ;; *) echo 'Run the copy inside the board backup directory'; exit 1 ;; esac
[ -f "$backup/oem/usr/bin/RkLunch.sh" ] || exit 1
[ ! -x /etc/init.d/S59cctv ] || /etc/init.d/S59cctv stop
[ ! -x /etc/init.d/S60heartbeat ] || /etc/init.d/S60heartbeat stop
for relative in oem/usr/bin/RkLunch.sh usr/bin/cctv-dual-rtsp usr/bin/cctv-check-health etc/init.d/S59cctv usr/bin/i1-heartbeat etc/init.d/S60heartbeat etc/heartbeat.conf; do
    if [ -f "$backup/$relative" ]; then
        cp -p "$backup/$relative" "/$relative"
    else
        rm -f "/$relative"
    fi
done
rm -f /userdata/cctv-i2.enabled /tmp/cctv-oem-ready
sync
echo 'Previous files restored. Reboot manually.'
