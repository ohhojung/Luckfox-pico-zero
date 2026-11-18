#!/bin/sh
# Read-only; never print WireGuard keys or full configs.
set -u
iface=${1:-wg0}
case "$iface" in wg0|wg1) ;; *) echo 'Usage: sh check-board.sh [wg0|wg1]'; exit 2 ;; esac
failed=0
for service in S59cctv S60heartbeat; do
    if [ -x "/etc/init.d/$service" ]; then
        "/etc/init.d/$service" status || failed=1
    else
        echo "FAIL missing $service"; failed=1
    fi
done
if [ -x /etc/init.d/S59cctv ]; then
    /etc/init.d/S59cctv health || failed=1
fi
if pidof rkipc >/dev/null 2>&1; then
    echo 'FAIL rkipc active: check camera ownership'; failed=1
fi
if stamps=$(wg show "$iface" latest-handshakes 2>/dev/null); then
    now=$(date +%s)
    printf '%s\n' "$stamps" | awk -v now="$now" '
    NF==2 { peers++; if ($2==0) {print "FAIL peer=" peers " handshake=never"; bad=1}
    else if ($2>now) {print "UNKNOWN clock mismatch"; bad=1}
    else print "peer=" peers " handshake_age_seconds=" now-$2 }
    END {if (!peers) {print "FAIL no peers"; bad=1}; exit bad}' || failed=1
else
    echo "FAIL cannot query $iface"; failed=1
fi
echo 'Review route below: expected VPN interface for heartbeat destination'
ip route get 10.0.0.100 || failed=1
echo 'UTC: compare against Windows/server'
date -u
echo 'PENDING: HTTP200, server TCP RTSP reception, actual codec/fps/GOP, web playback, reboot recovery'
echo 'Process/frame checks do not prove end-to-end playback. Review handshake age and route manually.'
exit "$failed"
