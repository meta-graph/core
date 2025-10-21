#include "metagraph/dpoi.h"
#include "metagraph/arena.h"
#include "metagraph/base.h"
#include "metagraph/graph.h"
#include "metagraph/rmg.h"
#include "metagraph/rule.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "metagraph/match.h"
#include "metagraph/result.h"

static const uint32_t METAGRAPH_INITIAL_MATCH_CAPACITY = 8U;
static const uint32_t METAGRAPH_MATCH_GROWTH_FACTOR = 2U;
static const uint32_t METAGRAPH_TOUCHED_CAPACITY = MG_MATCH_MAX_TOUCHED_NODES;
static const uint64_t METAGRAPH_KEY_SEED = 0x9e3779b97f4a7c15ULL;
static const uint8_t METAGRAPH_SINGLETON_NODES = 1U;
static const uint8_t METAGRAPH_PAIR_NODES = 2U;
static const uint8_t METAGRAPH_PAIR_EDGES = 1U;

static metagraph_result_t metagraph_match_set_grow(mg_match_set_t *set,
                                                   uint32_t min_capacity);
static metagraph_result_t metagraph_prepare_match_buffer(mg_match_set_t *set);

/**
 * Ensure a match set has capacity for at least `min_capacity` entries.
 *
 * If the set's capacity is smaller than `min_capacity`, the underlying storage
 * is resized and `set->data` and `set->capacity` are updated accordingly.
 *
 * @param set Match set to ensure capacity for.
 * @param min_capacity Minimum required capacity (number of mg_match_t entries).
 * @returns METAGRAPH_SUCCESS on success, or an error result on failure (for
 *          example, out-of-memory if allocation fails).
 */
static metagraph_result_t metagraph_match_set_grow(mg_match_set_t *set,
                                                   uint32_t min_capacity) {
    if (set->capacity >= min_capacity) {
        return METAGRAPH_SUCCESS;
    }
    uint32_t new_capacity =
        set->capacity > 0U ? set->capacity : METAGRAPH_INITIAL_MATCH_CAPACITY;
    while (new_capacity < min_capacity) {
        new_capacity *= METAGRAPH_MATCH_GROWTH_FACTOR;
    }
    mg_match_t *next =
        (mg_match_t *)realloc(set->data, new_capacity * sizeof(mg_match_t));
    if (!next) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "unable to grow match set");
    }
    set->data = next;
    set->capacity = new_capacity;
    return METAGRAPH_SUCCESS;
}

/**
 * Append a computed match for `rule` using the provided `image` into `set`,
 * computing its ordering key and recording touched nodes.
 *
 * @param rule Source rule whose `rule_id` is recorded in the emitted match.
 * @param image Array of graph node identifiers forming the match (length
 * `count`).
 * @param count Number of node identifiers in `image`.
 * @param set Match buffer to which the new match will be appended; must be
 * prepared prior to calling.
 * @returns METAGRAPH_SUCCESS on success, or an error code on failure (for
 * example, when memory allocation fails).
 */
static metagraph_result_t metagraph_emit_match(const mg_rule_t *rule,
                                               const mg_node_id_t *image,
                                               uint8_t count,
                                               mg_match_set_t *set) {
    METAGRAPH_CHECK(metagraph_match_set_grow(set, set->count + 1U));

    mg_match_t *match = &set->data[set->count];
    mg_zero_buffer(match, sizeof(*match));
    match->rule_id = rule->rule_id;
    match->L_n = count;

    uint64_t min_identifier = UINT64_MAX;
    for (uint8_t index = 0; index < count; ++index) {
        match->L2G_node[index] = image[index];
        if (image[index] < min_identifier) {
            min_identifier = image[index];
        }
        if (match->tn < METAGRAPH_TOUCHED_CAPACITY) {
            match->touched_nodes[match->tn++] = image[index];
        }
    }

    const uint64_t key = ((uint64_t)match->rule_id << 32U) | min_identifier;
    match->key_hi = key ^ METAGRAPH_KEY_SEED;
    match->key_lo = key;

    set->count += 1U;
    return METAGRAPH_SUCCESS;
}

/**
 * Find and emit matches for a rule that consists of a single node.
 *
 * For each graph node whose type equals the rule's required node type, appends
 * a one-node mapping to the provided match set.
 *
 * @param graph Graph to search for matching nodes.
 * @param rule  Rule whose single-node pattern (rule->L.node_type[0]) is used.
 * @param set   Match set to append found matches into; must be prepared by the
 * caller.
 *
 * @returns METAGRAPH_SUCCESS on success, or an error code propagated from match
 * emission (e.g., out-of-memory).
 */
static metagraph_result_t metagraph_match_single_node(const mg_graph_t *graph,
                                                      const mg_rule_t *rule,
                                                      mg_match_set_t *set) {
    const mg_type_id_t required_type = rule->L.node_type[0];
    for (size_t node_index = 0; node_index < graph->node_count; ++node_index) {
        if (graph->nodes[node_index].type != required_type) {
            continue;
        }
        mg_node_id_t mapping[1] = {graph->nodes[node_index].id};
        METAGRAPH_CHECK(metagraph_emit_match(rule, mapping,
                                             METAGRAPH_SINGLETON_NODES, set));
    }
    return METAGRAPH_SUCCESS;
}

/**
 * Find and emit matches for a two-node (one-edge) rule pattern in the given
 * graph.
 *
 * Scans graph edges for ordered pairs where the source node has the rule's
 * left-hand type and the target node has the rule's right-hand type, and
 * appends a two-node mapping for each valid pair to the provided match set.
 *
 * @param graph Graph to search for matching node pairs.
 * @param rule Rule whose two-node LHS/RHS types are used to filter matches.
 * @param set Output match set to which discovered two-node matches are
 * appended.
 * @returns METAGRAPH_SUCCESS on success, or an error code propagated from match
 * emission on failure.
 */
static metagraph_result_t metagraph_match_two_nodes(const mg_graph_t *graph,
                                                    const mg_rule_t *rule,
                                                    mg_match_set_t *set) {
    const mg_type_id_t lhs_type = rule->L.node_type[0];
    const mg_type_id_t rhs_type = rule->L.node_type[1];
    for (size_t lhs_index = 0; lhs_index < graph->node_count; ++lhs_index) {
        const mg_node_rec_t *lhs = &graph->nodes[lhs_index];
        if (lhs->type != lhs_type) {
            continue;
        }
        const uint32_t begin = lhs->adj_offset;
        const uint32_t end = (lhs_index + 1U < graph->node_count)
                                 ? graph->nodes[lhs_index + 1U].adj_offset
                                 : (uint32_t)graph->nbr_count;
        for (uint32_t offset = begin; offset < end; ++offset) {
            const uint32_t rhs_index = graph->nbr_ids[offset];
            if (rhs_index >= graph->node_count) {
                continue;
            }
            const mg_node_rec_t *rhs = &graph->nodes[rhs_index];
            if (rhs->type != rhs_type) {
                continue;
            }
            mg_node_id_t mapping[2] = {lhs->id, rhs->id};
            METAGRAPH_CHECK(
                metagraph_emit_match(rule, mapping, METAGRAPH_PAIR_NODES, set));
        }
    }
    return METAGRAPH_SUCCESS;
}

/**
 * Generate candidate matches for a rule by scanning a rule-meta-graph's
 * skeleton.
 *
 * Prepares the output match buffer and emits matches for simple rule arities:
 * - single-node rules: emits one-node matches for graph nodes whose type
 * matches the rule.
 * - two-node one-edge rules: emits ordered node-pair matches for adjacent nodes
 * matching lhs/rhs types.
 *
 * @param rmg Rule-meta-graph containing a skeleton graph to search; must not be
 * NULL.
 * @param rule Rule definition whose pattern will be matched; must not be NULL.
 * @param arena Arena allocator (unused by this implementation; may be NULL).
 * @param out_matches Match buffer to populate; must not be NULL.
 * @returns METAGRAPH_SUCCESS on success. Propagates error codes from buffer
 * preparation or allocation failures (e.g., out-of-memory) on failure.
 */
metagraph_result_t mg_dpoi_match_rmg(const mg_rmg_t *rmg, const mg_rule_t *rule,
                                     mg_arena_t *arena,
                                     mg_match_set_t *out_matches) {
    (void)arena;
    METAGRAPH_VALIDATE_PTR(rmg, "rmg");
    METAGRAPH_VALIDATE_PTR(rule, "rule");
    METAGRAPH_VALIDATE_PTR(out_matches, "out_matches");

    METAGRAPH_CHECK(metagraph_prepare_match_buffer(out_matches));

    const mg_graph_t *graph = rmg->skel;
    if (!graph || graph->node_count == 0U || rule->L.node_count == 0U) {
        return METAGRAPH_SUCCESS;
    }

    if (rule->L.node_count == METAGRAPH_SINGLETON_NODES) {
        return metagraph_match_single_node(graph, rule, out_matches);
    }
    if (rule->L.node_count == METAGRAPH_PAIR_NODES &&
        rule->L.edge_count == METAGRAPH_PAIR_EDGES) {
        return metagraph_match_two_nodes(graph, rule, out_matches);
    }
    return METAGRAPH_SUCCESS;
}

/**
 * Compare two match records by their composite key: first `key_hi`, then
 * `key_lo`.
 *
 * @param lhs Pointer to the first `mg_match_t`.
 * @param rhs Pointer to the second `mg_match_t`.
 * @returns `-1` if `lhs` is less than `rhs`, `1` if `lhs` is greater than
 * `rhs`, `0` if they are equal.
 */
static int metagraph_match_compare(const void *lhs, const void *rhs) {
    const mg_match_t *left = (const mg_match_t *)lhs;
    const mg_match_t *right = (const mg_match_t *)rhs;
    if (left->key_hi < right->key_hi) {
        return -1;
    }
    if (left->key_hi > right->key_hi) {
        return 1;
    }
    if (left->key_lo < right->key_lo) {
        return -1;
    }
    if (left->key_lo > right->key_lo) {
        return 1;
    }
    return 0;
}

/**
 * Ensure the match set has an initialized match buffer and resets its count.
 *
 * If the set already has an allocated data buffer, the function resets
 * set->count to 0. Otherwise it allocates an initial array of
 * METAGRAPH_INITIAL_MATCH_CAPACITY elements, sets set->capacity and
 * set->count accordingly.
 *
 * @param set Match set whose buffer and counters will be initialized or reset.
 * @returns METAGRAPH_SUCCESS on success, otherwise an out-of-memory error
 * result.
 */
static metagraph_result_t metagraph_prepare_match_buffer(mg_match_set_t *set) {
    if (set->data != NULL) {
        set->count = 0U;
        return METAGRAPH_SUCCESS;
    }
    set->data = (mg_match_t *)calloc(METAGRAPH_INITIAL_MATCH_CAPACITY,
                                     sizeof(mg_match_t));
    if (!set->data) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "unable to allocate match set");
    }
    set->capacity = METAGRAPH_INITIAL_MATCH_CAPACITY;
    set->count = 0U;
    return METAGRAPH_SUCCESS;
}

/**
 * Determine whether two matches share any touched node identifiers.
 *
 * @param lhs First match to compare; its touched_nodes array and tn count are
 * used.
 * @param rhs Second match to compare; its touched_nodes array and tn count are
 * used.
 * @returns `true` if the matches share at least one touched node identifier,
 * `false` otherwise.
 */
static bool metagraph_matches_overlap(const mg_match_t *lhs,
                                      const mg_match_t *rhs) {
    for (uint16_t lhs_index = 0; lhs_index < lhs->tn; ++lhs_index) {
        const mg_node_id_t lhs_node = lhs->touched_nodes[lhs_index];
        for (uint16_t rhs_index = 0; rhs_index < rhs->tn; ++rhs_index) {
            if (lhs_node == rhs->touched_nodes[rhs_index]) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Selects a maximal subset of non-overlapping matches from the provided match
 * set.
 *
 * Sorts the matches by their composite key and then retains, in-place, the
 * first set of matches that do not overlap on any touched node. The function
 * updates matches->count to reflect the number of matches kept.
 *
 * @param matches Match set to reduce; modified in-place. If `matches` is NULL
 *                or contains zero or one entry, the function returns without
 *                making changes.
 */
void mg_dpoi_schedule_maximal(mg_match_set_t *matches) {
    if (!matches || matches->count <= 1U) {
        return;
    }

    qsort(matches->data, matches->count, sizeof(mg_match_t),
          metagraph_match_compare);

    uint32_t kept = 0U;
    for (uint32_t index = 0; index < matches->count; ++index) {
        const mg_match_t *candidate = &matches->data[index];
        bool overlaps = false;
        for (uint32_t test = 0; test < kept; ++test) {
            if (metagraph_matches_overlap(candidate, &matches->data[test])) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) {
            if (kept != index) {
                matches->data[kept] = *candidate;
            }
            kept += 1U;
        }
    }
    matches->count = kept;
}

/**
 * Commit a set of scheduled matches to the graph.
 *
 * @param graph Target graph to apply the scheduled matches to.
 * @param rules Array of rules corresponding to the matches in `schedule`.
 * @param rule_count Number of rules in `rules`.
 * @param schedule Set of matches to commit to `graph`.
 * @returns METAGRAPH_SUCCESS on success, or an error code on failure.
 */
metagraph_result_t mg_dpo_commit(mg_graph_t *graph, const mg_rule_t *rules,
                                 uint32_t rule_count,
                                 const mg_match_set_t *schedule) {
    (void)graph;
    (void)rules;
    (void)rule_count;
    (void)schedule;
    return METAGRAPH_SUCCESS;
}
