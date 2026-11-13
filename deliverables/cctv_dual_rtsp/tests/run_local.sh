#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
mkdir -p "$root/build/tests"
${CC:-cc} -std=gnu11 -Wall -Wextra -Werror -I"$root/src" "$root/tests/test_config.c" "$root/src/config.c" -o "$root/build/tests/test_config"
"$root/build/tests/test_config"
echo 'PASS: production options, retry parsing, diagnostics, invalid combinations, stream profiles'

${CC:-cc} -std=gnu11 -Wall -Wextra -Werror -I"$root/src" "$root/tests/test_capture.c" "$root/src/capture.c" -o "$root/build/tests/test_capture"
"$root/build/tests/test_capture"
