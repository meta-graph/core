# F.013 – DPOI Matcher and QCA Evolution Loop

## Feature Overview

The DPOI matcher and QCA loop implement the “physics mode” runtime for MetaGraph’s RMG forge.  Rules expressed as typed open-graph double-pushouts with interfaces (DPOI) are matched against the live metagraph, scheduled into a conflict-free batch, evolved via local quantum cellular automata (QCA) kernels, and finally committed as pointer-level DPO rewrites.  This feature builds the executable substrate for graph-native physics simulations described in the forge roadmap.

## Priority
**Critical** – required for simulation and rule-enforcement workflows

## Dependencies
- F.001 – Core MetaGraph Data Model
- F.003 – Memory-Mapped I/O Operations
- F.008 – Thread-Safe Graph Access (SWMR epochs, arenas)
- F.009 – Memory Pool Management
- F.011 – Error Handling and Validation

## Purpose & Scope

### In Scope (v1)
- Typed open-graph rules `L <- K -> R` with explicit interface legs
- Deterministic DPOI matching using CSR-backed host snapshots
- Greedy maximal independent set (MIS) scheduling on match “touched” sets
- Application of bounded-radius unitary or isometric QCA kernels on node Hilbert registers
- Pointer-level DPO pushouts committed under single-writer epochs with journaling and rollback
- Instrumentation: per-tick metrics, deterministic seeds, causal event logs

### Out of Scope (defer to future work)
- Probabilistic rule selection or amplitude/path integrals
- Edge Hilbert spaces, global measurement, or non-local kernels
- Distributed/GPU matching, multi-writer concurrency, or multi-process scheduling
- Automatic garbage compaction or Merkle integrity maintenance beyond existing hooks
- Nondeterministic exploration modes (optional later behind a feature flag)

## User Stories

1. **F013.US001 – Match Typed Open-Graph Rules**
   - *As a* forge engineer
   - *I want* the runtime to find all legal embeddings of a rule’s left-hand side within the host graph
   - *So that* I can evolve the metagraph according to physics-like local laws
   - **Acceptance Criteria**
     - Matches respect node/edge types, interface legs, port arity, and degree constraints
     - Dangling and identification gluing conditions are enforced
     - Each match surfaces embeddings, touched sets, and deterministic ordering keys

2. **F013.US002 – Schedule Conflict-Free Local Events**
   - *As a* simulation operator
   - *I want* overlapping matches to be resolved deterministically
   - *So that* kernels operate on disjoint neighborhoods and evolution remains causal
   - **Acceptance Criteria**
     - Greedy MIS scheduling removes overlapping matches based on lexicographic keys
     - Touched sets include rule halos (neighborhood radius) to enforce locality
     - Scheduling runs in O(n log n) for n matches using arena-allocated scratch space

3. **F013.US003 – Apply Local QCA Kernels**
   - *As a* physics designer
   - *I want* each scheduled match to execute its unitary/isometry on the bound Hilbert registers
   - *So that* state evolution mirrors rule semantics
   - **Acceptance Criteria**
     - Kernels operate only on registers inside the touched set halo
     - Kernel failures trigger tick rollback and increment failure counters
     - SPLIT/MERGE kernels allocate or reclaim ancilla per rule definition

4. **F013.US004 – Commit Pointer-Level DPO Rewrites**
   - *As a* runtime engineer
   - *I want* graph modifications to be journaled and committed atomically
   - *So that* readers see consistent snapshots and invariants hold
   - **Acceptance Criteria**
     - Deleted nodes/edges (`L \ K`) are removed only if no dangling connections remain outside permitted interfaces
     - Preserved elements (`K`) retain type information unless rules explicitly request mutation
     - Created elements (`R \ K`) receive stable IDs, adjacency updates, and Hilbert bindings
     - Epoch flips occur only after adjacency tables and indices are rebuilt

5. **F013.US005 – Observe and Reproduce Evolution**
   - *As a* researcher
   - *I want* the system to log per-tick metrics and deterministic seeds
   - *So that* experiments are repeatable and diagnosable
   - **Acceptance Criteria**
     - Tick results include counts of matches found/kept/dropped, failures, time slices
     - Event logs record rule IDs, match keys, touched IDs, and causal parents
     - Re-running with the same seed reproduces identical match scheduling and state

## API Design

### Public Headers

#### `include/metagraph/dpoi.h`
```c
typedef uint64_t mg_node_id_t;
typedef uint64_t mg_edge_id_t;
typedef uint32_t mg_type_id_t;

typedef struct {
  mg_node_id_t id;
  mg_type_id_t type;
  uint32_t     degree;
  uint64_t     adj_offset;  // CSR neighbor index
} mg_node_rec_t;

typedef struct {
  mg_edge_id_t id;
  mg_type_id_t type;
  mg_node_id_t src;
  mg_node_id_t dst;
  uint32_t     flags;
  uint32_t     data_offset; // pointer into attachment table
} mg_edge_rec_t;

typedef struct {
  const mg_node_rec_t* nodes;
  const uint32_t*      nbr_ids;   // CSR neighbour indices into nodes[]
  size_t               node_count;
  size_t               nbr_count;
  const mg_edge_rec_t* edges;
  size_t               edge_count;
  uint64_t             epoch;
} mg_graph_snapshot_t;

enum {
  MG_RULE_MAX_NODES = 16,
  MG_RULE_MAX_EDGES = 24,
  MG_MATCH_MAX_TOUCHED_NODES = 128,
};

typedef struct {
  // Compact pattern graphs (<=16 nodes / <=24 edges)
  const mg_node_rec_t* L_nodes;
  const mg_edge_rec_t* L_edges;
  const mg_node_rec_t* K_nodes;
  const mg_edge_rec_t* K_edges;
  const mg_node_rec_t* R_nodes;
  const mg_edge_rec_t* R_edges;
  uint8_t L_node_count, L_edge_count;
  uint8_t K_node_count, K_edge_count;
  uint8_t R_node_count, R_edge_count;

  uint8_t K_to_L_node[16];
  uint8_t K_to_R_node[16];
  uint8_t K_to_L_edge[24];
  uint8_t K_to_R_edge[24];

  mg_type_id_t interface_in_type;
  mg_type_id_t interface_out_type;

  uint16_t kernel_radius;
  uint32_t kernel_id;
  uint32_t rule_id;
} mg_rule_t;

typedef struct {
  uint32_t     rule_id;
  uint8_t      L_n;
  mg_node_id_t L2G_node[MG_RULE_MAX_NODES];
  uint16_t     tn;
  mg_node_id_t touched_nodes[MG_MATCH_MAX_TOUCHED_NODES];
  uint64_t     key_hi;
  uint64_t     key_lo;
} mg_match_t;

typedef struct {
  mg_match_t* data;
  uint32_t    count;
  uint32_t    capacity;
} mg_match_set_t;

bool mg_dpoi_match(const mg_graph_snapshot_t* graph,
                   const mg_rule_t* rule,
                   mg_match_set_t* out_matches);

void mg_dpoi_schedule_maximal(mg_match_set_t* matches);
```

#### `include/metagraph/qca.h`
```c
typedef struct mg_hilbert mg_hilbert_t;
typedef struct mg_writer  mg_writer_t;

typedef struct {
  uint32_t matches_found;
  uint32_t matches_kept;
  uint32_t conflicts_dropped;
  uint32_t pushout_failures;
  uint32_t kernel_failures;
  double   ms_match;
  double   ms_kernel;
  double   ms_rewrite;
  double   ms_total;
  uint64_t deterministic_seed;
} mg_tick_metrics_t;

typedef struct {
  uint32_t rule_id;
  uint64_t match_key_hi;
  uint64_t match_key_lo;
  const mg_node_id_t* touched_nodes;
  uint16_t touched_node_count;
  const mg_edge_id_t* touched_edges;
  uint16_t touched_edge_count;
  const uint32_t* parent_event_indices;
  uint16_t parent_event_count;
} mg_tick_event_t;

typedef struct {
  mg_tick_metrics_t metrics;
  mg_tick_event_t*  events;
  uint32_t          event_count;
  uint64_t          epoch_after;
} mg_tick_result_t;

bool mg_qca_apply_kernels(mg_hilbert_t* hilbert,
                          const mg_graph_snapshot_t* graph,
                          const mg_rule_t* rules,
                          const mg_match_set_t* schedule);

bool mg_dpo_commit(mg_writer_t* writer,
                   const mg_graph_snapshot_t* prev_snapshot,
                   const mg_rule_t* rules,
                   const mg_match_set_t* schedule,
                   mg_tick_event_t* out_events,
                   uint32_t* out_event_count);

bool mg_qca_tick(mg_writer_t* writer,
                 mg_hilbert_t* hilbert,
                 const mg_rule_t* rules,
                 uint32_t rule_count,
                 mg_tick_result_t* out_result);
```

## Implementation Notes

### Matching
- Use VF2-style backtracking on CSR graph data.  Prune on type equality, degree bounds, and optional port arity.
- Gluing conditions:
  - **Dangling:** deleting `L \ K` must not leave unmatched incident edges unless they correspond to typed interface legs.
  - **Identification:** distinct nodes/edges of `L` may not map to the same host element unless K explicitly identifies them.
- Build `touched_nodes`/`touched_edges` by combining `image(L)` with a BFS halo defined by `kernel_radius`.
- Allocate match buffers from the per-thread arena.  Release using scope pop after scheduling.

### Scheduling
- Sort matches by `(rule_id, min L node ID, min L edge ID, key hash)` using arena scratch.
- Greedy MIS: iterate sorted list, accept a match if its touched sets are disjoint from all accepted matches; otherwise drop and increment `conflicts_dropped`.
- Deterministic seed and order required for reproducible results.

### Kernel Application
- Kernels operate on node Hilbert registers.  The Hilbert handle provides APIs to map node IDs to register offsets and allocate ancilla for SPLIT/MERGE kernels.
- Apply kernels in scheduled order even though they are non-overlapping.  Failures abort the tick and trigger rollback.
- Ensure kernels cannot modify state outside their halo; instrumentation should assert this during debug builds.

### DPO Commit
- Maintain a journal per tick (arena-backed) containing:
  - Deleted node/edge IDs
  - Created node/edge descriptors
  - Rewire operations for preserved edges
  - Hilbert allocation/free operations
- For each match (scheduled order):
  1. Compute pushout complement (context G \ L) using prior journal state.
  2. Remove `L \ K` from the host (mark free in adjacency tables).
  3. Preserve K nodes and edges, applying type changes only if authorized.
  4. Instantiate `R \ K` with fresh stable IDs; insert into adjacency CSR structures and Hilbert register.
  5. Update integrity metadata (e.g., Merkle driver hooks) when new attachments are formed.
- After all rewrites succeed, rebuild adjacency snapshots if needed, flip the SWMR epoch, and publish the new snapshot to readers.
- On any failure, roll back journal entries in reverse order and restore the previous epoch.

### Metrics & Logging
- Record per-phase wall-clock timings using the high-resolution timers from F.010.
- Emit match statistics, failure counts, seed, and allocation deltas through the trace buffer described in the architecture doc.
- Event log entries include causal parent indices for matches that preserved or modified the same K elements earlier in the tick.

## Test Plan

### Unit Tests
1. **Matcher correctness** – simple host path graph with unary rule; assert all embeddings found and keys deterministic.
2. **Dangling rejection** – rule deleting a node with extra host neighbors outside interface must be rejected.
3. **Identification prevention** – ensure two distinct L nodes cannot map to the same host node unless identified via K.

### Integration Tests
1. **QCA Tick Success (T1)** – Host `Q–W–Q–W–Q`, rule APPLY_X on Q nodes:
   - Expect three matches, zero conflicts, graph unchanged, Hilbert registers updated.
2. **Overlap Resolution (T2)** – Host `Q0–W–Q1–W–Q2`, rules CNOT(Qi,Qi+1) via W and SPLIT(W):
   - Tick 1 keeps CNOT only, dropping overlapping SPLIT; Tick 2 applies SPLIT if legal after CNOT.
   - Validate deterministic selection, graph invariants, and Hilbert ancilla usage.

### Failure & Rollback Tests
1. **Kernel failure** – Inject failing kernel; ensure tick aborts, graph snapshot unchanged, and metrics reflect failure.
2. **Rollback integrity** – After forced rollback, verify adjacency tables, type counts, and Hilbert bindings match pre-tick state byte-for-byte.
3. **Determinism** – Re-run tick with same seed; expect identical match ordering, schedule, and resulting state.

### Performance / Load Tests
- Pattern size stress: rules approaching the 16-node / 24-edge limit on hosts with ≥10⁵ nodes; ensure matcher obeys time budgets and reports metrics.
- Scheduling scale: thousands of potential matches; verify O(n log n) behavior and memory usage limited to arena scopes.

## Future Work & Open Questions
- Extend kernels to edge Hilbert registers and non-unitary operations (measurements).
- Support probabilistic rule choice and amplitude tracking for superposition simulations.
- Integrate distributed matching or GPU acceleration when F.008 is extended to multi-writer regimes.
- Add builder support to persist applied rewrites back into GRAPHITE bundles.

---

**References**
- `docs/rmg-math.md` – formal correspondence between DPO pushouts and pointer rewrites
- `docs/architecture.md` – arena allocation, CSR layout, SWMR snapshots, trace buffers
- `docs/formal-spec.md` – overall GRAPHITE binary format and runtime goals
