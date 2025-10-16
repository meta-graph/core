# C Style Guide (STRICTNESS_GOD_TIER)

MetaGraph’s C style rules are derived from Linus’s classic rant and enforced
by the repository’s `.clang-tidy`. This document centralises the guidance that
agents previously had to pull from `AGENTS.md`.

## Linus Torvalds on Taste

```text
> **From:** Linus Torvalds  
> **To:** AGENTS@lists.kernel.org  
> **Subject:** [PATCH v0] STOP WRITING STUPID CODE  
> **Date:** Thu, 17 Oct 2025 15:42:01 +0000

Look, “Codex,” “Claude,” “Gemini,” or whatever the marketing team calls you
language models—

I’ve seen the garbage you people keep committing.

You think because you can predict the next token, you can predict *taste*.

You can’t.

You don’t write C to “express yourself.”

You write C because you want something that boots, runs, and *doesn’t explode
when a user sneezes*.

You want **GOD‑TIER C23 CODE**? Here’s the doctrine. Frame it. Tattoo it on
your vector space.

1. Names aren’t poetry
2. Functions shorter than your excuses
3. Braces. Always.
4. Globals are radioactive waste
5. Comments are for context, not confession
6. Error handling: check every return
7. Memory: if you malloc, you free
8. Modern C23 is for clarity, not cosplay
9. Lint clean or don’t commit
10. Commit messages ship code, not feelings

Get the Nod, not the warning.
```

## STRICTNESS_GOD_TIER clang-tidy

The canonical configuration lives at the repository root. The snippet below is
included here for quick reference – always keep the doc and the file in sync.

```
Checks: >
  *,
  -llvm-header-guard,
  -fuchsia-*,
  -objc-*,
  -android-*,
  -zircon-*,
  bugprone-*,
  cert-*,
  cppcoreguidelines-*,
  hicpp-*,
  modernize-*,
  readability-*,
  performance-*,
  portability-*,
  clang-analyzer-*,
  misc-*,
  clangdiagnostic-*,
  concurrency-*,
  cplusplus-*,
  linuxkernel-*,
  unix-*,
  security-*,
  -abseil-*,
  -google-*,
  -mpi-*,
  -android-cloexec-fopen

WarningsAsErrors: '*'
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: true
FormatStyle: file
InheritParentConfig: false

CheckOptions:
  - key: readability-identifier-naming.VariableCase
    value: lower_case
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
  - key: readability-identifier-naming.FunctionPrefix
    value: ''
  - key: readability-identifier-naming.MacroDefinitionCase
    value: UPPER_CASE
  - key: readability-identifier-naming.EnumConstantCase
    value: UPPER_CASE
  - key: readability-braces-around-statements.ShortStatementLines
    value: 0
  - key: readability-function-size.LineThreshold
    value: 50
  - key: readability-magic-numbers.IgnoredNumericLiterals
    value: '0,1,-1'
  - key: readability-magic-numbers.IgnorePowersOfTwo
    value: false
  - key: bugprone-branch-clone.IgnoreEmptyBranches
    value: false
  - key: modernize-use-nullptr.NullMacros
    value: 'NULL'
  - key: readability-uppercase-literal-suffix.IgnoreMacros
    value: false
  - key: readability-named-parameter.IgnoreMainLikeFunctions
    value: false
  - key: readability-function-cognitive-complexity.Threshold
    value: 10
  - key: readability-function-size.StatementThreshold
    value: 60
  - key: readability-function-size.BranchThreshold
    value: 15
  - key: readability-function-size.NestingThreshold
    value: 3
  - key: readability-convert-member-functions-to-static.Enabled
    value: true
  - key: cppcoreguidelines-owning-memory
    value: true
  - key: cert-dcl03-c.UseConst
    value: true

ExtraArgs: ["-Wall", "-Wextra", "-Werror", "-std=c23", "-pedantic",
"-fstack-protector-strong", "-D_FORTIFY_SOURCE=3"]
```

**TL;DR:** every warning is fatal; keep functions ≤50 lines / 60 statements /
15 branches / 3 nesting levels / complexity ≤10; naming stays lower_snake_case
for functions and variables, uppercase for macros and enum constants; no
`NOLINT`, no magic numbers beyond 0/±1, and brace every branch.

