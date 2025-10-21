# Issue Draft: Phase 4 – QCA harmonization + telemetry

**Title:** Phase 4 – Harmonize QCA tick with new matcher/commit pipeline  
**Milestone:** `Forge MVP`  
**Labels:** `enhancement`, `physics-mode`, `telemetry`

---

## Goal

Wire the QCA tick loop to the upgraded matcher and commit engine, ensuring deterministic scheduling, updated metrics, and debug-only invariants.

---

## Tasks

- [ ] Refactor tick loop to reuse arena-allocated match buffers and diff lists.  
- [ ] Ensure kernel application order follows deterministic key ordering (key_hi/key_lo, then insertion order).  
- [ ] Feed a deterministic RNG seed into tick (CLI/API) and propagate it through scheduling.  
- [ ] Update metrics (matches found/kept, conflicts dropped, journal stats, timings, attachment/skeleton epochs) and document JSON schema.  
- [ ] Integrate CLI output for journal and epoch telemetry (JSON lines with timestamp + seed).  
- [ ] Add integration tests (`t1`, `t2`) covering deterministic MIS + halo behaviour under the new pipeline.  
- [ ] Tidy → integrate → tidy (`clang-tidy -p build`).

---

## Acceptance Criteria

- [ ] QCA tick produces identical results across runs (given same seed).  
- [ ] Metrics/telemetry reflect new data (journal counts, attachment/skeleton epochs, RNG seed) with schema documented.  
- [ ] MG_DEBUG invariants list (halo preservation, epoch ordering) is documented and passes in CI.  
- [ ] Integration tests pass in Release and MG_DEBUG modes.  
- [ ] Tracker updated for Phase 4.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 4)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
