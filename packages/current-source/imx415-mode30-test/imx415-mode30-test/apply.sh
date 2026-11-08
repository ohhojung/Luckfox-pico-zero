#!/bin/sh
set -eu
pkg=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
kernel=${2:-/home/student/luckfox-live/luckfox-pico/sysdrv/source/kernel}
target="$kernel/drivers/media/i2c/imx415.c"
backup="$target.before-mode30"
original=a3a847eafb1b8a675af60ed436a73977229c5e6c98c6fb6c8e3a1064bf2d9831
modified=01d58e9cdac2341501a45c346ebce5d9868c839c95d9135ce9844a362d881c8a
hash() { sha256sum "$1" | cut -d ' ' -f 1; }
case ${1:-check} in
check)
    printf 'Current source: '; hash "$target"
    printf 'Package source: '; hash "$pkg/imx415.c"
    [ "$(hash "$pkg/imx415.c")" = "$modified" ]
    ;;
apply)
    [ "$(hash "$pkg/imx415.c")" = "$modified" ] || { echo 'Package hash mismatch'; exit 1; }
    [ "$(hash "$target")" = "$original" ] || { echo 'Source differs; stop without overwriting'; exit 1; }
    if [ -e "$backup" ]; then
        [ "$(hash "$backup")" = "$original" ] || { echo 'Backup differs; stop'; exit 1; }
    else
        cp -p "$target" "$backup"
    fi
    cp "$pkg/imx415.c" "$target"
    [ "$(hash "$target")" = "$modified" ]
    echo "Applied source only. Backup: $backup"
    ;;
restore)
    [ "$(hash "$backup")" = "$original" ] || { echo 'Backup hash mismatch'; exit 1; }
    [ "$(hash "$target")" = "$modified" ] || { echo 'Source differs; stop without overwriting'; exit 1; }
    cp -p "$backup" "$target"
    [ "$(hash "$target")" = "$original" ]
    echo 'Original source restored. Installed board firmware is unchanged.'
    ;;
*) echo 'Usage: sh apply.sh check|apply|restore [kernel-directory]'; exit 2 ;;
esac
