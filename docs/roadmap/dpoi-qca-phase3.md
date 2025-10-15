# Issue Draft: Phase 3 – Attachment pushouts, journaling, epochs

**Title:** Phase 3 – Attachment pushouts with journaling, rollback, and dual epochs  
**Milestone:** `Forge MVP`  
**Labels:** `enhancement`, `physics-mode`, `architecture`

---

## Goal

Implement attachment-aware DPO commits: diff journaling for nodes/edges/attachments, rollback support, attachment-first publish, and dual epoch flips.

---

## Tasks

- [ ] Introduce adjacency workspace with diff lists (added nodes/edges, removed edges).  
- [ ] Capture attachment offset/flag updates in a journal structure.  
- [ ] Apply journal → verify invariants → publish attachments → flip attachment epoch → publish CSR → flip skeleton epoch.  
- [ ] Provide rollback path that discards workspace/journal on failure.  
- [ ] Add MG_DEBUG invariants (symmetry, no orphans, preserved port compliance).  
- [ ] Update telemetry to include journal stats and both epochs.  
- [ ] Tidy → integrate → tidy (clang-tidy passes).

---

## Acceptance Criteria

- [ ] Attachment updates behave atomically; rollback restores original state.  
- [ ] Epoch counters reflect attachment/skeleton publishes.  
- [ ] Debug invariants pass in MG_DEBUG builds.  
- [ ] `clang-tidy -p build` clean.  
- [ ] Phase 3 checked off in tracker.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 3)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
