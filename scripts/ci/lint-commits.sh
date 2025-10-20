#!/usr/bin/env bash
# Lints all commit messages in the PR using commitlint (conventional commits).
# Requires HEAD_SHA environment variable to point at the PR HEAD commit.

set -euo pipefail

range="$1"  # e.g. "origin/$BASE_REF...$HEAD_SHA"

base_ref="$range"
if [[ "$range" == *...* ]]; then
  base_ref="${range%%...*}"
fi

if ! git rev-parse --verify --quiet "$base_ref" >/dev/null; then
  if git rev-parse --verify --quiet "origin/$base_ref" >/dev/null; then
    base_ref="origin/$base_ref"
  else
    echo "Error: Base ref '$base_ref' not found locally or in origin" >&2
    exit 1
  fi
fi

if [[ -z "$base_ref" ]] || ! git rev-parse --verify --quiet "$base_ref" >/dev/null; then
  echo "Error: Could not resolve valid base ref from range: $range" >&2
  exit 1
fi

merge_base="$(git merge-base "$base_ref" "$HEAD_SHA")"

npx --yes -p @commitlint/cli@18 -p @commitlint/config-conventional@18 commitlint --from "$merge_base" --to "$HEAD_SHA"
