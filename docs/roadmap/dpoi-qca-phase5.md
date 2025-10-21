# Issue Draft: Phase 5 – Final STRICTNESS_GOD_TIER sweep

**Title:** Phase 5 – Final build/test/clang-tidy sweep for DPOI/QCA integration  
**Milestone:** `Forge MVP`  
**Labels:** `lint`, `verification`, `physics-mode`

---

## Goal

After phases 0–4 are complete, run the full verification gauntlet (builds, tests, STRICTNESS_GOD_TIER clang-tidy) and prepare the final PR.

---

## Tasks

- [ ] Build Release + MG_DEBUG configurations (`cmake --build build`, optional `-DMG_DEBUG=ON`).  
- [ ] Run `ctest --test-dir build --output-on-failure`.  
- [ ] Run `clang-tidy -p build` across `include/` and `src/`.  
- [ ] Summarize results (metrics impact, new tests, invariant status) in PR description.  
- [ ] Update tracker + roadmap to mark milestone completion.

---

## Acceptance Criteria

- [ ] All builds/tests pass.  
- [ ] Zero clang-tidy warnings.  
- [ ] Tracker issue closed and Forge MVP milestone progress reflects completion.  
- [ ] PR ready for review with documentation links and metrics.

---

## References

- Plan: `docs/dpoi-qca-integration-plan.md` (Phase 5)  
- Parent tracker: `docs/roadmap/dpoi-qca-tracker.md`
