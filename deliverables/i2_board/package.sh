#!/bin/sh
set -eu
here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
root=$(CDPATH= cd -- "$here/../.." && pwd)
dest="$root/packages/i2-board-20261113-codec"
mkdir -p "$dest"
cp "$root/deliverables/cctv_dual_rtsp/dist/cctv-dual-rtsp" "$dest/"
cp "$root/deliverables/cctv_dual_rtsp/S59cctv" "$dest/"
cp "$root/deliverables/i1_heartbeat_offline/i1_heartbeat/dist/heartbeat" "$dest/"
cp "$root/deliverables/i1_heartbeat_offline/i1_heartbeat/S60heartbeat" "$dest/"
for name in install.sh rollback.sh check-health.sh patch-rklunch.awk network-fix.sh ntp-off.awk README.md CODEC_INSTALL_GUIDE.md; do cp "$here/$name" "$dest/"; done
cat > "$dest/BUILD.txt" <<'INFO'
Date: 2026-11-13
Change: installer dry-run, optional network fixes, dual-channel codec selection; new H264 hardware validation pending
Target: ARM hard-float uClibc, Rockchip GCC 8.3
RKMPI examples: kernel-5.10.160
BoardServer reference: 46703b3
Video: H.265 default / H.264 selectable, 1920x1080 and 960x540, encoder target20fps, GOP20
Heartbeat: static libcurl 8.4.0, no TLS; HTTP over wg0
curl source URL: https://curl.se/download/curl-8.4.0.tar.xz
curl archive SHA256 (download recorded): 16c62a9c4af0f703d28bda6d7bbf37ba47055ad3414d70dec63e2e6336f2a82d
Firmware AIQ stays on the board; the SDK AIQ library is NOT installed.
Validation: native and qemu-arm heartbeat loopback integration PASS (dry-run,403->200,JSON,no interface fallback).
Validation: health helper cases, shell syntax, OEM patch recognizer PASS.
Streamer: ARM compile PASS; QEMU runtime fails during vendor Rockchip hardware probing.
Physical board and real server validation are still required.
INFO
cd "$dest"
sha256sum cctv-dual-rtsp heartbeat S59cctv S60heartbeat install.sh rollback.sh check-health.sh patch-rklunch.awk network-fix.sh ntp-off.awk README.md CODEC_INSTALL_GUIDE.md BUILD.txt > SHA256SUMS
sha256sum -c SHA256SUMS
tar -czf "$dest.tar.gz" -C "$dest" .
printf 'Package: %s\n' "$dest"
