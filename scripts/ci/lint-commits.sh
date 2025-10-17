#!/usr/bin/env bash
# Lints all commit messages in the PR using commitlint (conventional commits).

set -euo pipefail

range="$1"  # e.g. "origin/$BASE_REF...$HEAD_SHA"

base_ref="$range"
if [[ "$range" == *...* ]]; then
  base_ref="${range%%...*}"
fi

merge_base="$(git merge-base "$base_ref" "$HEAD_SHA")"

npx --yes @commitlint/cli@18 commitlint --from "$merge_base" --to "$HEAD_SHA"
