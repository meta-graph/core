# RMG Math: Confluence

The clean, formal stack—with exactly what we can prove for RMGs (under the DPOI semantics we’ve defined) and what needs extra hypotheses.

## 0) What we’re proving (scope)

**Tick‑level confluence (determinism)**: If we take any set of matches that a scheduler allows in the same tick (pairwise independent), the result is unique up to typed open‑graph isomorphism, regardless of the order you apply them.

**Two‑plane commutation**: Under the “no‑delete‑under‑descent” invariant, attachment updates and skeleton rewrites commute (attachments‑first then skeleton = skeleton then attachments), up to isomorphism. ✅

**Global confluence of the entire rewrite system**: true _only under standard conditions_ (e.g., termination + local confluence, or decreasing diagrams, or “orthogonal” rule sets with all DPOI critical pairs joinable). This is not automatic. ⚠️

Here, we define the objects precisely and prove the first two; then state the conditions for the third with pointers to the known lemmata we rely on (Critical Pair, Newman’s, Concurrency).

## 1) Preliminaries: categories and DPOI rewriting

### Typed open graphs

Fix a finite set of types T. Let $bf{Graph}_T$ be the adhesive category of T-typed (directed) graphs and type‑preserving morphisms. Let $bf{OGraph}_T$ be the category of typed open graphs: objects are cospans

$I \xrightarrow{i} G \xleftarrow{o} O$

with i,o monomorphisms (inputs/outputs), and morphisms are commuting maps of cospans. $bf{OGraph}_T$ is adhesive (pushouts along monos exist and are Van Kampen squares).

**Adhesive means**: pushouts along monos are stable under pullback and satisfy the Van Kampen property. This is exactly what makes DPO rewriting behave.

### DPOI rules and steps

A DPOI rule is a span of monos in $bf{OGraph}_T$:

$$p \;=\; (L \xleftarrow{\ell} K \xrightarrow{r} R)$$

where L (left), K (interface), R (right) are typed open graphs and $\ell,r$ are monos respecting interfaces.



A match of $p$ into a host open graph $G$ is a mono $m:L\to G$ that satisfies the gluing conditions:



**Dangling**: everything in $L\setminus K$ that would be deleted has no external incident edges in $G$ outside $m(L)$;

**Identification**: $m$ does not identify distinct items in $L\setminus K$.



Given a match $m$, one constructs a DPO step $G \Rightarrow_p H$ via the standard double square (pushout complement + pushout):

```math
\begin{array}{ccc}

K & \xrightarrow{\ell} & L \\

\;\;\downarrow k & & \downarrow m \\

D & \xrightarrow{} & G

\end{array}

\qquad

\begin{array}{ccc}

K & \xrightarrow{r} & R \\

\;\;\downarrow k & & \downarrow \\

D & \xrightarrow{} & H

\end{array}
```

All arrows are monos; the left square is a pushout complement; the right square a pushout in $\mathbf{OGraph}_T$.

Typed ports (your interfaces) are enforced by restricting $m$ to be boundary‑preserving and by typing constraints on $\ell,r$; if they fail, the pushout complement does not exist, and the match is rejected.


### 2) RMG states and “recursion” (attachments)

An RMG state is a triple

```math
\mathcal{U} \;=\; \big( G \,;\; \alpha,\beta \big)

where G\in \mathbf{OGraph}_T is the skeleton, \alpha:V(G)\to \mathbf{OGraph}_T assigns a (possibly empty) node‑attachment to each node, and \beta:E(G)\to \mathbf{OGraph}T assigns an edge‑attachment to each edge. Think of \alpha,\beta as objects in the product fibration \prod{x\in V(G)\cup E(G)}\mathbf{OGraph}_T.
```

A rewrite step on $cal{U}$ is a finite family of DPOI steps of two kinds:

**Attachment steps**: DPOI steps in fibers $\alpha(v)$ or $\beta(e)$, only at positions that are preserved by any simultaneous skeleton step (see below). These do not change $G$.

**Skeleton steps**: a set of DPOI steps on $G$ w.r.t. rules $p_i=(L_i \leftarrow K_i \to R_i)$ and matches $m_i:L_i \to G$.

We require pairwise independence of the skeleton matches (defined next) and the **no‑delete‑under‑descent invariant**: if an attachment step touches $\alpha(v)$ or $\beta(e)$, no concurrent skeleton step deletes $v$ or $e$.

Publishing a step is two‑plane:

1. apply all attachment steps;
2. apply all skeleton steps.

This is exactly your “journal + epoch” split.

### 3) Independence and the scheduler

Two matches $m_1:L_1\to G$, $m_2:L_2\to G$ are (parallel) independent if

$m_1(L_1\setminus K_1)$ is disjoint from $m_2(L_2)$, and symmetrically; and

the pushout complements for each exist (gluing holds).

**Intuitively**: neither step deletes or identifies anything the other needs. Your MIS + halo scheduler enforces (1) by construction (disjoint touched sets) and enforces (2) via the gluing tests (dangling/typed‑ports).

## 4) Confluence: formal definitions

Given a binary relation $\to$ on states, write $\to^\ast$ for its reflexive–transitive closure. The system is

confluent (Church–Rosser) iff $\forall X,Y_1,Y_2 with X\to^\ast Y_1$ and $X\to^\ast Y_2, \exists Z with Y_1\to^\ast Z and Y_2\to^\ast Z$ (here up to isomorphism of typed open graphs and attachment indices);

locally confluent iff the same holds for single steps $X\to Y_1, X\to Y_2$.

**Newman’s Lemma**: if $\to$ is terminating (no infinite chains) and locally confluent, then it is confluent.

For adhesive DPO rewriting there’s also the **Critical Pair Lemma** and the **Concurrency/Parallelism Theorem**; we use both.

## 5) Theorems we can claim now

### **Theorem A (Tick‑level confluence for the scheduler)**

Let $\mathcal{U}$ be an RMG state and $\{(p_i,m_i)\}_{i\in I}$ a **scheduler‑admissible family** of skeleton matches (pairwise independent) and attachment matches that satisfy no‑delete‑under‑descent. Let $\sigma$ range over permutations of $I$.

Then, applying the family in any serial order consistent with the two‑plane split (attachments first, then skeleton),

```math
\mathcal{U} \;\Rightarrow\; \cdots \;\Rightarrow\; \mathcal{U}’
```

yields a unique result $\mathcal{U}’$ up to typed open‑graph isomorphism—independent of $\sigma$.

#### Proof sketch

**(i) Skeleton**: In adhesive categories, the Parallel Independence/Concurrency Theorem for DPO rewriting states that independent DPO steps commute (the characteristic double squares paste, yielding isomorphic results regardless of order). Induct on $|I|: base |I|=2$ is the theorem; the inductive step composes two commuting squares at a time.

**(ii) Attachments**: By no‑delete‑under‑descent, all touched attachment positions (indices in $\alpha,\beta$) are preserved by skeleton steps. Since attachment DPO steps occur in the product of fibers $\prod_x\mathbf{OGraph}_T$, they are trivially independent of each other and of the base when considered before the skeleton edit. Therefore the overall two‑plane composite is order‑independent. ■

This is the precise mathematical justification for _“a tick’s batch is deterministic regardless of order.”_

### Theorem B (Two‑plane commutation)

Let $A$ be any finite family of attachment steps on positions $P$ and $S$ a family of skeleton steps such that $S$ preserves all positions in $P$ (no‑delete‑under‑descent). Then:

```math
\text{(apply \(A\) then \(S\))} \;\cong\; \text{(apply \(S\) then the transported \(A\))}
```

i.e., the square made by the product fibration of attachments over the skeleton commutes up to isomorphism.

#### Proof sketch.


Consider the projection functor $\pi:\mathsf{RMG}\to \mathbf{OGraph}_T$ (forget attachments). An attachment update is a DPO step in the fiber over a fixed base object; a skeleton update is a DPO step in the base. Because the base update is mono‑created (DPO along monos) and positions in $P$ are preserved, the reindexing functor on fibers (transport of attachments along the base mono) preserves pushouts along monos. This is exactly the Van Kampen property lifted to the fibration, hence the two sequences yield isomorphic totals. ■

**Operationally**: the “flip attachments epoch, then skeleton epoch” is not just convenient—it is correct.

### Theorem C (Local confluence via critical pairs — conditional)

Let $R$ be a finite rule set (DPOI over $\mathbf{OGraph}_T$). Suppose every DPOI critical pair of $R$ is strongly joinable (there are derivations from both peaks to a common reduct), modulo typed‑boundary isomorphism. Then the rewrite relation $\Rightarrow_R$ is locally confluent.


If, in addition, $\Rightarrow_R$ is terminating on the class of states considered (or satisfies a decreasing diagrams labelling), then $\Rightarrow_R$ is confluent (Church–Rosser).


#### Proof idea.

Standard Critical Pair Lemma for DPO in adhesive categories gives local confluence from joinability of all critical pairs. Combine with Newman’s Lemma (or van Oostrom’s Decreasing Diagrams in the non‑terminating case) to lift to global confluence. Ports/interfaces restrict matches, but do not invalidate the lemma—they just remove some potential overlaps. ■

**Translation**: if you curate the rule algebra (or prove decreasingness), you get global confluence. Otherwise, don’t claim it.

## 6) What the engine enforces to make A & B true

**Independence checks**: matches are pruned so that images of L\setminus K are disjoint; gluing/dangling/typed‑ports enforced → DPO complements exist.

**No‑delete‑under‑descent**: a descend flag on $v$ or $e$ requires $v,e\in K$ (preserved) in any concurrent skeleton step; the scheduler rejects combinations that violate this.

**Two‑plane publish**: attachments (fibers) updated first; then skeleton (base). This realizes Theorem B’s hypothesis.

Those are exactly the preconditions the formal theorems need.

## 7) What we should not overclaim

General global confluence for arbitrary RMG rule packs **is false in general** (graph rewriting can diverge). You either:

restrict to orthogonal rule sets (no deletes/no overlaps across left‑sides), or
prove all DPOI critical pairs joinable, or
impose a well‑founded measure (termination) or a decreasing diagram labelling.

**The engine can help**: we can ship a critical‑pair analyzer and a decreasing labelling checker for your rule packs.

## 8) One‑page “formal” summary we can drop in the spec

Work in $\mathbf{OGraph}_T$, an adhesive category of typed open graphs.

Rules $p=(L\leftarrow K\to R)$ are linear (monos), matches $m:L\to G$ satisfy gluing.

Tick semantics = apply a family of pairwise independent DPOI steps on $G$ and DPOI steps in attachment fibers at preserved positions; publish as attachments then skeleton.

Theorem A (Concurrency) ⇒ tick is deterministic up to iso.

Theorem B ⇒ two‑plane order commutes up to iso.

Theorem C (conditional) ⇒ global confluence holds if you ensure joinable critical pairs (or termination / decreasing diagrams).

## 9) If we want the proofs “in code”

Add a unit test that shuffles the order of a scheduled batch and asserts isomorphism of results (tick‑level confluence).

Add an A/B harness that executes “attachments then skeleton” vs “skeleton then transported attachments” and checks isomorphism (two‑plane commutation) under the no‑delete‑under‑descent invariant.

Add a critical‑pair generator for your rule packs with a joinability checker (small SAT/SMT or bounded search) for local confluence evidence.

## Bottom line

We can formally guarantee the determinism you’ve claimed: within a tick and across the two planes.

We can get global Church–Rosser when you give the usual rewrite‑theory hypotheses (orthogonality / CP‑joinability / termination or decreasing diagrams).

The math needed is standard DPO in adhesive categories + product‑fiber reasoning; your “typed ports,” “no‑delete‑under‑descent,” and “attachments‑first” are exactly the guardrails that make the theorems go through.

---

## 1) Tiny clarifications (to make the proofs airtight)

### a) Independence = delete∩use disjointness (both ways)

For two skeleton matches $m_i:L_i\to G$ of rules $p_i=(L_i\leftarrow K_i\to R_i)$,
define
• **delete set**: $\mathrm{Del}(m_i)=m_i(L_i\setminus K_i)$ (the bits that will be removed),
- **use set**: $\mathrm{Use}(m_i)=m_i(L_i)$ (the whole context the rule relies on).

Parallel independence is exactly:

```math
\mathrm{Del}(m_1)\cap \mathrm{Use}(m_2)=\varnothing
\quad\text{and}\quad
\mathrm{Del}(m_2)\cap \mathrm{Use}(m_1)=\varnothing,
```

plus gluing (dangling/identification) holds for both.

### b) Scheduler’s “touch” set is a safe over‑approximation

For scheduling we use

```math
\mathrm{Touch}(m)=\mathrm{Use}(m)\ \cup\ \mathrm{Halo}_r(\mathrm{Use}(m)),
where the halo is the r-hop neighborhood (kernel radius).
```

MIS on these stronger sets implies the formal independence above. That’s why Theorem A applies.

### c) Two‑plane commutation needs one more explicit clause

We already stated no‑delete‑under‑descent. Add also no‑clone‑under‑descent for strictness:

- If a skeleton step duplicates a preserved node/edge (via $K\to R$ mapping with multiple targets), define how attachments are transported (copy vs share). In our current spec we don’t clone preserved positions—$K\to R$ is injective on preserved items—so attachments just reindex. That makes Theorem B immediate.

---

## 2) Math ⇄ code contract (drop‑in to the spec)

### Sets we actually compute

For a compiled rule $p$ and $a$ host match $m$:

- `del_nodes = image(L.nodes \ K.nodes)`
- `del_edges = image(L.edges \ K.edges)`
- `use_nodes = image(L.nodes)`
- `use_edges = image(L.edges)`
- `touch_nodes = use_nodes ∪ Halo_r(use_nodes) (same for edges if needed`

### Independence check (symmetric)

```math
Independent(m1,m2) iff
  (del_nodes1 ∩ use_nodes2 = ∅) and (del_edges1 ∩ use_edges2 = ∅) and
  (del_nodes2 ∩ use_nodes1 = ∅) and (del_edges2 ∩ use_edges1 = ∅) and
  gluing(m1) and gluing(m2)  // dangling + ports OK
```

### **Scheduler rule**

- Build all matches.
- Filter by gluing (dangling + typed port constraints).
- Order by stable key; greedily take a maximal independent set using `touch_*` (over‑approx); drop the rest.

This enforces the hypotheses of the DPO Concurrency Theorem, giving you tick‑level confluence (Theorem A).

### Two‑plane discipline

- If any $v$ or $e$ has descend flagged in the attachment batch, forbid any skeleton match that deletes or clones that $v$ or $e$ in the same tick.
- Apply attachments first (within fibers), then skeleton pushouts.
- Result is iso to the other order (Theorem B).

---

## 3) Preconditions checklist (paste this into “Correctness Assumptions”)

1. Category: typed open graphs $\mathbf{OGraph}_T$ form an adhesive category; rules are linear (span of monos).
2. Ports: matches are boundary‑preserving and satisfy typed‑interface constraints; if not, no match (pushout complement doesn’t exist).
3. Independence: scheduler only co‑applies matches $m_i$ satisfying delete∩use disjointness both directions.
4. Descent invariants: no‑delete‑under‑descent (and no‑clone‑under‑descent if you keep “no clone” as a design rule).
5. Two‑plane: publish attachments → skeleton.

Under 1–5, Theorem A (tick determinism) and Theorem B (plane commutation) hold.

Global confluence requires, in addition:
   1. Either (i) termination + local confluence (all critical pairs joinable), or (ii) a decreasing‑diagrams labelling for overlaps.

## 4) Critical‑pair analysis (how we’ll make C claimable)

Goal: For a given rule pack R, compute all DPOI overlaps L_1 \hookleftarrow O \hookrightarrow L_2 (respecting ports), generate the two peaks and check joinability (bounded or with a strategy). If all join, and we have a termination witness, we can assert global confluence for that pack.

Plan:
•	Overlap generator: enumerate monos o_i:O\to L_i with boundary typing; forbid trivial embeddings and those violating ports.
•	Peak constructor: pushout to build the two one‑step divergences.
•	Join search: bounded breadth‑first search with your scheduler (or SMT‑guided) to find a common reduct modulo typed isomorphism.
•	Report: list unjoinable CPs or produce a confluence certificate (rule‑pack versioned).

This plugs straight into CI; small rule packs (ETL surgery, routing) are typically orthogonal or trivially joinable.

## 5) Tests you should add (they reflect the proofs)

1. **Tick determinism**: shuffle a scheduled batch $N$ times; assert host graph iso each time.
2. **Two‑plane commutation**: $A→S$ vs $S→(reindexed A)$ yields iso hosts; assert epochs flipped appropriately.
3. **Independence necessary & sufficient**: construct pairs
   (a) independent,
   (b) dependent via delete↔use;
verify (a) both orders commute; (b) one order is blocked or gives non‑iso unless scheduled separately.
4. **CP sanity**: include a deliberately non‑joinable pair; expect analyzer to flag it.

## 6) Where to be careful (edge cases you can call out)

- **Ports in overlaps**: Critical pairs must respect typed interfaces; this often removes would‑be overlaps—good news for local confluence.
- **Created boundary**: If a skeleton step creates a new boundary leg, it’s still fine (pushout along mono), but attachments for brand‑new nodes/edges must be defaulted (e.g., empty).
- **Cloning preserved items**: If you ever allow $K\to R$ to be many‑to‑one on preserved positions (true duplication), state an explicit transport policy for attachments (copy vs share) and re‑prove Theorem B for that policy.
