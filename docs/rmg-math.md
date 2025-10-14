# Recursive Meta-Graphs: Formal Model and Correspondence

> This note packages the mathematical backbone of the MetaGraph project.  It makes the “graphs all the way down” slogan precise, shows how standard algebraic machinery applies, and cross-checks the theory against the engineering design captured in `docs/architecture.md`.

---

## 1. Inductive Set-Theoretic Definition

Fix a universe of payloads $P$ (raw blobs, metadata, etc.).  Define the class $\mathrm{RMG}$ of **recursive meta-graphs** to be the **least** class closed under the following constructors:

1. **Atoms.** For any $p \in P$, the object $\operatorname{Atom}(p)$ belongs to $\mathrm{RMG}$.
2. **Graphs.** Let $S = (V, E, s, t)$ be a finite directed multigraph (“1-skeleton”), and let
   \[
       \alpha : V \to \mathrm{RMG}, \qquad \beta : E \to \mathrm{RMG}
   \]
   assign a meta-graph to each vertex and edge respectively.  The triple $(S, \alpha, \beta)$ is an element of $\mathrm{RMG}$.

Minimality ensures well-foundedness: every RMG is built by finitely many applications of these constructors, bottoming out at atoms.  In practice, atoms correspond to graph chunks whose `node_count = edge_count = 0` with payload stored in the bundle’s blob region.

---

## 2. Initial-Algebra View

Let $\mathcal{G}$ denote the set of shapes $S = (V,E,s,t)$. Consider the polynomial endofunctor on $\mathbf{Set}$
\[
F(X) = \coprod_{S \in \mathcal{G}} \bigl( (V \to X) \times (E \to X) \bigr) \; + \; P .
\]
An RMG is an element of the **initial algebra** $\mu F$.  This formulation yields:

- **Structural recursion (catamorphisms).** For any $F$-algebra $\varphi : F(X) \to X$, there exists a unique homomorphism $\operatorname{fold}_\varphi : \mathrm{RMG} \to X$ defined by
  \[
  \begin{aligned}
    \operatorname{fold}_\varphi(\operatorname{Atom}(p)) &= \varphi(\operatorname{inr}(p)), \\
    \operatorname{fold}_\varphi(S,\alpha,\beta) &= \varphi \bigl( \operatorname{inl}(S, \operatorname{fold}_\varphi \circ \alpha, \operatorname{fold}_\varphi \circ \beta) \bigr) .
  \end{aligned}
  \]
  This is the semantic foundation for `metagraph_fold(...)` style APIs.
- **Induction principle.** Properties $\mathcal{P}$ on $\mathrm{RMG}$ can be shown by checking $\mathcal{P}$ on atoms and assuming it holds for each attachment when proving it for $(S,\alpha,\beta)$.
- **Coinduction (optional).** Replacing $\mu F$ by the greatest fixed point $\nu F$ admits possibly infinite unfoldings, useful for live streaming or netlists with recursion.

---

## 3. Morphisms and Category Structure

A morphism $f : (S, \alpha, \beta) \to (S', \alpha', \beta')$ consists of:

- A graph homomorphism on 1-skeletons $f_V : V \to V'$, $f_E : E \to E'$ with $s' \circ f_E = f_V \circ s$ and $t' \circ f_E = f_V \circ t$.
- Compatible meta-morphisms $f_v : \alpha(v) \to \alpha'(f_V(v))$ and $f_e : \beta(e) \to \beta'(f_E(e))$, defined recursively.

With identity and composition defined pointwise, $\mathrm{RMG}$ becomes a category.  Pushouts along monomorphisms (needed for “splicing in” subgraphs) exist by standard results for adhesive categories and are implemented operationally by pointer rewrites in the loader.

---

## 4. Recursion Schemes and Unfoldings

- **Depth.** $\operatorname{depth}(\operatorname{Atom}(p)) = 0$, and
  \[
    \operatorname{depth}(S,\alpha,\beta) = 1 + \max \Bigl\{ \max_{v \in V} \operatorname{depth}(\alpha(v)), \max_{e \in E} \operatorname{depth}(\beta(e)) \Bigr\} .
  \]
- **$k$-unfolding.** Define $\llbracket G \rrbracket_k$ by recursively replacing attachments up to depth $k$ with their 1-skeletons.  The filtered colimit $\varinjlim_k \llbracket G \rrbracket_k$ is the infinite unfolding used for analysis or visualization.
- **Hylomorphisms / paramorphisms.** Standard recursion schemes (build-then-fold, folds with access to subterms) are inherited from the initial algebra semantics.

---

## 5. Graph Rewriting Perspective

The engineering design relies on constructive graph rewrites (e.g. inlining a node’s attachment).  Formally, we work in an adhesive category where DPO (double-pushout) rewriting is defined.  Concretely:

1. Extract the 1-skeleton pattern $L \hookrightarrow G$ to replace.
2. Use the attachment pointers to identify the interface.
3. Perform the pushout to splice in the replacement RMG.

Because each attachment is itself an RMG, rewrites recurse automatically: replacing a vertex or edge is equivalent to replacing its attached subgraph and updating offsets.  The chunk layout in the bundle (node/edge tables, `edge_data_idx`) is precisely the data needed to compute these pushouts in $O(1)$ pointer arithmetic.

---

## 6. Implementation Correspondence

| Mathematical Notion | On-Disk Representation | Loader Behaviour |
| --- | --- | --- |
| Shape $S=(V,E,s,t)$ | Graph chunk header (`node_count`, `edge_count`) + node/edge index arrays | Hydration reconstructs adjacency and offsets |
| Vertex attachment $\alpha(v)$ | Entry in node index referencing another graph chunk | `mg_graph_hydrate_node(...)` follows offset into mapped region |
| Edge attachment $\beta(e)$ | `edge_data_idx` (graph chunk containing semantics/pipeline) | `mg_graph_hydrate_edge(...)` pulls edge-graph lazily |
| Atom | `node_count = edge_count = 0`, payload stored as blob/string | Treated as leaf: hydration returns zero-degree graph |
| Fold $\operatorname{fold}_\varphi$ | `metagraph_fold(...)` style API | Provided callback `\varphi` is invoked once per chunk |
| Pushout splice | In-place pointer rewrite + integrity update | Implements DPO rewrite guaranteeing consistency |

This table ensures that the theoretical constructors map one-to-one to concrete chunk types and loader operations.  No “extra” structures appear in code that lack a slot in the mathematics, and vice versa.

---

## 7. Properties Worth Exploiting

1. **Universal property.** The initial algebra ensures uniqueness of interpreters.  Any evaluation or analysis pass must map `Atom` and `Graph` constructors; the fold generated is unique.
2. **Compositionality.** The (strict symmetric) monoidal structure inherited from graph composition allows parallel composition and nesting to commute with rewrites (useful for concurrency).
3. **Expressiveness.** Every RMG simultaneously generalizes node-replacement, edge-replacement, and hierarchical graph models—enabling nodes *and* edges to store semantics without losing regularity.
4. **Rewrite safety.** Adhesivity plus DPO rewriting provide confluence results and critical pair analysis, guiding safe live edits or builder transforms.
5. **Homotopy-flavoured reasoning.** Execution traces of rewrites form higher-dimensional paths; this opens the door to exploiting homotopy type theory or infinity-groupoid interpretations for proof-carrying pipelines.

---

## 8. Alignment with the Architecture Document

Sections §4 and §5 of `docs/architecture.md` describe arenas, chunk packets, hydration, and integrity checks.  The set-theoretic constructors here align exactly: atoms ↔ leaf chunks, $(S,\alpha,\beta)$ ↔ graph chunks with attachment tables.  The recursion schemes explain the folding APIs; the DPO view explains splice/inject operations.  In short: the maths and the code are in one-to-one correspondence.

---

### References and Further Reading

- Engelfriet & Rozenberg, *Node Replacement Graph Grammars*.
- Drewes et al., *Hyperedge Replacement Graph Grammars*.
- Lack & Sobociński, *Adhesive Categories*.
- Fong, *Decorated Cospans*.
- OpenCog / Hyperon Atomspace documentation (metagraph rewriting in practice).

