#!/bin/bash
# Run the quality-matrix workflow locally on macOS without consuming GitHub Actions minutes.
# Mirrors the linux + mac clang Debug/Release legs using the host toolchain.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_ROOT="$ROOT_DIR/build-matrix-local"

# clang on mac already defaults to Apple Clang; allow overriding if users installed custom LLVM.
CC_BIN="${CC:-clang}"
CXX_BIN="${CXX:-clang++}"

# Ensure ninja exists – CI assumes it, so we do the same here.
if ! command -v ninja >/dev/null 2>&1; then
  echo "❌ 'ninja' not found. Install via 'brew install ninja'" >&2
  exit 1
fi

configure_and_build() {
  local build_type="$1"
  local build_dir
  build_dir="$BUILD_ROOT/$(echo "$build_type" | tr '[:upper:]' '[:lower:]')"
  local enable_sanitizers="OFF"
  if [[ "$build_type" == "Debug" ]]; then
    enable_sanitizers="ON"
  fi

  printf '\n🛠️  Configuring %s build\n' "$build_type"
  cmake -S "$ROOT_DIR" -B "$build_dir" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DMETAGRAPH_WERROR=ON \
    -DMETAGRAPH_SANITIZERS="$enable_sanitizers" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_C_COMPILER="$CC_BIN" \
    -DCMAKE_CXX_COMPILER="$CXX_BIN"

  printf '🚧 Building %s\n' "$build_type"
  cmake --build "$build_dir" --parallel

  printf '🧪 Running tests (%s)\n' "$build_type"
  ctest --test-dir "$build_dir" --output-on-failure --parallel

  if [[ "$build_type" == "Debug" ]]; then
    printf '🔍 Running clang-tidy (Debug leg)\n'
    (cd "$ROOT_DIR" && ./scripts/run-clang-tidy.sh --check)
  else
    printf '🛡️  Running security audit (Release leg)\n'
    (cd "$ROOT_DIR" && ./scripts/security-audit.sh)
  fi
}

mkdir -p "$BUILD_ROOT"

printf '📦 Using C compiler: %s\n' "$CC_BIN"
printf '📦 Using C++ compiler: %s\n' "$CXX_BIN"

declare -a BUILD_TYPES=("Debug" "Release")
for type in "${BUILD_TYPES[@]}"; do
  configure_and_build "$type"
  printf '✅ Completed %s leg\n' "$type"
done

printf '\n🎉 Local quality matrix complete (macOS).\n'
