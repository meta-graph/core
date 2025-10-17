#!/bin/bash
# Mirror the CI quality matrix locally on macOS without consuming hosted runners.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DEFAULT_BUILD_ROOT="$ROOT_DIR/build-matrix-local"
BUILD_ROOT="$DEFAULT_BUILD_ROOT"
KEEP_BUILDS=false
CC_BIN="${CC:-clang}"
CXX_BIN="${CXX:-clang++}"

usage() {
  cat <<USAGE
Usage: ${0##*/} [--build-root DIR] [--keep-builds]

Options:
  --build-root DIR  Override build output directory (default: $DEFAULT_BUILD_ROOT)
  --keep-builds     Do not delete the matrix build directory after success

Environment:
  CC, CXX   Override host compilers passed to CMake
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --build-root)
      shift || { usage >&2; exit 1; }
      BUILD_ROOT="$1"
      ;;
    --keep-builds)
      KEEP_BUILDS=true
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      usage >&2
      exit 1
      ;;
  esac
  shift || true
fi

if ! command -v ninja >/dev/null 2>&1; then
  echo "❌ 'ninja' not found. Install via 'brew install ninja'" >&2
  exit 1
fi

printf '📦 Using C compiler: %s\n' "$CC_BIN"
printf '📦 Using C++ compiler: %s\n' "$CXX_BIN"
printf '📁 Build root: %s\n' "$BUILD_ROOT"

mkdir -p "$BUILD_ROOT"

scripts_failed=false
for build_type in Debug Release; do
  if ! BUILD_ROOT="$BUILD_ROOT" CC="$CC_BIN" CXX="$CXX_BIN" \
       "$ROOT_DIR/scripts/run-quality-matrix-leg.sh" "$build_type"; then
    scripts_failed=true
    break
  fi
done

if [[ "$scripts_failed" == false ]]; then
  if [[ "$KEEP_BUILDS" == false ]]; then
    rm -rf "$BUILD_ROOT"
  fi
  printf '\n🎉 Local quality matrix complete (macOS).\n'
  exit 0
else
  echo "❌ Quality matrix failed. Build artifacts retained at $BUILD_ROOT" >&2
  exit 1
fi
