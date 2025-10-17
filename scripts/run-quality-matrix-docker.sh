#!/bin/bash
# Execute the quality matrix inside Docker to mirror CI Linux builds locally.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
COMPOSE_FILE="$ROOT_DIR/docker/matrix/docker-compose.yml"
BUILD_ROOT_CONTAINER="/workspace/build-matrix-docker"
MATRIX_IMAGE="metagraph/matrix:clang18"

usage() {
  cat <<USAGE
Usage: ${0##*/} [--keep-builds]

Options:
  --keep-builds   Preserve Docker build artifacts under $BUILD_ROOT_CONTAINER (default: cleaned)
USAGE
}

KEEP_BUILDS=false
while [[ $# -gt 0 ]]; do
  case "$1" in
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
done

if ! command -v docker >/dev/null 2>&1; then
  echo "❌ Docker is required to run the matrix locally." >&2
  exit 1
fi

compose() {
  if docker compose version >/dev/null 2>&1; then
    docker compose -f "$COMPOSE_FILE" "$@"
  else
    docker-compose -f "$COMPOSE_FILE" "$@"
  fi
}

printf '🐳 Building matrix image (%s)\n' "$MATRIX_IMAGE"
compose build matrix

run_leg() {
  local build_type="$1"
  printf '\n🏃 Running %s leg inside Docker\n' "$build_type"
  compose run --rm \
    --user "$(id -u):$(id -g)" \
    -e BUILD_ROOT="$BUILD_ROOT_CONTAINER" \
    matrix bash -lc "./scripts/run-quality-matrix-leg.sh $build_type"
}

set +e
run_leg Debug
DEBUG_STATUS=$?
if [[ $DEBUG_STATUS -ne 0 ]]; then
  compose down --remove-orphans >/dev/null 2>&1 || true
  exit $DEBUG_STATUS
fi

run_leg Release
RELEASE_STATUS=$?
set -e

if [[ $RELEASE_STATUS -ne 0 ]]; then
  compose down --remove-orphans >/dev/null 2>&1 || true
  exit $RELEASE_STATUS
fi

if [[ $KEEP_BUILDS == false ]]; then
  compose run --rm matrix bash -lc "rm -rf $BUILD_ROOT_CONTAINER" >/dev/null 2>&1 || true
fi

compose down --remove-orphans >/dev/null 2>&1 || true

printf '\n🎉 Docker quality matrix complete.\n'
