#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
sdk=${2:-/home/student/luckfox-live/luckfox-pico}
target="$sdk/sysdrv/source/kernel/drivers/rknpu/rknpu_job.c"
backup="$target.before-progress-diag"
original=1235ce135a3e0a46e7339b929d8893ac67366e82261263877540fec61d886f3c
patched=6919692a1d4398c1f751bfe9c1ff7fefac1afb9beabbec8701d2ca1455768ec5
hash_file() { sha256sum "$1" | cut -d ' ' -f 1; }
[ -f "$target" ] || { echo "Missing source: $target"; exit 1; }
current=$(hash_file "$target")
case "${1:-check}" in
check)
    echo "Current: $current"
    echo "Original: $original"
    echo "Patched: $patched"
    ;;
apply)
    [ "$(hash_file "$here/rknpu_job.c")" = "$patched" ] || { echo 'Package hash mismatch'; exit 1; }
    if [ "$current" = "$patched" ]; then echo 'Already applied'; exit 0; fi
    [ "$current" = "$original" ] || { echo 'Source changed; refusing overwrite'; exit 1; }
    if [ -e "$backup" ]; then
        [ "$(hash_file "$backup")" = "$original" ] || { echo 'Backup mismatch'; exit 1; }
    else
        cp -p "$target" "$backup"
    fi
    cp "$here/rknpu_job.c" "$target"
    echo "Source patched. Backup: $backup"
    ;;
restore)
    [ "$current" = "$patched" ] || { echo 'Current source is not this patch; refusing overwrite'; exit 1; }
    [ -f "$backup" ] && [ "$(hash_file "$backup")" = "$original" ] || { echo 'Original backup missing or changed'; exit 1; }
    cp -p "$backup" "$target"
    echo 'Source restored; installed board module is unchanged'
    ;;
*) echo 'Usage: sh apply.sh check|apply|restore [SDK_DIR]'; exit 2 ;;
esac
