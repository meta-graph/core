# Issue Draft: Integrate XTRA DPOI/QCA Skeleton (STRICTNESS_GOD_TIER)

**Title:** Integrate rmg-c-rmg-skeleton-xtra into meta-graph/core under STRICTNESS_GOD_TIER lint discipline  
**Milestone:** `Forge MVP` (captures all work required for the deterministic DPOI/QCA forge loop)  
**Labels:** `enhancement`, `physics-mode`, `lint`, `architecture`

---

## Summary

Land the `rmg-c-rmg-skeleton-xtra` drop (typed ports, seeded VF2 matcher, attachment pushouts, diff-based rollback) into `meta-graph/core`, ensuring every phase passes the root `.clang-tidy` (`STRICTNESS_GOD_TIER_BRUTAL_NO_MERCY`) before and after changes.

Plan lives in `docs/dpoi-qca-integration-plan.md`.  
Immediate workflow loop: **tidy clang → integrate → tidy clang**.

---

## Tasks

1. **Phase 0 – Lint Baseline**
   - Revert/adjust current stubs until `clang-tidy -p build` passes on main branch.
2. **Phase 1 – Structural Types**
   - Add typed interfaces, port caps, attachment update structs, dual epochs.
   - Update rule builders and unit tests.
3. **Phase 2 – Matcher Upgrade**
   - Integrate seeded VF2 (bitsets + SIMD degree filter, non-recursive stack).
   - Enforce node/edge port caps; add rejection tests.
4. **Phase 3 – Attachment Pushouts**
   - Journal attachment offsets/flags; implement diff rollback.
   - Flip attachment/skeleton epochs per publish.
   - Add MG_DEBUG invariants.
5. **Phase 4 – QCA Harmonization**
   - Wire tick loop to new matcher + commit; update metrics/telemetry.
   - Ensure deterministic schedule + halo behavior.
6. **Phase 5 – Final STRICTNESS pass**
   - Run build, tests, and `clang-tidy -p build` (Release + MG_DEBUG).
   - File PR; include before/after epoch + journal telemetry.

Each phase ends with the tidy→integrate→tidy cadence.

---

## Acceptance Criteria

- [ ] `docs/dpoi-qca-integration-plan.md` executed end-to-end.
- [ ] Root `.clang-tidy` passes (no `NOLINT`s added).
- [ ] New unit/integration tests cover dangling rejection, port enforcement, rollback, epoch flips.
- [ ] CLI reports journal stats and dual epochs after each tick.
- [ ] `MG_DEBUG` invariants succeed on debug builds.
- [ ] Forge MVP milestone reflects completion.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md`
- Drop: `rmg-c-rmg-skeleton-xtra.zip`
- Specs: `docs/architecture.md`, `docs/rmg-math.md`, `docs/formal-spec.md`
