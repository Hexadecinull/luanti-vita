#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

if ! git -C "$REPO_ROOT" rev-parse --git-dir &>/dev/null; then
    echo "ERROR: $REPO_ROOT is not a git repository."
    exit 1
fi

try_apply() {
    local patch="$1"
    local name
    name="$(basename "$patch")"

    if git -C "$REPO_ROOT" apply --check --ignore-whitespace "$patch" 2>/dev/null; then
        git -C "$REPO_ROOT" apply --ignore-whitespace "$patch"
        echo "  [OK]      $name"
    else
        if git -C "$REPO_ROOT" apply --reverse --check --ignore-whitespace "$patch" 2>/dev/null; then
            echo "  [SKIP]    $name  (already applied)"
        else
            echo "  [CONFLICT] $name  — applying with --reject for manual review"
            git -C "$REPO_ROOT" apply --reject --ignore-whitespace "$patch" || true
        fi
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
