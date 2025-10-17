#!/bin/bash
# Execute a single quality-matrix leg (Debug or Release).
# Can be invoked directly or via higher level wrappers.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

usage() {
  cat <<USAGE
Usage: ${0##*/} <Debug|Release>

Environment:
  BUILD_ROOT   Optional build root directory (default: \$ROOT_DIR/build-matrix-local)
  CC           C compiler (default: clang)
  CXX          C++ compiler (default: clang++)
USAGE
}

if [[ $# -lt 1 ]]; then
  usage >&2
  exit 1
fi

BUILD_TYPE="$1"
case "$BUILD_TYPE" in
  Debug|Release) ;;
  *)
    echo "❌ Unknown build type: $BUILD_TYPE" >&2
    usage >&2
    exit 1
    ;;
esac

BUILD_ROOT="${BUILD_ROOT:-$ROOT_DIR/build-matrix-local}"
CC_BIN="${CC:-clang}"
CXX_BIN="${CXX:-clang++}"

BUILD_DIR_NAME=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')
BUILD_DIR="$BUILD_ROOT/$BUILD_DIR_NAME"

mkdir -p "$BUILD_DIR"

SANITIZERS="OFF"
if [[ "$BUILD_TYPE" == "Debug" ]]; then
  SANITIZERS="ON"
fi

printf '\n🛠️  Configuring %s build (%s)\n' "$BUILD_TYPE" "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DMETAGRAPH_WERROR=ON \
  -DMETAGRAPH_SANITIZERS="$SANITIZERS" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER="$CC_BIN" \
  -DCMAKE_CXX_COMPILER="$CXX_BIN"

printf '🚧 Building %s\n' "$BUILD_TYPE"
cmake --build "$BUILD_DIR" --parallel

printf '🧪 Running tests (%s)\n' "$BUILD_TYPE"
ctest --test-dir "$BUILD_DIR" --output-on-failure --parallel

if [[ "$BUILD_TYPE" == "Debug" ]]; then
  printf '🔍 Running clang-tidy (Debug leg)\n'
  MG_TIDY_BUILD_DIR="$BUILD_DIR" \
    (cd "$ROOT_DIR" && ./scripts/run-clang-tidy.sh --check)
else
  printf '🛡️  Running security audit (Release leg)\n'
  MG_BUILD_DIR="$BUILD_DIR" \
    (cd "$ROOT_DIR" && ./scripts/security-audit.sh)
fi

printf '✅ Completed %s leg\n' "$BUILD_TYPE"
