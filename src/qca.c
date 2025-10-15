#include "metagraph/qca.h"

#include <stdint.h>
#include <string.h>

#include "metagraph/arena.h"
#include "metagraph/base.h"
#include "metagraph/dpoi.h"
#include "metagraph/epoch.h"
#include "metagraph/hilbert.h"
#include "metagraph/match.h"
#include "metagraph/result.h"
#include "metagraph/rmg.h"
#include "metagraph/rule.h"

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
        memcpy(&aggregate->data[aggregate->count], per_rule.data,
               per_rule.count * sizeof(mg_match_t));
        aggregate->count += per_rule.count;
        mg_match_set_free(&per_rule);
    }
    return METAGRAPH_SUCCESS;
}

static void metagraph_qca_apply_matches(mg_hilbert_t *hilbert,
                                        const mg_match_set_t *schedule) {
    for (uint32_t index = 0; index < schedule->count; ++index) {
        const mg_match_t *match = &schedule->data[index];
        if (match->rule_id == 1 && match->L_n >= 1U) {
            const mg_node_id_t node = match->L2G_node[0];
            if (node < hilbert->node_count) {
                hilbert->node_bits[node] ^= 1U;
            }
        } else if (match->rule_id == 2 && match->L_n >= 2U) {
            const mg_node_id_t control = match->L2G_node[0];
            const mg_node_id_t target = match->L2G_node[1];
            if (control < hilbert->node_count && target < hilbert->node_count &&
                hilbert->node_bits[control] != 0U) {
                hilbert->node_bits[target] ^= 1U;
            }
        }
    }
}

metagraph_result_t mg_qca_apply_kernels(mg_hilbert_t *hilbert,
                                        const mg_rmg_t *rmg,
                                        const mg_rule_t *rules,
                                        const mg_match_set_t *schedule) {
    (void)rmg;
    (void)rules;
    METAGRAPH_VALIDATE_PTR(hilbert, "hilbert");
    METAGRAPH_VALIDATE_PTR(schedule, "schedule");
    metagraph_qca_apply_matches(hilbert, schedule);
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
    metrics->conflicts_dropped = 0U;

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
