#include "metagraph/qca.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "metagraph/arena.h"
#include "metagraph/base.h"
#include "metagraph/dpoi.h"
#include "metagraph/epoch.h"
#include "metagraph/graph.h"
#include "metagraph/hilbert.h"
#include "metagraph/match.h"
#include "metagraph/result.h"
#include "metagraph/rmg.h"
#include "metagraph/rule.h"

static bool metagraph_graph_find_index_by_id(const mg_graph_t *graph,
                                             mg_node_id_t node_id,
                                             uint32_t *out_index) {
    if (!graph || !out_index) {
        return false;
    }
    for (size_t index = 0; index < graph->node_count; ++index) {
        if (graph->nodes[index].id == node_id) {
            if (index > UINT32_MAX) {
                return false;
            }
            *out_index = (uint32_t)index;
            return true;
        }
    }
    return false;
}

static metagraph_result_t
metagraph_qca_collect_matches(const mg_rmg_t *rmg, const mg_rule_t *rules,
                              uint32_t rule_count, mg_arena_t *arena,
                              mg_match_set_t *aggregate) {
    aggregate->count = 0U;
    for (uint32_t rule_index = 0; rule_index < rule_count; ++rule_index) {
        mg_match_set_t per_rule = {0};
        metagraph_result_t result =
            mg_dpoi_match_rmg(rmg, &rules[rule_index], arena, &per_rule);
        if (metagraph_result_is_error(result)) {
            mg_match_set_free(&per_rule);
            return result;
        }
        if (!mg_match_set_reserve(aggregate,
                                  aggregate->count + per_rule.count)) {
            mg_match_set_free(&per_rule);
            return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                                 "unable to append matches");
        }
        const size_t bytes_to_copy = per_rule.count * sizeof(mg_match_t);
        memcpy(&aggregate->data[aggregate->count], per_rule.data,
               bytes_to_copy);
        aggregate->count += per_rule.count;
        mg_match_set_free(&per_rule);
    }
    return METAGRAPH_SUCCESS;
}

static void metagraph_qca_apply_matches(const mg_graph_t *graph,
                                        mg_hilbert_t *hilbert,
                                        const mg_match_set_t *schedule) {
    if (!graph || !hilbert || !schedule) {
        return;
    }
    for (uint32_t index = 0; index < schedule->count; ++index) {
        const mg_match_t *match = &schedule->data[index];
        if (match->rule_id == 1 && match->L_n >= 1U) {
            uint32_t node_index = 0U;
            if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[0],
                                                  &node_index)) {
                continue;
            }
            if (node_index < hilbert->node_count) {
                hilbert->node_bits[node_index] ^= 1U;
            }
            continue;
        }
        if (match->rule_id == 2 && match->L_n >= 2U) {
            uint32_t control_index = 0U;
            uint32_t target_index = 0U;
            if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[0],
                                                  &control_index)) {
                continue;
            }
            if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[1],
                                                  &target_index)) {
                continue;
            }
            if (control_index < hilbert->node_count &&
                target_index < hilbert->node_count &&
                hilbert->node_bits[control_index] != 0U) {
                hilbert->node_bits[target_index] ^= 1U;
            }
        }
    }
}

metagraph_result_t mg_qca_apply_kernels(mg_hilbert_t *hilbert,
                                        const mg_rmg_t *rmg,
                                        const mg_rule_t *rules,
                                        const mg_match_set_t *schedule) {
    METAGRAPH_VALIDATE_PTR(rmg, "rmg");
    (void)rules;
    METAGRAPH_VALIDATE_PTR(hilbert, "hilbert");
    METAGRAPH_VALIDATE_PTR(schedule, "schedule");
    const mg_graph_t *graph = rmg ? rmg->skel : NULL;
    metagraph_qca_apply_matches(graph, hilbert, schedule);
    return METAGRAPH_SUCCESS;
}

metagraph_result_t mg_qca_tick_rmg(mg_rmg_t *rmg, mg_hilbert_t *hilbert,
                                   const mg_rule_t *rules, uint32_t rule_count,
                                   mg_arena_t *arena, mg_epoch_t *epoch,
                                   mg_tick_metrics_t *metrics) {
    METAGRAPH_VALIDATE_PTR(rmg, "rmg");
    METAGRAPH_VALIDATE_PTR(hilbert, "hilbert");
    METAGRAPH_VALIDATE_PTR(rules, "rules");
    METAGRAPH_VALIDATE_PTR(metrics, "metrics");

    mg_match_set_t aggregate = {0};
    if (!mg_match_set_init(&aggregate, 64U)) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "unable to allocate match set");
    }

    metagraph_result_t result = metagraph_qca_collect_matches(
        rmg, rules, rule_count, arena, &aggregate);
    if (metagraph_result_is_error(result)) {
        mg_match_set_free(&aggregate);
        return result;
    }
    metrics->matches_found = aggregate.count;

    mg_dpoi_schedule_maximal(&aggregate);
    metrics->matches_kept = aggregate.count;
    metrics->conflicts_dropped =
        (metrics->matches_found > aggregate.count)
            ? (metrics->matches_found - aggregate.count)
            : 0U;

    result = mg_qca_apply_kernels(hilbert, rmg, rules, &aggregate);
    if (metagraph_result_is_error(result)) {
        mg_match_set_free(&aggregate);
        return result;
    }

    result = mg_dpo_commit(rmg->skel, rules, rule_count, &aggregate);
    if (metagraph_result_is_error(result)) {
        mg_match_set_free(&aggregate);
        return result;
    }

    if (epoch) {
        mg_epoch_flip(epoch);
    }

    metrics->ms_match = 0.0;
    metrics->ms_kernel = 0.0;
    metrics->ms_rewrite = 0.0;
    metrics->ms_total = 0.0;

    mg_match_set_free(&aggregate);
    return METAGRAPH_SUCCESS;
}
