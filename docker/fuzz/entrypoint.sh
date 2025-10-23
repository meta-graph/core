#!/usr/bin/env bash

set -euo pipefail

# Allow overriding behaviour by passing a command.
if [[ $# -gt 0 ]]; then
  exec "$@"
fi

SOURCE_DIR=${SOURCE_DIR:-/workspace}
BUILD_DIR=${BUILD_DIR:-${SOURCE_DIR}/.docker-fuzz/build}
CMAKE_GENERATOR=${CMAKE_GENERATOR:-Ninja}
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Debug}
FUZZ_CFLAGS=${FUZZ_CFLAGS:--fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer}
FUZZ_LDFLAGS=${FUZZ_LDFLAGS:--fsanitize=fuzzer,address,undefined}
FUZZ_TARGETS=${FUZZ_TARGETS:-auto}
FUZZ_MAX_TOTAL_TIME=${FUZZ_MAX_TOTAL_TIME:-300}
FUZZ_TIMEOUT=${FUZZ_TIMEOUT:-30}
CORPUS_ROOT_DEFAULT="${HOME}/corpus"
CORPUS_ROOT=${FUZZ_CORPUS_ROOT:-${CORPUS_ROOT_DEFAULT}}

read -ra EXTRA_CMAKE_ARGS <<< "${EXTRA_CMAKE_ARGS:-}"
read -ra LIBFUZZER_ARGS <<< "${LIBFUZZER_ARGS:-}"

if [[ ! -d "${SOURCE_DIR}" ]]; then
  echo "[fuzz-entrypoint] SOURCE_DIR ${SOURCE_DIR} does not exist. Mount the repo at /workspace." >&2
  exit 1
fi

echo "[fuzz-entrypoint] Configuring fuzz build in ${BUILD_DIR}"
cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" -G "${CMAKE_GENERATOR}" \
  -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
  -DMETAGRAPH_FUZZING=ON \
  -DMETAGRAPH_SANITIZERS=ON \
  -DMETAGRAPH_ASAN=ON \
  -DMETAGRAPH_UBSAN=ON \
  -DCMAKE_C_FLAGS="${FUZZ_CFLAGS}" \
  -DCMAKE_EXE_LINKER_FLAGS="${FUZZ_LDFLAGS}" \
  "${EXTRA_CMAKE_ARGS[@]}"

mapfile -t AVAILABLE_FUZZ_TARGETS < <(
  ninja -C "${BUILD_DIR}" -t targets all 2>/dev/null \
    | awk '{print $1}' \
    | sed 's/:$//' \
    | grep '^fuzz-' || true
)

if [[ ${#AVAILABLE_FUZZ_TARGETS[@]} -eq 0 ]]; then
  echo "[fuzz-entrypoint] No CMake targets prefixed with fuzz- were generated." >&2
  echo "[fuzz-entrypoint] Add fuzz targets (e.g. add_executable(fuzz-foo ...)) to enable the rig." >&2
  exit 0
fi

declare -a SELECTED_TARGETS
if [[ "${FUZZ_TARGETS}" == "auto" ]]; then
  SELECTED_TARGETS=("${AVAILABLE_FUZZ_TARGETS[@]}")
else
  read -ra REQUESTED_TARGETS <<< "${FUZZ_TARGETS}"
  for target in "${REQUESTED_TARGETS[@]}"; do
    if printf '%s\n' "${AVAILABLE_FUZZ_TARGETS[@]}" | grep -qx "${target}"; then
      SELECTED_TARGETS+=("${target}")
    elif printf '%s\n' "${AVAILABLE_FUZZ_TARGETS[@]}" | grep -qx "fuzz-${target}"; then
      SELECTED_TARGETS+=("fuzz-${target}")
    else
      echo "[fuzz-entrypoint] Requested CMake target '${target}' not found; skipping." >&2
    fi
  done
fi

if [[ ${#SELECTED_TARGETS[@]} -eq 0 ]]; then
  echo "[fuzz-entrypoint] No fuzz targets selected for build." >&2
  exit 0
fi

echo "[fuzz-entrypoint] Building targets: ${SELECTED_TARGETS[*]}"
cmake --build "${BUILD_DIR}" --target "${SELECTED_TARGETS[@]}"

BIN_ROOT="${BUILD_DIR}/tests/fuzz"
declare -a TARGET_BINARIES
for cmake_target in "${SELECTED_TARGETS[@]}"; do
  candidate="${BIN_ROOT}/${cmake_target}"
  if [[ -x "${candidate}" ]]; then
    TARGET_BINARIES+=("${candidate}")
    continue
  fi
  alt="${BIN_ROOT}/${cmake_target#fuzz-}"
  if [[ -x "${alt}" ]]; then
    TARGET_BINARIES+=("${alt}")
    continue
  fi
  match=$(find "${BUILD_DIR}" -type f -perm -111 -name "${cmake_target}" -o -name "${cmake_target#fuzz-}" | head -n 1)
  if [[ -n "${match}" ]]; then
    TARGET_BINARIES+=("${match}")
  else
    echo "[fuzz-entrypoint] Built target ${cmake_target} but could not locate the binary; skipping." >&2
  fi
done

if [[ ${#TARGET_BINARIES[@]} -eq 0 ]]; then
  echo "[fuzz-entrypoint] Fuzz targets built, but no executables located." >&2
  exit 0
fi

if ! mkdir -p "${CORPUS_ROOT}"; then
  local_fallback="${CORPUS_ROOT_DEFAULT}"
  if [[ "${CORPUS_ROOT}" != "${local_fallback}" ]]; then
      echo "[fuzz-entrypoint] Unable to write to ${CORPUS_ROOT}; falling back to ${local_fallback}" >&2
  fi
  CORPUS_ROOT="${local_fallback}"
  mkdir -p "${CORPUS_ROOT}"
fi
echo "[fuzz-entrypoint] Using corpus root ${CORPUS_ROOT}"

PARALLEL_JOBS=$(nproc)

for bin_path in "${TARGET_BINARIES[@]}"; do
  target_name=$(basename "${bin_path}")
  corpus_dir="${CORPUS_ROOT}/${target_name}"
  mkdir -p "${corpus_dir}"

  echo "[fuzz-entrypoint] Running ${target_name} with corpus ${corpus_dir}"
  "${bin_path}" "${corpus_dir}" \
    -max_total_time="${FUZZ_MAX_TOTAL_TIME}" \
    -timeout="${FUZZ_TIMEOUT}" \
    -print_final_stats=1 \
    -artifact_prefix="${corpus_dir}/crash-" \
    -jobs="${PARALLEL_JOBS}" \
    -workers="${PARALLEL_JOBS}" \
    "${LIBFUZZER_ARGS[@]}"
done

echo "[fuzz-entrypoint] Fuzzing session complete. Artifacts stored under ${CORPUS_ROOT}."
