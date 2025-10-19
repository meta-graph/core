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

The canonical configuration lives at the repository root in `.clang-tidy`. The
snippet below mirrors that file for quick reference; if this doc and the root
file ever diverge, treat the root as the source of truth.

```yaml
Checks: >
  -*,
  bugprone-*,
  cert-*,
  clang-analyzer-*,
  concurrency-*,
  misc-*,
  performance-*,
  portability-*,
  readability-*,
  -readability-magic-numbers,
  -bugprone-easily-swappable-parameters

WarningsAsErrors: '*'
HeaderFilterRegex: '(include|src)/.*\.(h|c)$'

CheckOptions:
  - key: readability-identifier-naming.TypedefCase
    value: lower_case
  - key: readability-identifier-naming.TypedefSuffix
    value: '_t'
  - key: readability-identifier-naming.StructCase
    value: lower_case
  - key: readability-identifier-naming.UnionCase
    value: lower_case
  - key: readability-identifier-naming.EnumCase
    value: lower_case
  - key: readability-identifier-naming.EnumConstantCase
    value: UPPER_CASE
  - key: readability-identifier-naming.EnumConstantPrefix
    value: 'METAGRAPH_'
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
  - key: readability-identifier-naming.FunctionPrefix
    value: ''
  - key: readability-identifier-naming.VariableCase
    value: lower_case
  - key: readability-identifier-naming.ParameterCase
    value: lower_case
  - key: readability-identifier-naming.MacroDefinitionCase
    value: UPPER_CASE
  - key: readability-identifier-naming.MacroDefinitionPrefix
    value: 'METAGRAPH_'
  - key: readability-identifier-naming.GlobalConstantCase
    value: UPPER_CASE
  - key: readability-identifier-naming.GlobalConstantPrefix
    value: 'METAGRAPH_'
  - key: readability-function-cognitive-complexity.Threshold
    value: '25'
  - key: readability-function-size.LineThreshold
    value: '50'
  - key: readability-function-size.StatementThreshold
    value: '60'
  - key: readability-function-size.BranchThreshold
    value: '15'
  - key: readability-function-size.ParameterThreshold
    value: '8'
  - key: readability-function-size.NestingThreshold
    value: '5'
  - key: bugprone-suspicious-string-compare.WarnOnImplicitComparison
    value: true
  - key: bugprone-suspicious-string-compare.WarnOnLogicalNotComparison
    value: true
  - key: cert-err33-c.CheckedFunctions
    value: '::aligned_alloc;::calloc;::clock;::fclose;::ferror;::fflush;::fgetc;::fgetpos;::fgets;::fgetwc;::fopen;::fprintf;::fputc;::fputs;::fputwc;::fread;::freopen;::fscanf;::fseek;::fsetpos;::ftell;::fwprintf;::fwrite;::fwscanf;::getc;::getchar;::gets;::getwc;::getwchar;::gmtime;::localtime;::malloc;::mbrtowc;::mbsrtowcs;::mbstowcs;::memchr;::mktime;::printf;::putc;::putchar;::puts;::putwc;::putwchar;::raise;::realloc;::remove;::rename;::scanf;::setlocale;::setvbuf;::signal;::snprintf;::sprintf;::sscanf;::strchr;::strerror_s;::strftime;::strpbrk;::strrchr;::strstr;::strtod;::strtof;::strtoimax;::strtok;::strtol;::strtoll;::strtoul;::strtoull;::strtoumax;::strxfrm;::swprintf;::swscanf;::time;::tmpfile;::tmpnam;::ungetc;::ungetwc;::vfprintf;::vfscanf;::vfwprintf;::vfwscanf;::vprintf;::vscanf;::vsnprintf;::vsprintf;::vsscanf;::vswprintf;::vswscanf;::vwprintf;::vwscanf;::wcrtomb;::wcschr;::wcsftime;::wcspbrk;::wcsrchr;::wcsrtombs;::wcsstr;::wcstod;::wcstof;::wcstoimax;::wcstok;::wcstol;::wcstoll;::wcstombs;::wcstoul;::wcstoull;::wcstoumax;::wcsxfrm;::wctob;::wmemchr;::wprintf;::wscanf'
  - key: performance-move-const-arg.CheckTriviallyCopyableMove
    value: false
  - key: performance-no-automatic-move.AllowedTypes
    value: ''
  - key: modernize-replace-auto-ptr.IncludeStyle
    value: google
  - key: modernize-use-auto.MinTypeNameLength
    value: '5'
  - key: modernize-use-auto.RemoveStars
    value: false
  - key: portability-restrict-system-includes.Includes
    value: '*'
  - key: misc-misplaced-const.CheckPrimitiveCasts
    value: true
```

**TL;DR:** every warning is fatal; keep functions ≤50 lines / 60 statements /
15 branches / 3 nesting levels / complexity ≤10; naming stays lower_snake_case
for functions and variables, uppercase for macros and enum constants; no
`NOLINT`, no magic numbers beyond 0/±1, and brace every branch.
