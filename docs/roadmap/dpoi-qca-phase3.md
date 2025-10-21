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
- [ ] Provide rollback that discards workspace/journal when a commit fails, restoring attachments and skeleton tables atomically for the whole scheduled batch.  
- [ ] Add MG_DEBUG invariants (symmetry, no orphans, preserved port compliance) and document their O(n) cost.  
- [ ] Update telemetry to include journal stats and both epochs (attachment epoch -> `epoch_att`, skeleton epoch -> `epoch_skel`).  
- [ ] Tidy → integrate → tidy (clang-tidy passes).

---

## Acceptance Criteria

- [ ] Attachment updates behave atomically; rollback restores original state.  
- [ ] Epoch counters `epoch_att` (attachments) and `epoch_skel` (skeleton) flip in the documented order (attachments first).  
- [ ] Debug invariants pass in MG_DEBUG builds and are referenced in documentation.  
- [ ] `clang-tidy -p build` clean.  
- [ ] Phase 3 checked off in tracker.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 3)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
