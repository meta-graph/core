#!/usr/bin/env bash
# Lints all commit messages in the PR using commitlint (conventional commits).

set -euo pipefail

range="$1"  # e.g. "origin/$BASE_REF...$HEAD_SHA"

base_ref="$range"
if [[ "$range" == *...* ]]; then
  base_ref="${range%%...*}"
fi

if ! git rev-parse --verify --quiet "$base_ref" >/dev/null; then
  if git rev-parse --verify --quiet "origin/$base_ref" >/dev/null; then
    base_ref="origin/$base_ref"
  fi
fi

merge_base="$(git merge-base "$base_ref" "$HEAD_SHA")"

npx --yes @commitlint/cli@18 --extends @commitlint/config-conventional \
  --from "$merge_base" --to "$HEAD_SHA"
