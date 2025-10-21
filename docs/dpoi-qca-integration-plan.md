# DPOI/QCA Integration Plan (STRICTNESS_GOD_TIER compliant)

Last updated: 2025-10-15 17:35 PDT  
Author: Codex agent (feat/minimal-dpoi-qca-loop)

This document tracks how we will merge the `rmg-c-rmg-skeleton-xtra` drop into `meta-graph/core` without violating the STRICTNESS_GOD_TIER clang-tidy regimen.

---

## Guiding Constraints

1. **Lint first, lint last** – every stage runs `clang-tidy -p build` under the repo’s root `.clang-tidy`. No `NOLINT` exceptions permitted.
2. **Keep functions lean** – target ≤ 50 lines and ≤ 25 statements per helper before landing patches (clang-tidy enforces LineThreshold=50; staying well under avoids churn).
3. **Epoch discipline** – attachment epoch flips immediately after attachment journal publish; skeleton epoch flips only after CSR publish.
4. **Journal → verify → publish** – every pushout goes through diff capture, invariant check (debug mode), then epoch flip.

---

## Phase 0 — Immediate Cleanup (“Tidy clang”)

**Goal:** Restore the current branch to a lint-clean state (STRICTNESS_GOD_TIER) before new code lands.

- [ ] Revert temporary matcher/QCA experiments that clang-tidy can’t validate.
- [ ] Run `cmake --build build` and `clang-tidy -p build` on existing sources; fix violations.
- [ ] Add a CI job (or local script) that runs lint + unit tests under the harder profile.

Deliverable: clean tree + green lint baseline.

---

## Phase 1 — Import Structural Types (no behavior changes)

**Goal:** Introduce the new headers/structs required by the XTRA drop without wiring them into runtime logic yet.

Tasks:
- [ ] Copy `mg_iface_sig_t`, `mg_edge_ifc_t`, and port direction enums into `include/metagraph/rmg.h` (renamed to fit project naming).
- [ ] Extend `mg_rule_t` with node-port caps and edge interface signatures, keeping constructor helpers updated.
- [ ] Add attachment update structs (`mg_att_update_t`) and dual epoch counters (`mg_epoch_t` for skeleton, new `mg_attachment_epoch_t`).
- [ ] Ensure each addition compiles + lint passes (update unit tests for struct initialization).

Deliverable: type definitions available to the rest of the repo, no functional changes yet.

---

## Phase 2 — Matcher Upgrade (VF2 seeded + port gluing)

**Goal:** Swap in the XTRA matcher piece by piece while staying under clang-tidy thresholds.

Steps:
1. Introduce reusable helpers:
   - Candidate seeding (degree-filter + neighborhood bitset) in a `metagraph_vf2_candidates.c`.
   - Port compliance checks (node caps, edge signatures) isolated into <= 40-line functions.
2. Replace the stub matcher with the new VF2 logic, using explicit stacks (no recursion) to satisfy `misc-no-recursion`.
3. Integrate SIMD degree filters via compile-time `#ifdef` guarded helpers (SSE2/NEON) with pure-C fallbacks.
4. Add unit tests:
   - Dangling rejection (delete non-boundary node).
   - Port min/max violation rejection.
   - Edge-interface mismatch rejection.

Deliverable: deterministic, lint-clean matcher with halo/touched sets and port gluing.

---

## Phase 3 — Attachment Pushouts + Journaling

**Goal:** Update the commit pipeline to journal attachments and topology separately, flip epochs accordingly, and support rollback.

Tasks:
- [ ] Introduce adjacency workspace + diff lists (`added_nodes`, `added_edges`, `removed_edges`).
- [ ] Capture attachment updates in `mg_att_update_t` list (old/new offsets & flags).
- [ ] Implement rollback by discarding workspace + restoring attachments when a commit fails (no partial state).
- [ ] Integrate debug-only invariants (`MG_DEBUG`) for symmetry, port preservation, and orphan detection.
- [ ] Add instrumentation outputs (journal stats, epochs) to the CLI.

Tests:
- Attachment pushout modifies offsets/flags correctly.
- Rollback leaves graph + attachments bit-identical.
- Epochs (`epoch_att`, `epoch_skel`) flip only when expected.

Deliverable: fully journaled DPO commit path with guardrails.

---

## Phase 4 — QCA Tick Harmonization

**Goal:** Rewire the tick loop to use the new matcher + commit plumbing, keeping metrics and determinism intact.

Tasks:
- [ ] Stage scratch arenas for matches and diff lists to avoid heap churn.
- [ ] Update metrics (match counts, timings, journal sizes).
- [ ] Ensure kernel application order remains deterministic (sorted by key).
- [ ] Add integration tests (`t1`, `t2`) against the upgraded runtime.

Deliverable: production-quality QCA tick with metrics + epochs + rollback.

---

## Phase 5 — Final STRICTNESS_GOD_TIER pass (“Tidy clang” again)

**Goal:** After functional integration, rerun the full lint/test gauntlet.

- [ ] `cmake --build build` (Release + MG_DEBUG)  
- [ ] `ctest --test-dir build --output-on-failure`  
- [ ] `clang-tidy -p build` over `include/` + `src/`  
- [ ] Submit CI job / review results.

Deliverable: green lint, green tests, ready for PR.

---

## References
- Skeleton drop: `rmg-c-rmg-skeleton-xtra`
- Docs consulted: `docs/architecture.md`, `docs/rmg-math.md`, `docs/formal-spec.md`, `AGENTS.md`

---

Questions or updates? Add them here, then notify the next agent via AGENTS.md debrief. 
