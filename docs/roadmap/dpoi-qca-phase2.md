# Issue Draft: Phase 2 – Integrate seeded VF2 matcher + port gluing

**Title:** Phase 2 – Seeded VF2 matcher with typed port enforcement  
**Milestone:** `Forge MVP`  
**Labels:** `enhancement`, `physics-mode`, `matcher`

---

## Goal

Replace the stub matcher with the XTRA seeded VF2 implementation, including neighborhood candidate seeding, SIMD degree filtering, and strict node/edge port gluing.

---

## Tasks

- [ ] Implement candidate-generation helpers (bitset neighbourhoods + SIMD degree filter with C fallback).  
- [ ] Write port-compliance helpers (node min/max, edge interface signatures).  
- [ ] Replace matcher with iterative VF2 (explicit stack, no recursion).  
- [ ] Update touched-set/halo logic to match skeleton behaviour.  
- [ ] Add unit tests for dangling rejection, port violations, preserved-edge interface mismatches.  
- [ ] Run tidy → integrate → tidy (clang-tidy before/after).

---

## Acceptance Criteria

- [ ] Matcher passes existing and new tests; produces deterministic schedules.  
- [ ] `clang-tidy -p build` clean.  
- [ ] Tracker updated with Phase 2 completion.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 2)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
