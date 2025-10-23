# MetaGraph Fuzzing Rig

This directory contains a ready-to-run Docker setup for building and executing
MetaGraph fuzz targets with libFuzzer. It mirrors the nightly fuzz workflow so
you can iterate locally without depending on GitHub Actions.

## Image contents

- Ubuntu 22.04 base pinned by digest for reproducibility
- Clang/LLVM 18 with the matching `libfuzzer-dev` runtime
- Latest CMake from the Kitware APT repository and distro Ninja
- Non-root `fuzzer` user so crashes cannot take down the host
- `fuzz-entrypoint.sh` helper that configures the project, builds fuzzers, and
  runs them against a corpus directory

## Quick start

From the repository root:

```bash
make fuzz
```

This builds the image (tagged `metagraph/fuzzing-rig:latest`), mounts the
working tree at `/workspace`, runs CMake with `-DMETAGRAPH_FUZZING=ON`, builds
the project, and executes every discovered `build-fuzz/tests/fuzz/fuzz-*`
binary for 20 seconds each by default. Corpus data lives under `/home/fuzzer/corpus`
inside the container (persisted via the `fuzz-corpus` volume). To call the
compose file directly, swap the command for
`docker compose -f docker/fuzz/docker-compose.yml up`.

Corpus files and crash artifacts are stored in the named volume `fuzz-corpus`
and exposed under `corpus/` inside the repository. Stop the container with
`Ctrl+C`; the entrypoint exits cleanly when all fuzzers finish.

## Customising the run

Set environment variables with `docker compose run -e KEY=value fuzz` or by
editing the compose file:

- `FUZZ_TARGETS`: Space-separated list of binaries to run (without the
  `fuzz-` prefix). Default `auto` runs every discovered fuzzer.
- `FUZZ_MAX_TOTAL_TIME`: Seconds to fuzz each target (default `20`).
- `FUZZ_TIMEOUT`: Per-input execution timeout in seconds (default `30`).
- `EXTRA_CMAKE_ARGS`: Additional CMake flags, for example
  `-DMETAGRAPH_DEV=ON -DMETAGRAPH_WERROR=ON`.
- `LIBFUZZER_ARGS`: Extra libFuzzer flags appended to every invocation.
- `FUZZ_CORPUS_ROOT`: Override the on-disk corpus location (defaults to
  `/workspace/corpus`).

To drop into an interactive shell inside the configured environment, override
the entrypoint command:

```bash
make fuzz-rig-shell
```

When you need to inspect results, run `make fuzz-rig-shell` and browse
`/home/fuzzer/corpus`. To discard the corpus volume and stop lingering
containers, run `make fuzz-rig-clean`.

## Notes

- The entrypoint exits successfully even if no fuzz binaries are present. This
  keeps the rig usable before fuzz harnesses are implemented.
- The image mirrors the nightly workflow’s toolchain; keeping them in sync
  helps catch discrepancies quickly.
