#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build-vita"
PSVITA_DIR="$REPO_ROOT/psvita"

if [[ -z "${VITASDK:-}" ]]; then
    echo "ERROR: VITASDK environment variable is not set."
    echo "       Install vitasdk and set VITASDK=/usr/local/vitasdk (or wherever it lives)."
    exit 1
fi

TOOLCHAIN="$VITASDK/share/vita.toolchain.cmake"
if [[ ! -f "$TOOLCHAIN" ]]; then
    echo "ERROR: vita.toolchain.cmake not found at: $TOOLCHAIN"
    exit 1
fi

VITA_CMAKE="$VITASDK/share/vita.cmake"
if [[ ! -f "$VITA_CMAKE" ]]; then
    echo "ERROR: vita.cmake not found at: $VITA_CMAKE"
    exit 1
fi

check_dep() {
    if ! command -v "$1" &>/dev/null; then
        echo "ERROR: '$1' not found in PATH. Please install it."
        exit 1
    fi
}

check_dep cmake
check_dep make
check_dep patch
check_dep python3
check_dep curl

apply_patch() {
    local patchfile="$1"
    if patch -d "$REPO_ROOT" -p1 --dry-run --silent < "$patchfile" 2>/dev/null; then
        patch -d "$REPO_ROOT" -p1 < "$patchfile"
        echo "  Applied: $(basename "$patchfile")"
    elif patch -d "$REPO_ROOT" -p1 --dry-run --reverse --silent < "$patchfile" 2>/dev/null; then
        echo "  Skipped (already applied): $(basename "$patchfile")"
    else
        echo "  WARNING: conflict in $(basename "$patchfile") — skipping"
    fi
}

echo "==> Patching src/config.h and creating system header shims..."
cat > "$REPO_ROOT/src/config.h" << 'EOF'
#pragma once
#define HAVE_CONFIG_H 1
#define BUILD_CLIENT 1
#define BUILD_SERVER 0
#define CHECK_CLIENT_BUILD() 1
#define PROJECT_NAME "luanti"
#define PROJECT_NAME_C "Luanti"
#define VERSION_MAJOR 5
#define VERSION_MINOR 16
#define VERSION_PATCH 0
#define VERSION_STRING "5.16.0-vita"
#define USE_GLES2 1
#define HAVE_OPENGL 1
#define RUN_IN_PLACE 1
#define BUILD_DIR_PATH "ux0:data/luanti"
EOF

if [[ ! -f "$VITASDK/arm-vita-eabi/include/endian.h" ]]; then
    cat > "$VITASDK/arm-vita-eabi/include/endian.h" << 'EOF'
#pragma once
#include <sys/endian.h>
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __BYTE_ORDER
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
EOF
fi

echo "==> Renaming src/util/string.h to avoid system header shadowing..."
if [[ -f "$REPO_ROOT/src/util/string.h" ]]; then
    mv "$REPO_ROOT/src/util/string.h" "$REPO_ROOT/src/util/strutil.h"
    find "$REPO_ROOT/src/" "$REPO_ROOT/irr/" -type f \( -name "*.h" -o -name "*.cpp" -o -name "*.c" \) \
        -exec grep -l 'util/string\.h' {} \; \
        | xargs -r sed -i 's|util/string\.h|util/strutil.h|g'
fi

echo "==> Applying source patches..."
for p in "$SCRIPT_DIR"/*.patch; do
    [[ -f "$p" ]] || continue
    apply_patch "$p"
done

echo "==> Checking vitasdk libraries..."
VLIB="$VITASDK/arm-vita-eabi/lib"
VINC="$VITASDK/arm-vita-eabi/include"

missing_libs=()

for lib in libSDL2.a libvitaGL.a libGLESv2.a libEGL.a libGL.a \
           libfreetype.a libpng16.a libjpeg.a libz.a \
           libvorbisfile.a libvorbis.a libogg.a; do
    [[ -f "$VLIB/$lib" ]] || missing_libs+=("$lib")
done

if [[ ${#missing_libs[@]} -gt 0 ]]; then
    echo "WARNING: The following vitasdk libraries are missing:"
    for l in "${missing_libs[@]}"; do
        echo "  - $l"
    done
    echo ""
    echo "Install them via vdpm or vitasdk package manager before linking."
    echo "Continuing with cmake configure step anyway..."
    echo ""
fi

echo "==> Configuring CMake for PS Vita..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake "$PSVITA_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release \
    -DVITASDK="$VITASDK" \
    -G "Unix Makefiles" \
    "$@"

echo ""
echo "==> Building..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 2)
make -j"$NPROC"

echo ""
if [[ -f "$BUILD_DIR/luanti.vpk" ]]; then
    echo "==> Build successful!"
    echo "    VPK: $BUILD_DIR/luanti.vpk"
    echo ""
    echo "    Transfer to Vita:"
    echo "      curl -T $BUILD_DIR/luanti.vpk ftp://<vita-ip>:1337/ux0:/"
    echo ""
    echo "    Or set PSVITAIP and run: make -C $BUILD_DIR send"
else
    echo "ERROR: luanti.vpk not found after build."
    exit 1
fi
