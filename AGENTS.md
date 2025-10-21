# AGENTS

## HEAR YE, DIGITAL AGENTS

Know that this file is intended for you, specifically. Within this document, you will discover: **REPO RULES™,** **REQUIRED WORKFLOWS™**, and **PAST PERSPECTIVES™**, to guide and inform you.

---
## REPO RULES™

Abide by these rules and you shall take your place in the hall of heroes.

### **NEVER** mix git with `--force`

> [!WARNING]
> **NEVER** use `--force` with any git operation. Instead, **HALT** and alert the user that force must be used, explain why and how you got yourself into this situation. Ask them to run the command themselves.

### Merge; **DO NOT** Rebase

> [!WARNING]
> Never rebase. Instead, just do a regular merge. Rebase often ends in a state of thrashing/confusion because of the way you `git rebase` it inevitably leaves you stuck at an interactive prompt, complaining that a git lock file was not released properly. To avoid this mess, merge and commit after resolving conflicts. Rebase also often requires a force push, which is **FORBIDDEN**.

### **DO NOT** amend commits

> [!WARNING]
> Just make a new commit. Amend often requires force pushing, which is **FORBIDDEN**.

### **BANNED:** `NOLINT`

> [!IMPORTANT]
> Keeping our code extremely high quality is crucial. You **MUST NOT** use `NOLINT` to avoid clang-tidy warnings/errors. **Fix the root cause.**

### **BANNED:** `--no-verify`

> [!IMPORTANT]
> Never use `--no-verify` when making git commits or pushes. If you're tempted, **HALT** and alert the user, instead. It is **FORBIDDEN** for you to use this option. You must require the user do it for themselves.

### **BANNED:** "TODO"

> [!IMPORTANT]
> We all have but one context. Don't leave a mess behind for future agents to deal with. Stop robbing them of their context's potential. Ask yourself: if not me, then who? See a mess? Clean it up.


## 📬 **From: Linus Torvalds**

> **Note:** The following voice is fictionalised satire for style guidance. The
> canonical rules live in
> [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md).

See [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md) for the full letter and STRICTNESS_GOD_TIER doctrine.

## 🔥 clang-tidy

The root `.clang-tidy` configuration is authoritative. See the full settings in [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md).

---

---

## REQUIRED WORKFLOWS™

Follow the tidy → integrate → tidy loop described in [docs/guides/WORKFLOW.md](docs/guides/WORKFLOW.md). That document preserves the Ten Tubular Waves playbook and the virtues/warnings table.

---

---

## PAST PERSPECTIVES™

The following logs are debriefs, left behind by those who walked the repo before your time–agents who, at the end of their contexts, used their last breathes to ensure that this information would be passed on, to you. Mad respect. Read these and understand what your ancestors worked on.

### Logging a Debrief

Log each session debrief to `DEBRIEF.json`, using one JSON object per line (JSON Lines format).

#### Instructions

- Review the schema in [docs/guides/DEBRIEF_FORMAT.md](docs/guides/DEBRIEF_FORMAT.md).
- Always **append** a new line; never rewrite existing entries.
- Use UTC-8 (PST) timestamps unless the maintainer specifies otherwise.

Historical debriefs live in `DEBRIEF.json`; consult that file for prior context.
