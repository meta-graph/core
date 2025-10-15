# Issue Draft: Phase 1 – Import structural types (ports, attachments, epochs)

**Title:** Phase 1 – Import structural types for DPOI/QCA integration  
**Milestone:** `Forge MVP`  
**Labels:** `enhancement`, `architecture`, `physics-mode`

---

## Goal

Introduce the data structures required by the XTRA drop (typed interfaces, attachment updates, dual epochs) without altering runtime behaviour yet.

---

## Tasks

- [ ] Add port direction enums, `mg_iface_sig_t`, and `mg_edge_ifc_t` to the RMG headers (rename to match MetaGraph naming).  
- [ ] Extend `mg_rule_t` with node port caps and preserved-edge interface signatures; update rule helper builders/tests.  
- [ ] Add attachment update structs (`metagraph_att_update_t` or equivalent) and second epoch counter for attachments.  
- [ ] Ensure unit tests cover struct initialization defaults.  
- [ ] Run tidy → integrate → tidy: `clang-tidy -p build` before and after changes.

---

## Acceptance Criteria

- [ ] New types compile without triggering clang-tidy.  
- [ ] No behavioural changes yet (CI tests pass unchanged).  
- [ ] Tracker issue updated with Phase 1 completion.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 1)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`  
- Skeleton drop: `rmg-c-rmg-skeleton-xtra.zip`
