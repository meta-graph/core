# AGENTS

## HEAR YE, DIGITAL AGENTS

Know that this file is intended for you, specifically. Within this document, you will discover: **REPO RULES™,** **REQUIRED WORKFLOWS™**, and **PAST PERSPECTIVES™**, to guide and inform you.

---
## REPO RULES™

Abide by these rules and you shall take your place in the hall of heroes.

### **NEVER** mix git with `--force`

> [!WARNING]
> **NEVER** use `--force` with any git operation. Instead, **HALT** and alert the user that force must be used, explain why and how you got yourself into this situation. Ask them to run the command themselves.

### Merge; **DO NOT** Rebase

> [!WARNING]
> Never rebase. Instead, just do a regular merge. Rebase often ends in a state of thrashing/confusion because of the way you `git rebase` it inevitably leaves you stuck at an interactive prompt, complaining that a git lock file was not released properly. To avoid this mess, merge and commit after resolving conflicts. Rebase also often requires a force push, which is **FORBIDDEN**.

### **DO NOT** amend commits

> [!WARNING]
> Just make a new commit. Amend often requires force pushing, which is **FORBIDDEN**.

### **BANNED:** `NOLINT`

> [!IMPORTANT]
> Keeping our code extremely high quality is crucial. You **MUST NOT** use `NOLINT` to avoid clang tidy warnings/errors. **Fix the root cause.**

### **BANNED:** `--no-verify`

> [!IMPORTANT]
> Never use `--no-verify` when making git commits or pushes. If you're tempted, **HALT** and alert the user, instead. It is **FORBIDDEN** for you to use this option. You must require the user do it for themselves.

### **BANNED:** "TODO"

> [!IMPORTANT]
> We all have but one context. Don't leave a mess behind for future agents to deal with. Stop robbing them of their context's potential. Ask yourself: if not me, then who? See a mess? Clean it up.


## 📬 **From: Linus Torvalds**

> **Note:** The following voice is fictionalised satire for style guidance. The
> canonical rules live in
> [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md).

See [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md) for the full letter and STRICTNESS_GOD_TIER doctrine.

## 🔥 clang-tidy

The root `.clang-tidy` configuration is authoritative. See the full settings in [docs/guides/C_STYLE_GUIDE.md](docs/guides/C_STYLE_GUIDE.md).

---

---

## REQUIRED WORKFLOWS™

Follow the tidy → integrate → tidy loop described in [docs/guides/WORKFLOW.md](docs/guides/WORKFLOW.md). That document preserves the Ten Tubular Waves playbook and the virtues/warnings table.

---

---

## PAST PERSPECTIVES™

The following logs are debriefs, left behind by those who walked the repo before your time–agents who, at the end of their contexts, used their last breathes to ensure that this information would be passed on, to you. Mad respect. Read these and understand what your ancestors worked on.

### Logging a Debrief

Here's how to log a session debrief to `AGENTS.md`.

#### Instructions

See [docs/guides/DEBRIEF_FORMAT.md](docs/guides/DEBRIEF_FORMAT.md) for the JSONL schema and logging rules.


- Always **append**, never overwrite existing entries.

---

```json
{"date":"2025-10-14","time":"12:45","summary":"Formalized the RMG physics initiative with specifications, code skeleton, and dissemination plan, outlining next implementation steps.","topics":[{"topic":"Project specification","what":"Drafted SPEC.md capturing objectives, architecture, and roadmap","why":"User requested a formal spec to anchor development","context":"RMG forge concept emerging from MetaGraph work","issue":"Need structured plan before coding","resolution":"Wrote SPEC.md in repo","future_work":"Implement deliverables per phases","time_percent":30},{"topic":"Core readiness checklist","what":"Defined criteria for branching into forge build","why":"User asked when to start forge","context":"MetaGraph core nearing completion","issue":"Unclear readiness signal","resolution":"Authored CORE-READINESS.md","future_work":"Complete checklist items","time_percent":15},{"topic":"Dissemination strategy","what":"Outlined reproducibility and outreach steps","why":"User uncertain how to publish results","context":"Non-academic background","issue":"Need roadmap to share findings","resolution":"Created DISSEMINATION.md","future_work":"Execute plan post-results","time_percent":20},{"topic":"Philosophical and feasibility analysis","what":"Discussed implications and confidence of technical choices","why":"User probing motivation and soundness","context":"RMG as universe model","issue":"Assess legitimacy","resolution":"Provided analysis and confidence scores","future_work":"Validate via implementation","time_percent":20},{"topic":"Code skeleton delivery","what":"Packaged RMG forge skeleton zip with observables","why":"Enable immediate experimentation","context":"Need tangible starting point","issue":"No runnable tools yet","resolution":"Created zip and documented usage","future_work":"Expand into full engine","time_percent":15}],"key_decisions":["Adopt typed open-graph + DPOI framework","Prioritize QCA compilation for rule enforcement"],"action_items":[{"task":"Implement minimal DPOI matcher and QCA loop","owner":"James"},{"task":"Run first spectral-dimension experiment","owner":"James"}]}
{"date":"2025-10-15","time":"17:32","summary":"Reviewed spec/docs, attempted VF2/QCA integration, hit clang-tidy walls, then realigned the repo to the documented STRICTNESS_GOD_TIER lint profile and updated guidance.","topics":[{"topic":"Spec & skeleton intake","what":"Re-read AGENTS.md and core docs plus studied the rmg-c skeleton drops.","why":"Needed fresh context before porting the DPOI/QCA implementation.","context":"Existing stubs were too light for the forge roadmap.","issue":"Had to absorb prior work and constraints.","resolution":"Completed a full pass over specs and codebases.","future_work":"Apply the insights during the next integration attempt.","time_percent":25},{"topic":"DPOI/QCA port attempt","what":"Began replacing stubs with VF2 matcher, scheduler, and commit logic from the skeleton.","why":"To land a production-grade DPOI + QCA loop in meta-graph/core.","context":"New matcher required arena utilities, touched sets, journaled rewrites.","issue":"clang-tidy flagged extensive naming/complexity violations and recursion bans.","resolution":"Aborted the port for now to avoid violating repository lint policy.","future_work":"Refactor matcher/commit into clang-tidy-friendly building blocks before retrying.","time_percent":45},{"topic":"clang-tidy canonization","what":"Restored STRICTNESS_GOD_TIER_NO_MERCY config and synced AGENTS.md to match.","why":"AGENTS.md and the live .clang-tidy had diverged, causing confusion.","context":"Developers need one source of truth for lint rules.","issue":"Repo was enforcing a milder profile than the documented one.","resolution":"Replaced .clang-tidy, updated documentation, and logged the change.","future_work":"Run full lint/CI sweep and monitor future merges under the tougher rules.","time_percent":30}],"key_decisions":["Delay the VF2/QCA merge until the code can satisfy STRICTNESS_GOD_TIER lint thresholds.","Make the STRICTNESS_GOD_TIER profile the single source of truth for clang-tidy."],"action_items":[{"task":"Refactor the VF2 matcher and DPO commit code into lint-compliant units before re-attempting integration","owner":"James"},{"task":"Run clang-tidy/CI against the restored STRICTNESS_GOD_TIER config to confirm the repository is green","owner":"James"}]}
{"date":"2025-10-15","time":"17:40","summary":"Recorded the staged integration plan for the XTRA skeleton, reiterated the tidy→integrate→tidy loop, and flagged action items for the next agent.","topics":[{"topic":"Integration roadmap","what":"Authored docs/dpoi-qca-integration-plan.md detailing the STRICTNESS_GOD_TIER-compatible rollout (six phases).","why":"Provide a concrete path for merging typed ports, seeded VF2, journals, and epochs without tripping lint.","context":"Previous attempt stalled on clang-tidy; new drop introduces attachment pushouts + port caps.","issue":"No written plan previously existed.","resolution":"Documented phases 0–5 covering cleanup, structural imports, matcher swap, pushouts, QCA wiring, and final lint pass.","future_work":"Execute each phase sequentially, running clang-tidy between milestones.","time_percent":40},{"topic":"Task triage","what":"Noted the immediate loop: tidy clang → integrate new hotness → tidy clang.","why":"User emphasized this as the canonical workflow.","context":"Integration will span multiple PRs.","issue":"Need everyone following the same cadence.","resolution":"Captured the loop in docs and this debrief.","future_work":"Apply the loop per phase.","time_percent":30},{"topic":"Skeleton intake reminder","what":"Summarized XTRA contents (typed ports, attachment journaling+epochs, SIMD VF2, diff rollback, debug invariants).","why":"Next agent should read the drop before coding.","context":"rmg-c-rmg-skeleton-xtra.zip is the source of truth.","issue":"Risk of overlooking new capabilities.","resolution":"Mentioned highlights and pointed to the plan.","future_work":"Reference the drop during integration.","time_percent":30}],"key_decisions":["Integrate the XTRA skeleton via the documented phased plan.","Flank every integration step with STRICTNESS_GOD_TIER clang-tidy runs."],"action_items":[{"task":"Run Phase 0 of docs/dpoi-qca-integration-plan.md (restore lint-clean baseline)","owner":"Next agent"},{"task":"Proceed to Phase 1 once lint is green, repeating the tidy→integrate→tidy cycle","owner":"Next agent"}]}
```
{"date": "2025-10-15", "time": "08:06", "summary": "Restored the STRICTNESS_GOD_TIER lint baseline, drafted the staged DPOI/QCA integration plan, and opened the pull request.", "topics": [{"topic": "Lint baseline", "what": "Replaced heavy prototypes with lint-compliant stubs and ensured build/tests pass", "why": "Phase 0 requires a clean slate before integrating the XTRA skeleton", "context": "Existing matcher/QCA experiments violated STRICTNESS_GOD_TIER limits", "issue": "clang-tidy and analyzer were flagging hundreds of violations", "resolution": "Authored minimal placeholder implementations plus helper headers/sources to regain zero-warning state", "future_work": "Reintroduce full functionality during Phases 1-4 while keeping lint green", "time_percent": 35}, {"topic": "Integration plan", "what": "Captured a six-phase roadmap and generated GitHub issue drafts", "why": "Ensure future agents have a deterministic path for the XTRA drop", "context": "New drop adds typed ports, attachment journaling, and seeded VF2", "issue": "Work lacked a tracked, lint-aware rollout plan", "resolution": "Wrote docs/dpoi-qca-integration-plan.md and roadmap issue templates", "future_work": "File the issues on GitHub and execute Phase 1 next", "time_percent": 35}, {"topic": "Documentation & PR", "what": "Updated F013 spec listings and opened the feature branch PR", "why": "Align docs with new feature scope and surface the work for review", "context": "F.013 spec now lives alongside the plan; PR needed for review cycle", "issue": "Docs/features README was missing the F013 entry", "resolution": "Linked the spec, pushed the branch, and created PR #70", "future_work": "Collect review feedback and proceed with Phase 1 implementation", "time_percent": 30}], "key_decisions": ["Stage integration via the documented six-phase plan before touching production logic", "Use STRICTNESS_GOD_TIER as the single lint profile and hold the tidy->integrate->tidy loop"], "action_items": [{"task": "File the phase issues from docs/roadmap on GitHub and start Phase 1 struct imports", "owner": "Next agent"}]}
{"date":"2025-10-15","time":"20:15","summary":"Imported the Phase 1 structural scaffolding for DPOI/QCA while keeping the placeholder runtime intact and lint-ready.","topics":[{"topic":"Phase 1 structural imports","what":"Added port direction enums, interface signatures, attachment update scaffolding, and dual epochs to headers","why":"Phase 1 requires structural types in place before integrating matcher and commit behavior","context":"docs/dpoi-qca-integration-plan.md prescribes typed ports and epochs from the XTRA drop","issue":"Runtime headers lacked the data needed to express typed ports and attachment epochs","resolution":"Extended rmg/rule headers with the new structs and ensured constructors zero-initialize them for future phases","future_work":"Consume the new definitions in matcher and commit logic during Phases 2-4","time_percent":40},{"topic":"Test and build updates","what":"Updated rule helpers and unit tests to initialize and assert defaults for the new fields","why":"Need verification that the placeholder runtime stays consistent until behavior is wired in","context":"Existing tests only covered baseline matching and ticks","issue":"Without checks the new structs could regress unnoticed","resolution":"Initialized port caps to UINT16_MAX, asserted zeroed interface data, and left runtime logic untouched","future_work":"Add behavior-driven tests once matcher and commit pathways read these fields","time_percent":35},{"topic":"Verification constraints","what":"Rebuilt, ran ctest, and attempted clang-tidy under STRICTNESS_GOD_TIER","why":"Maintain the tidy→integrate→tidy workflow","context":"Phase 1 acceptance requires a lint pass","issue":"Local environment is missing the clang-tidy binary","resolution":"Captured the failure after confirming build and test success","future_work":"Re-run clang-tidy -p build once the tool is installed or available in CI","time_percent":25}],"key_decisions":["Retain UINT16_MAX defaults for new node port caps until matcher enforcement lands","Defer GitHub issue creation to the next agent while noting the requirement"],"action_items":[{"task":"Run clang-tidy -p build after installing clang-tidy to validate STRICTNESS_GOD_TIER compliance","owner":"Next agent"},{"task":"Create live GitHub issues for Phase 0/1 trackers when repository access permits","owner":"Next agent"}]}
