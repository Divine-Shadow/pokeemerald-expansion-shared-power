#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../.." && pwd)"

if ! command -v cmake >/dev/null 2>&1 || ! command -v git >/dev/null 2>&1; then
    if command -v nix-shell >/dev/null 2>&1; then
        exec nix-shell -p git cmake gcc gnumake pkg-config lua libpng zlib libzip sqlite ffmpeg --run "$0 $*"
    fi
    echo "Missing cmake/git and nix-shell is unavailable" >&2
    exit 1
fi

mgba_commit="${MGBA_COMMIT:-b19b557a78930ede7ee7f5dcbc880f9ff2533ffe}"
mgba_src_dir="${MGBA_SRC_DIR:-$repo_root/build/mgba-headless-src}"
mgba_build_dir="${MGBA_BUILD_DIR:-$repo_root/build/mgba-headless-build}"
jobs="${JOBS:-$(nproc)}"

if [ ! -d "$mgba_src_dir/.git" ]; then
    rm -rf "$mgba_src_dir"
    git clone https://github.com/mgba-emu/mgba.git "$mgba_src_dir"
fi

git -C "$mgba_src_dir" fetch --depth 1 origin "$mgba_commit"
git -C "$mgba_src_dir" checkout --detach "$mgba_commit"

cmake -S "$mgba_src_dir" -B "$mgba_build_dir" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_HEADLESS=ON \
    -DBUILD_QT=OFF \
    -DBUILD_SDL=OFF \
    -DBUILD_TEST=OFF \
    -DBUILD_SUITE=OFF \
    -DBUILD_PERF=OFF \
    -DBUILD_ROM_TEST=OFF \
    -DBUILD_CINEMA=OFF \
    -DBUILD_LIBRETRO=OFF \
    -DBUILD_SHARED=ON \
    -DENABLE_SCRIPTING=ON \
    -DUSE_LUA=ON \
    -DUSE_DISCORD_RPC=OFF \
    -DUSE_FFMPEG=OFF

cmake --build "$mgba_build_dir" --target mgba-headless -j"$jobs"
"$mgba_build_dir/mgba-headless" --help 2>&1 | grep -q -- '--script'

echo "$mgba_build_dir/mgba-headless"
