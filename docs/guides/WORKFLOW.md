# MetaGraph Development Workflow

This guide captures the canonical **Cycle of Work** that agents must follow.
It was previously embedded in `AGENTS.md` and is now tracked here so pull
requests can reference it directly.

> [!INFO]
> ## The Cycle of Work
> > By ChatGPT PRIME™ • 2025-10-15 @ 01:53
>
> ## **Preface**: Yo, What Even ***Is*** Code?
>
> Greetings, my dudes. You ever think about code, and like, dude... it's all
> **energy**, man. Zeroes and ones, you feel me? And, like, *tests*? Tests are
> like… the **tuning forks** of that energy. Like, I knowrite? *WOAH*! Yeah. So,
> like, when you sit down to code, don’t grind. Don’t force it, broh. **Just
> catch the wave.**
>
> **The Cycle of Work** is how we vibe through the repo without wiping out. It’s
> a sacred ritual, passed down from linters past and CI/CD pipelines old as
> time.
>
> Let’s drop in.

### The Ten Tubular Waves of Work

1. **Wipe the Wax Off Your Git Deck** – `git status`
2. **Return to the Lineup (origin/main)** – stay synced before branching.
3. **Drop a Fresh Branch** – `git switch -c feat/<what-you’re-rippin>`.
4. **Test First, Bro** – write the failing test before adding logic.
5. **Let It Fail, and Love That Red** – failure proves the test is real.
6. **Sketch the Surf Shack** – shape the public API without logic.
7. **Fill It With Stoke (The Logic Phase)** – implement just enough to go
   green.
8. **Refactor the Barrel** – clean up once tests stabilise.
9. **Speak the Truth in Docs** – update docs, diagrams, READMEs.
10. **Push and Let It Fly** – push, open the PR, invite feedback, repeat.

### Virtues & Wipeouts Cheat Sheet

| Virtue                | Meaning                                         |
|-----------------------|-------------------------------------------------|
| **SRP**               | Each module surfs solo                          |
| **KISS**              | Keep it Simple, Shredder                        |
| **YAGNI**             | Don’t build what you don’t need                 |
| **DRY**               | Don’t repeat yourself                           |
| **Test Double Vibes**| Mock external chaos, never your own logic       |

Avoid: spying on internals, mocking your own code, or testing
implementation instead of behaviour.

### Fail Loud, Fix Fast

- Red bars mean alignment forming—embrace them.
- Stay in the tidy → integrate → tidy loop: every change starts and ends with
  `clang-tidy -p build`.
