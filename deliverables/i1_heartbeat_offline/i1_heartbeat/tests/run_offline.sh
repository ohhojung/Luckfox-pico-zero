#!/bin/sh
set -eu
project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mkdir -p "$project_dir/build/tests"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror \
    "$project_dir/tests/test_protocol.c" "$project_dir/src/protocol.c" \
    -o "$project_dir/build/tests/test_protocol"
"$project_dir/build/tests/test_protocol"
