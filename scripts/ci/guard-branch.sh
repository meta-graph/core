#!/usr/bin/env bash
# Fail-fast rules for source/target branch combinations.

set -euo pipefail

SRC="$1"   # head ref
DST="$2"   # base ref

die() { echo "::error::$*"; exit 1; }

case "$SRC" in
  coderabbitai/*)
    [[ "$DST" == "main" ]] \
      || die "coderabbitai/* branches must target main."
    ;;
  feat/minimal-dpoi-qca-loop)
    # Temporary allowance while PR #70 lands Phase 1 directly onto main.
    [[ "$DST" == "main" ]] \
      || die "feat/minimal-dpoi-qca-loop must target main during Phase 1.";
    ;;
  release/v*)
    [[ "$DST" == "main" ]] || die "release/* must target main."
    ;;
  fix/*)
    true ;; # fix/* can target anything
  feat/*)
    [[ "$DST" == release/v* ]] \
      || die "feat/* must target a release/vX.Y.Z branch."
    [[ "$SRC" =~ ^feat/[0-9]+-[a-z0-9._-]+$ ]] \
      || die "feat/* name must be feat/{issue#}-{slug}."
    ;;
  *)
    die "Branch must begin with release/, fix/, or feat/."
    ;;
esac

if [[ "$DST" == "main" && ! "$SRC" =~ ^(release|fix)/ ]]; then
  [[ "$SRC" == "feat/minimal-dpoi-qca-loop" || "$SRC" =~ ^coderabbitai/ ]] \
    || die "Only release/*, fix/*, coderabbitai/*, or feat/minimal-dpoi-qca-loop may target main."
fi
