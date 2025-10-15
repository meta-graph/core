# Issue Draft: Phase 0 – Restore STRICTNESS_GOD_TIER lint baseline

**Title:** Phase 0 – Restore STRICTNESS_GOD_TIER lint baseline (tidy → integrate → tidy)  
**Milestone:** `Forge MVP`  
**Labels:** `lint`, `cleanup`, `physics-mode`

---

## Goal

Bring the current branch back to a zero-warning state under the repo’s `.clang-tidy` (STRICTNESS_GOD_TIER_BRUTAL_NO_MERCY) before any integration work begins.

---

## Tasks

- [ ] Remove/adjust experimental matcher/QCA code that violates the stricter lint profile.  
- [ ] Regenerate build files if necessary (`cmake .. -DCMAKE_BUILD_TYPE=Release`).  
- [ ] Run `cmake --build build` and `clang-tidy -p build`; fix every reported issue.  
- [ ] Optionally add a CI step (or local script) that runs the stricter clang-tidy automatically.  
- [ ] Document the clean baseline status in the tracker issue.

---

## Acceptance Criteria

- [ ] `clang-tidy -p build` reports zero warnings.  
- [ ] No `NOLINT` annotations introduced.  
- [ ] Tracker issue updated with completion status.  
- [ ] Ready to proceed to Phase 1.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 0)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`
