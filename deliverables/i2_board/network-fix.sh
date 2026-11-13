#!/bin/sh
set -eu
mode=${1:-}
case "$mode" in --dry-run) [ "$#" = 1 ] || exit 2 ;; --apply) [ "$#" = 2 ] || exit 2 ;; *) exit 2 ;; esac
here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
factory=$(readlink -f /tmp/rkipc-factory-config.ini)
case "$factory" in /oem/usr/share/*.ini) ;; *) echo 'Unexpected factory INI path'; exit 1 ;; esac
for target in "$factory" /userdata/rkipc.ini /etc/dhcpcd.conf; do
    [ -f "$target" ] && [ ! -L "$target" ] || { echo "Missing or symlink: $target"; exit 1; }
done
for target in "$factory" /userdata/rkipc.ini; do
    awk -f "$here/ntp-off.awk" "$target" >/dev/null || { echo "Unrecognized NTP section: $target"; exit 1; }
    echo "PLAN: disable rkipc NTP in $target (not system time synchronization)"
done
echo 'PLAN: ensure global denyinterfaces wg* in /etc/dhcpcd.conf; no network restart'
[ "$mode" = --apply ] || exit 0
backup=$2
case "$backup" in /userdata/cctv-i2-backup-*) ;; *) exit 1 ;; esac
[ -d "$backup" ] || exit 1
# Back up every target before changing any target.
for target in "$factory" /userdata/rkipc.ini /etc/dhcpcd.conf; do
    mkdir -p "$backup/network$(dirname "$target")"
    cp -p "$target" "$backup/network$target"
    printf '%s\n' "$target" >> "$backup/network-files"
done
staged=$(mktemp /tmp/cctv-network.XXXXXX)
trap 'rm -f "$staged"' EXIT
for target in "$factory" /userdata/rkipc.ini; do
    awk -f "$here/ntp-off.awk" "$target" > "$staged"
    cat "$staged" > "$target"
done
# Prepend so the directive is global, outside any interface/profile block.
{ printf 'denyinterfaces wg*\n'; awk '$0 != "denyinterfaces wg*" { print }' /etc/dhcpcd.conf; } > "$staged"
cat "$staged" > /etc/dhcpcd.conf
