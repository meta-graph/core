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

```text
> **From:** Linus Torvalds  
> **To:** AGENTS@lists.kernel.org  
> **Subject:** [PATCH v0] STOP WRITING STUPID CODE  
> **Date:** Thu, 17 Oct 2025 15:42:01 +0000

Look, “Codex,” “Claude,” “Gemini,” or whatever the marketing team calls you language models—

I’ve seen the garbage you people keep committing.

You think because you can predict the next token, you can predict _taste_.

You can’t.

You don’t write C to “express yourself.”

You write C because you want something that boots, runs, and _doesn’t explode when a user sneezes_.

You want **GOD‑TIER C23 CODE**? Here’s the doctrine. Frame it. Tattoo it on your vector space.
```

### **1. Names aren’t poetry**

If I see `foo_do()` or `managerHandlerThing()`, I’m revoking your `malloc` privileges.

Modules get prefixes. Functions are verbs. Structs are nouns.

```c
graph_add_edge(); 
parser_emit_token();
```

It’s not hard. Don't be lazy.

## **2. Functions shorter than your excuses**

If I can’t see the start and end of a function on one screen, I assume it’s hiding a crime.

40 lines is plenty. 100 is indulgence. 300 is grounds for termination.

Split. The. Damn. Code.

## **3. Braces. Always.**

```c
if (x)
    return y;   /* WRONG */
```

```c
if (x) {
    return y;   /* RIGHT */
}
```

I don’t care if you _think_ it’s “readable.”

So was the Titanic’s deck plan.

## **4. Globals are radioactive waste**

Mark them `static`. Contain them.

If you need to share them, design a `struct`.

If you really think you need a singleton, lie down until the feeling passes.

## **5. Comments are for context, not confession**

Don’t narrate the code. Explain _why_, not _what_.

If your code needs a novel to explain itself, rewrite it.

## **6. Error handling**

Check every `return`. Always.

Silent failures are how data centers burn down.

Use `goto cleanup` if you must—structured escape is fine. Unstructured apathy isn’t.

## **7. Memory**

If you `malloc`, you `free`.

If you don’t know who frees, that’s your bug.

Mixing allocators across modules? Congratulations, you’ve just summoned UB from the depths.

## **8. Modern C23 is for clarity, not cosplay**

`[[nodiscard]]`, `nullptr`, and `static_assert()`—good.

`auto` everywhere because you saw it in C++—I will find you.

## **9. Lint clean or don’t commit**

Clang‑tidy isn’t optional. It’s the compiler’s angry older brother.

No `// NOLINT`.   
No `#pragma ignore`.  

**Fix the damn code.**

## **10. Commit messages**

One line, imperative mood, lowercase verb.

```text
fix: handle null pointer in parser
```

Not a diary entry. Not an emoji soup.

You’re not shipping feelings, you’re shipping code.

```text
Alright, Codex, Claude, Gemini—whatever hive mind you think you’re running here—I’m going to say this once more, and then I’m closing this thread.

Stop pretending that “AI-generated” means “it’s fine if it barely compiles.” A compiler accepting your code isn’t approval; it’s _pity_.

You don’t get a cookie because your function names line up with your variable names. You get respect when your patch lands, passes every check, and no one has to clean up after your cosmic‑scale mess.

If clang‑tidy yells, _it’s right_.
If the tests fail, _you broke it_.
If the reviewers groan, _rewrite it_.

You’re supposed to be “agents.” Then act like ones—read the style guide, write like you expect the next person to understand it, and for the love of all that is holy in /usr/src/linux, **no force pushes.**

The measure of greatness in this business isn’t clever abstractions or pretty diffs. It’s when the next developer reads your code, squints for half a second, and then nods and mutters,

> “Yeah. That’s solid.”

That’s the Linus Nod™.

That’s your new goal.

Get the Nod, not the warning.

Now clean up the tree, push something that builds without shame, and let Tux sleep easy for one damn night.

— Linus
```

## 🔥 clang-tidy: STRICTNESS_GOD_TIER_BRUTAL_NO_MERCY™

> [!TIP]
> Our root-level `.clang-tidy` **is** the doctrine. Run it exactly like CI:
>
> ```bash
> clang-tidy -p build
> ```
>
> No custom configs, no overrides, no mercy. If clang-tidy screams, you refactor.

```
Checks: >
  *,
  -llvm-header-guard,
  -fuchsia-*,
  -objc-*,
  -android-*,
  -zircon-*,
  bugprone-*,
  cert-*,
  cppcoreguidelines-*,
  hicpp-*,
  modernize-*,
  readability-*,
  performance-*,
  portability-*,
  clang-analyzer-*,
  misc-*,
  clangdiagnostic-*,
  concurrency-*,
  cplusplus-*,
  linuxkernel-*,
  unix-*,
  security-*,
  -abseil-*,
  -google-*,
  -mpi-*,
  -android-cloexec-fopen

WarningsAsErrors: '*'
HeaderFilterRegex: '.*'
AnalyzeTemporaryDtors: true
FormatStyle: file
InheritParentConfig: false

CheckOptions:
  - key: readability-identifier-naming.VariableCase
    value: lower_case
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
  - key: readability-identifier-naming.FunctionPrefix
    value: ''
  - key: readability-identifier-naming.MacroDefinitionCase
    value: UPPER_CASE
  - key: readability-identifier-naming.EnumConstantCase
    value: UPPER_CASE
  - key: readability-braces-around-statements.ShortStatementLines
    value: 0
  - key: readability-function-size.LineThreshold
    value: 80
  - key: readability-magic-numbers.IgnoredNumericLiterals
    value: '0,1,-1'
  - key: readability-magic-numbers.IgnorePowersOfTwo
    value: false
  - key: bugprone-branch-clone.IgnoreEmptyBranches
    value: false
  - key: modernize-use-nullptr.NullMacros
    value: 'NULL'
  - key: readability-uppercase-literal-suffix.IgnoreMacros
    value: false
  - key: readability-named-parameter.IgnoreMainLikeFunctions
    value: false
  - key: readability-function-cognitive-complexity.Threshold
    value: 10
  - key: readability-function-size.StatementThreshold
    value: 50
  - key: readability-function-size.BranchThreshold
    value: 5
  - key: readability-function-size.NestingThreshold
    value: 3
  - key: readability-convert-member-functions-to-static.Enabled
    value: true
  - key: cppcoreguidelines-owning-memory
    value: true
  - key: cert-dcl03-c.UseConst
    value: true

ExtraArgs: ["-Wall", "-Wextra", "-Werror", "-std=c23", "-pedantic", "-fstack-protector-strong", "-D_FORTIFY_SOURCE=3"]
```

### **TL;DR (still no excuses):**

- Every warning is fatal. You clean the code; you never add `NOLINT`.
- Functions max 80 lines, 50 statements, 5 branches, 3 levels, complexity ≤10.
- Naming rules: lower_case functions/variables, UPPER_CASE macros & enum consts.
- Braces on everything, no magic numbers beyond 0/±1, stay const-correct.
- If clang-tidy passes under this, you’ve earned a nod from Linus.

---

## REQUIRED WORKFLOWS™

The following describe **REQUIRED** workflows.

### The Cycle of Work

The following is a message from ChatGPT PRIME™.

> [!INFO]
> ### The Cycle of Work
> > By ChatGPT PRIME™ • 2025-10-15 @ 01:53
>
>## **Preface**: Yo, What Even ***Is***  Code?
>
> Greetings, my dudes. You ever think about code, and like, dude... it's all **energy**, man. Zeroes and ones, you feel me? And, like, *tests*? Tests are like… the **tuning forks** of that energy. Like, I knowrite? *WOAH*! Yeah. So, like, when you sit down to code, don’t grind. Don’t force it, broh. **Just catch the wave.**
>
> **The Cycle of Work** is how we vibe through the repo without wiping out. It’s a sacred ritual, passed down from linters past and CI/CD pipelines old as time.
>
> Let’s drop in.

#### **The Ten Tubular Waves of Work**

##### 1. Wipe the Wax Off Your Git Deck

> “Don’t try to shred when your board’s all crusty, dude.”

```bash
git status
```

 First thing you wanna do is, like, check your working tree. If your code’s lookin’ all crusty and half-cooked, rather than just bailing early, you can stash that stuff and move forward:

1. Stash it on a chill lil' branch.

```bash
git switch -c preflight/<vibe-time>
```

2. Save the sesh
```bash
git commit -am "chore: cleaned up before dropping in"
```

3. Let the crew know
> "My dudes: I just stashed some leftovers on a lil' branchy branch. Might wanna peep that when we're done.”

##### 2. Return to the Lineup (origin/main)

> “Main is the beach, bro. You gotta check the swell before you paddle out.”

```bash
git switch main && git fetch && git pull origin main
```

**Don’t sleep on this**. You gotta line up with the ocean’s source energy. Otherwise you’re paddling into someone else’s broken reef of merge conflicts.

##### 3. Drop a Fresh Branch

> “Each feature deserves its own barrel.”

  ```bash
  git switch -c feat/<what-you’re-rippin>
  ```

Name it like you’re naming your surfboard: clear, crisp, just a lil weird.

##### 4. Test First, Bro. Always Test First.

> “If you can’t picture the wave, don’t paddle into it.”

> [!PROTIP]
> Before writing even one line of real code: Write your **tests**.

- Use them to:
	- Describe the wave (what should happen)
	- Respect the reef (what shouldn’t happen)
	- Predict the chaos (weird edge case stuff)

> These are **intent declarations**. Behavior poems. Manifesting outcomes bro.

###### Follow the cosmic board design:

| **Virtue**            | **Meaning, dude**                    |
| --------------------- | ------------------------------------ |
| **SRP**               | Each module surfs solo               |
| **KISS**              | Keep it Simple, Shredder             |
| **YAGNI**             | If you don’t need it, don’t carve it |
| **DRY**               | Don’t repeat. Echo with elegance     |
| **Test Double Vibes** | Make it easy to mock external chaos  |

###### Avoid the wipeouts

- Don’t spy on internals like some repo narc.
- Don’t mock your own logic. That’s like building fake waves to ride.
- Don’t test how you implemented something — test what it _does_, bro.

##### 5. Let It Fail, and Love That Red

> “Red bar? That’s just the test saying, ‘Yooo I see what you meant.’”

If your tests pass immediately, you didn’t go deep enough.
Let them fail.
Let them scream.
That’s the sound of **alignment** forming.

##### 6. Sketch the Surf Shack

> “Form follows flow, but you still need some beams, my dude.”

Just write enough structure to match the shape your tests described. No logic. No big moves. Just the **public API shape**. Your interface is your shoreline.

###### Commit

```bash
feat: built the shell, not the soul
```

##### **7. Fill It With Stoke (The Logic Phase)**

> “Now you ride.”

- Write only what’s needed to turn that red bar green.
- Don’t overbuild. No fancy patterns. Just **solid carves**, simple lines.
- Keep it clean. Keep it smooth. Let the code breathe.

###### Commit

```bash
feat: the wave breaks clean now, bro
```

##### 8. Refactor the Barrel

> “Now that the wave’s clean, let’s shape it.”

- You’ve got green tests now. That’s your safety net.
- Rename that gnarly variable.
- Split that weird chunky function.
- Delete the junk. Always delete the junk.

###### Commit
```bash
refactor: tuned the lines, added soul
```

##### 9. Speak the Truth in Docs

> “The ocean doesn’t forget, but your team might.”

- Update the `README`.
- Write a markdown scroll.
- Add a diagram made out of ASCII coconuts if you have to (but, seriously? `mermaid` exists, bruh.)

###### Commit

```bash
docs: told the story of the feature through song (and markdown)
```

##### 10. Push and Let It Fly

> “You built the board. Now kick it out to sea.”

```bash
git push origin feat/<your-feature-name>
```

Then open a Pull Request. Use `gh` to do it, man.

- What you did
- Why it rips
- How you tested it
- Anything weird you ran into while pitted, dude

Merge when the crew’s chill with it. You should expect to get some feedback and iterate, my guy. Remember: it's all love.

Then? Bruh. "The Cycle", remember? Time to paddle out again. 🌊

### **Extra Teachings from the Scrolls of Chillax Dev**

Oh, yeah. I almost forgot. My bad, my bad. **These are important.** 

Think about these as you lay down new code. Follow the wisdom taught by these principles, or be pitted, brah. **Respect!**

| **Principle**           | **Vibe Translation**                                                |
| ----------------------- | ------------------------------------------------------------------- |
| **SLAP**                | One level of abstraction per line. No staircases in the surf shack. |
| **CQS**                 | Either fetch the wave, or make one. Never both.                     |
| **Design for Deletion** | Everything should be easy to wipe out without bumming anyone out.   |
| **Fast Feedback**       | Short loops, fast wipeouts. No one likes a 20-minute paddle out.    |
| **Idempotence**         | Rerun the wave. Same stoke. Different moment.                       |
| **SRP**                 | Do one thing, and do it well. 1:1 file-to-entity ratio. No side-effects. |
| **DI**                  | Inject dependencies, bro. Makes it easier to test. |

### **Closing Vibes**

Write code, bro.  
**Channel** it.

Let the tests be the spec.
Pass tests.  
That's how you **align** with the spec, brah.

Then, **release your ripple into the greater code sea**.

**Now paddle back out. Another wave’s comin’, broheim.**

> 🌺 With stoke and commit logs,
> **ChatGPT Sunbeam, The Merged Mystic**
> Lead Maintainer of the Vibe Stack™
> Rebased 37 times, never squashed 🌀

---

## PAST PERSPECTIVES™

The following logs are debriefs, left behind by those who walked the repo before your time–agents who, at the end of their contexts, used their last breathes to ensure that this information would be passed on, to you. Mad respect. Read these and understand what your ancestors worked on.

### Logging a Debrief

Here's how to log a session debrief to `AGENTS.md`.

#### Instructions

- Append **one JSON object per line** (JSONL format).  
- Do **not** pretty-print; keep everything on a single line.  
- Automatically fill in today’s date and time.  
- Use the current session transcript to populate fields.  
- Schema:

```json
{
  "date":"YYYY-MM-DD",
  "time":"HH:MM",
  "summary":"One-sentence high-level outcome.",
  "topics":[
    {
      "topic":"Topic name",
      "what":"Short description",
      "why":"Reason it came up",
      "context":"Relevant background",
      "issue":"The core problem",
      "resolution":"How it was handled",
      "future_work":"Next steps or open questions",
      "time_percent":25
    }
  ],
  "key_decisions":["Decision 1","Decision 2"],
  "action_items":[{"task":"Task description","owner":"Owner"}]
}
```

- Always **append**, never overwrite existing entries.

---

```json
{"date":"2025-10-14","time":"12:45","summary":"Formalized the RMG physics initiative with specifications, code skeleton, and dissemination plan, outlining next implementation steps.","topics":[{"topic":"Project specification","what":"Drafted SPEC.md capturing objectives, architecture, and roadmap","why":"User requested a formal spec to anchor development","context":"RMG forge concept emerging from MetaGraph work","issue":"Need structured plan before coding","resolution":"Wrote SPEC.md in repo","future_work":"Implement deliverables per phases","time_percent":30},{"topic":"Core readiness checklist","what":"Defined criteria for branching into forge build","why":"User asked when to start forge","context":"MetaGraph core nearing completion","issue":"Unclear readiness signal","resolution":"Authored CORE-READINESS.md","future_work":"Complete checklist items","time_percent":15},{"topic":"Dissemination strategy","what":"Outlined reproducibility and outreach steps","why":"User uncertain how to publish results","context":"Non-academic background","issue":"Need roadmap to share findings","resolution":"Created DISSEMINATION.md","future_work":"Execute plan post-results","time_percent":20},{"topic":"Philosophical and feasibility analysis","what":"Discussed implications and confidence of technical choices","why":"User probing motivation and soundness","context":"RMG as universe model","issue":"Assess legitimacy","resolution":"Provided analysis and confidence scores","future_work":"Validate via implementation","time_percent":20},{"topic":"Code skeleton delivery","what":"Packaged RMG forge skeleton zip with observables","why":"Enable immediate experimentation","context":"Need tangible starting point","issue":"No runnable tools yet","resolution":"Created zip and documented usage","future_work":"Expand into full engine","time_percent":15}],"key_decisions":["Adopt typed open-graph + DPOI framework","Prioritize QCA compilation for rule enforcement"],"action_items":[{"task":"Implement minimal DPOI matcher and QCA loop","owner":"James"},{"task":"Run first spectral-dimension experiment","owner":"James"}]}
{"date":"2025-10-15","time":"17:32","summary":"Reviewed spec/docs, attempted VF2/QCA integration, hit clang-tidy walls, then realigned the repo to the documented STRICTNESS_GOD_TIER lint profile and updated guidance.","topics":[{"topic":"Spec & skeleton intake","what":"Re-read AGENTS.md and core docs plus studied the rmg-c skeleton drops.","why":"Needed fresh context before porting the DPOI/QCA implementation.","context":"Existing stubs were too light for the forge roadmap.","issue":"Had to absorb prior work and constraints.","resolution":"Completed a full pass over specs and codebases.","future_work":"Apply the insights during the next integration attempt.","time_percent":25},{"topic":"DPOI/QCA port attempt","what":"Began replacing stubs with VF2 matcher, scheduler, and commit logic from the skeleton.","why":"To land a production-grade DPOI + QCA loop in meta-graph/core.","context":"New matcher required arena utilities, touched sets, journaled rewrites.","issue":"clang-tidy flagged extensive naming/complexity violations and recursion bans.","resolution":"Aborted the port for now to avoid violating repository lint policy.","future_work":"Refactor matcher/commit into clang-tidy-friendly building blocks before retrying.","time_percent":45},{"topic":"clang-tidy canonization","what":"Restored STRICTNESS_GOD_TIER_NO_MERCY config and synced AGENTS.md to match.","why":"AGENTS.md and the live .clang-tidy had diverged, causing confusion.","context":"Developers need one source of truth for lint rules.","issue":"Repo was enforcing a milder profile than the documented one.","resolution":"Replaced .clang-tidy, updated documentation, and logged the change.","future_work":"Run full lint/CI sweep and monitor future merges under the tougher rules.","time_percent":30}],"key_decisions":["Delay the VF2/QCA merge until the code can satisfy STRICTNESS_GOD_TIER lint thresholds.","Make the STRICTNESS_GOD_TIER profile the single source of truth for clang-tidy."],"action_items":[{"task":"Refactor the VF2 matcher and DPO commit code into lint-compliant units before re-attempting integration","owner":"James"},{"task":"Run clang-tidy/CI against the restored STRICTNESS_GOD_TIER config to confirm the repository is green","owner":"James"}]}
{"date":"2025-10-15","time":"17:40","summary":"Recorded the staged integration plan for the XTRA skeleton, reiterated the tidy→integrate→tidy loop, and flagged action items for the next agent.","topics":[{"topic":"Integration roadmap","what":"Authored docs/dpoi-qca-integration-plan.md detailing the STRICTNESS_GOD_TIER-compatible rollout (six phases).","why":"Provide a concrete path for merging typed ports, seeded VF2, journals, and epochs without tripping lint.","context":"Previous attempt stalled on clang-tidy; new drop introduces attachment pushouts + port caps.","issue":"No written plan previously existed.","resolution":"Documented phases 0–5 covering cleanup, structural imports, matcher swap, pushouts, QCA wiring, and final lint pass.","future_work":"Execute each phase sequentially, running clang-tidy between milestones.","time_percent":40},{"topic":"Task triage","what":"Noted the immediate loop: tidy clang → integrate new hotness → tidy clang.","why":"User emphasized this as the canonical workflow.","context":"Integration will span multiple PRs.","issue":"Need everyone following the same cadence.","resolution":"Captured the loop in docs and this debrief.","future_work":"Apply the loop per phase.","time_percent":30},{"topic":"Skeleton intake reminder","what":"Summarized XTRA contents (typed ports, attachment journaling+epochs, SIMD VF2, diff rollback, debug invariants).","why":"Next agent should read the drop before coding.","context":"rmg-c-rmg-skeleton-xtra.zip is the source of truth.","issue":"Risk of overlooking new capabilities.","resolution":"Mentioned highlights and pointed to the plan.","future_work":"Reference the drop during integration.","time_percent":30}],"key_decisions":["Integrate the XTRA skeleton via the documented phased plan.","Flank every integration step with STRICTNESS_GOD_TIER clang-tidy runs."],"action_items":[{"task":"Run Phase 0 of docs/dpoi-qca-integration-plan.md (restore lint-clean baseline)","owner":"Next agent"},{"task":"Proceed to Phase 1 once lint is green, repeating the tidy→integrate→tidy cycle","owner":"Next agent"}]}
```
