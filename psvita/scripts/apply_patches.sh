#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

if ! command -v patch &>/dev/null; then
    echo "ERROR: 'patch' command not found. Install it (e.g. apt-get install patch)."
    exit 1
fi

try_apply() {
    local patchfile="$1"
    local name
    name="$(basename "$patchfile")"

    if patch -d "$REPO_ROOT" -p1 --dry-run --silent < "$patchfile" 2>/dev/null; then
        patch -d "$REPO_ROOT" -p1 < "$patchfile"
        echo "  [OK]      $name"
    elif patch -d "$REPO_ROOT" -p1 --dry-run --reverse --silent < "$patchfile" 2>/dev/null; then
        echo "  [SKIP]    $name  (already applied)"
    else
        echo "  [CONFLICT] $name — applying with --forward --reject-file for manual review"
        patch -d "$REPO_ROOT" -p1 --forward --reject-file="${patchfile}.rej" < "$patchfile" || true
    fi
}

ORDERED_PATCHES=(
    "$SCRIPT_DIR/root_CMakeLists.patch"
    "$SCRIPT_DIR/porting_h_vita.patch"
    "$SCRIPT_DIR/porting_vita.patch"
    "$SCRIPT_DIR/irr_sdl_vita.patch"
    "$SCRIPT_DIR/game_vita.patch"
)

echo "==> Applying patches to: $REPO_ROOT"
echo ""

for p in "${ORDERED_PATCHES[@]}"; do
    if [[ -f "$p" ]]; then
        try_apply "$p"
    else
        echo "  [MISSING] $p"
    fi
done

echo ""
echo "==> Done. Review any .rej files if conflicts were reported."
