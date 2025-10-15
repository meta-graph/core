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
- [ ] Ensure kernel application order follows deterministic key ordering.  
- [ ] Update metrics (matches found/kept, conflicts dropped, journal stats, timings, epochs).  
- [ ] Integrate CLI output for journal and epoch telemetry.  
- [ ] Add integration tests (`t1`, `t2`) covering deterministic MIS + halo behaviour under the new pipeline.  
- [ ] Tidy → integrate → tidy (`clang-tidy -p build`).

---

## Acceptance Criteria

- [ ] QCA tick produces identical results across runs (given same seed).  
- [ ] Metrics/telemetry reflect new data (journal counts, attachment/skeleton epochs).  
- [ ] Integration tests pass in Release and MG_DEBUG modes.  
- [ ] Tracker updated for Phase 4.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 4)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
