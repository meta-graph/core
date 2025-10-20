#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#ifdef __APPLE__
#include <mach/mach_time.h>
#else
#include <sys/time.h>
#endif

#include "metagraph/qca.h"

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

typedef struct {
    bool have_start;
    bool have_end;
    struct timespec start;
    struct timespec end;
} mg_qca_timer_t;

static mg_qca_timer_t metagraph_timer_create(void) {
    mg_qca_timer_t timer = {
        .have_start = false,
        .have_end = false,
        .start = {0, 0},
        .end = {0, 0},
    };
    return timer;
}

static bool metagraph_monotonic_now(struct timespec *out);
static void metagraph_timer_begin(mg_qca_timer_t *timer);
static void metagraph_timer_end(mg_qca_timer_t *timer);
static double metagraph_timer_ms(const mg_qca_timer_t *timer);
static metagraph_result_t
metagraph_qca_collect_matches(const mg_rmg_t *rmg, const mg_rule_t *rules,
                              uint32_t rule_count, mg_arena_t *arena,
                              mg_match_set_t *aggregate);
static void metagraph_qca_apply_rule_x(const mg_graph_t *graph,
                                       mg_hilbert_t *hilbert,
                                       const mg_match_t *match);
static void metagraph_qca_apply_rule_cnot(const mg_graph_t *graph,
                                          mg_hilbert_t *hilbert,
                                          const mg_match_t *match);
static void metagraph_qca_apply_rule_split_w(const mg_graph_t *graph,
                                             const mg_match_t *match);

static double metagraph_timespec_diff_ms(const struct timespec *start,
                                         const struct timespec *end) {
    if (!start || !end) {
        return 0.0;
    }
    time_t sec_diff = end->tv_sec - start->tv_sec;
    long nsec_diff = end->tv_nsec - start->tv_nsec;
    if (nsec_diff < 0) {
        --sec_diff;
        nsec_diff += 1000000000L;
    }
    const double milliseconds_from_seconds = (double)sec_diff * 1000.0;
    const double milliseconds_from_nanoseconds = (double)nsec_diff / 1.0e6;
    return milliseconds_from_seconds + milliseconds_from_nanoseconds;
}

static metagraph_result_t
metagraph_qca_stage_match(mg_rmg_t *rmg, const mg_rule_t *rules,
                          uint32_t rule_count, mg_arena_t *arena,
                          mg_match_set_t *aggregate, mg_tick_metrics_t *metrics,
                          mg_qca_timer_t *timer) {
    if (timer) {
        metagraph_timer_begin(timer);
    }
    metagraph_result_t result =
        metagraph_qca_collect_matches(rmg, rules, rule_count, arena, aggregate);
    if (timer) {
        metagraph_timer_end(timer);
    }
    if (metagraph_result_is_error(result)) {
        return result;
    }

    metrics->matches_found = aggregate->count;

    mg_dpoi_schedule_maximal(aggregate);
    metrics->matches_kept = aggregate->count;
    metrics->conflicts_dropped =
        (metrics->matches_found > aggregate->count)
            ? (metrics->matches_found - aggregate->count)
            : 0U;

    return METAGRAPH_SUCCESS;
}

static metagraph_result_t metagraph_qca_stage_kernel(
    mg_hilbert_t *hilbert, const mg_rmg_t *rmg, const mg_rule_t *rules,
    const mg_match_set_t *aggregate, mg_qca_timer_t *timer) {
    if (timer) {
        metagraph_timer_begin(timer);
    }
    metagraph_result_t result =
        mg_qca_apply_kernels(hilbert, rmg, rules, aggregate);
    if (timer) {
        metagraph_timer_end(timer);
    }
    return result;
}

static metagraph_result_t
metagraph_qca_stage_rewrite(mg_rmg_t *rmg, const mg_rule_t *rules,
                            uint32_t rule_count, mg_match_set_t *aggregate,
                            mg_epoch_t *epoch, mg_qca_timer_t *timer) {
    if (timer) {
        metagraph_timer_begin(timer);
    }
    metagraph_result_t result =
        mg_dpo_commit(rmg->skel, rules, rule_count, aggregate);
    if (timer) {
        metagraph_timer_end(timer);
    }
    if (metagraph_result_is_error(result)) {
        return result;
    }
    if (epoch) {
        mg_epoch_flip(epoch);
    }
    return METAGRAPH_SUCCESS;
}

static bool metagraph_monotonic_now(struct timespec *out) {
    if (!out) {
        return false;
    }
#ifdef __APPLE__
    static mach_timebase_info_data_t timebase_info = {0};
    if (timebase_info.denom == 0) {
        if (mach_timebase_info(&timebase_info) != 0) {
            return false;
        }
    }
    const uint64_t absolute = mach_absolute_time();
    const uint64_t nanoseconds = absolute * (uint64_t)timebase_info.numer /
                                 (uint64_t)timebase_info.denom;
    out->tv_sec = (time_t)(nanoseconds / 1000000000ULL);
    out->tv_nsec = (long)(nanoseconds % 1000000000ULL);
    return true;
#else
#if defined(TIME_MONOTONIC)
    if (timespec_get(out, TIME_MONOTONIC) != 0) {
        return true;
    }
#endif
    if (timespec_get(out, TIME_UTC) != 0) {
        return true;
    }

    struct timeval wall_time;
    if (gettimeofday(&wall_time, NULL) != 0) {
        return false;
    }
    out->tv_sec = wall_time.tv_sec;
    out->tv_nsec = (long)wall_time.tv_usec * 1000L;
    return true;
#endif
}

static void metagraph_timer_begin(mg_qca_timer_t *timer) {
    if (!timer) {
        return;
    }
    timer->have_start = metagraph_monotonic_now(&timer->start);
    timer->have_end = false;
}

static void metagraph_timer_end(mg_qca_timer_t *timer) {
    if (!timer || !timer->have_start) {
        return;
    }
    timer->have_end = metagraph_monotonic_now(&timer->end);
}

static double metagraph_timer_ms(const mg_qca_timer_t *timer) {
    if (!timer || !timer->have_start || !timer->have_end) {
        return 0.0;
    }
    return metagraph_timespec_diff_ms(&timer->start, &timer->end);
}

static void metagraph_qca_apply_rule_x(const mg_graph_t *graph,
                                       mg_hilbert_t *hilbert,
                                       const mg_match_t *match) {
    if (!graph || !hilbert || !match || match->L_n < 1U) {
        return;
    }
    uint32_t node_index = 0U;
    if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[0],
                                          &node_index)) {
        return;
    }
    if (node_index < hilbert->node_count) {
        hilbert->node_bits[node_index] ^= 1U;
    }
}

static void metagraph_qca_apply_rule_cnot(const mg_graph_t *graph,
                                          mg_hilbert_t *hilbert,
                                          const mg_match_t *match) {
    if (!graph || !hilbert || !match || match->L_n < 2U) {
        return;
    }
    uint32_t control_index = 0U;
    uint32_t target_index = 0U;
    if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[0],
                                          &control_index)) {
        return;
    }
    if (!metagraph_graph_find_index_by_id(graph, match->L2G_node[1],
                                          &target_index)) {
        return;
    }
    if (control_index < hilbert->node_count &&
        target_index < hilbert->node_count &&
        hilbert->node_bits[control_index] != 0U) {
        hilbert->node_bits[target_index] ^= 1U;
    }
}

static void metagraph_qca_apply_rule_split_w(const mg_graph_t *graph,
                                             const mg_match_t *match) {
    (void)graph;
    (void)match;
    /*
     * The split_w rule rewires topology by inserting an intermediate node.
     * The Hilbert state for existing nodes is unaffected and newly created
     * nodes are initialised during commit, so no additional work is required
     * here beyond acknowledging the match.
     */
}

static bool metagraph_timer_valid(const mg_qca_timer_t *timer) {
    if (!timer) {
        return false;
    }
    if (!timer->have_start) {
        return false;
    }
    if (!timer->have_end) {
        return false;
    }
    return true;
}

static void metagraph_qca_zero_metrics(mg_tick_metrics_t *metrics) {
    if (!metrics) {
        return;
    }
    metrics->matches_found = 0U;
    metrics->matches_kept = 0U;
    metrics->conflicts_dropped = 0U;
    metrics->ms_match = 0.0;
    metrics->ms_kernel = 0.0;
    metrics->ms_rewrite = 0.0;
    metrics->ms_total = 0.0;
}

static void metagraph_qca_finalize_metrics(const mg_qca_timer_t *match_timer,
                                           const mg_qca_timer_t *kernel_timer,
                                           const mg_qca_timer_t *rewrite_timer,
                                           const mg_qca_timer_t *total_timer,
                                           mg_tick_metrics_t *metrics) {
    if (!metrics) {
        return;
    }

    const double match_ms = metagraph_timer_ms(match_timer);
    const double kernel_ms = metagraph_timer_ms(kernel_timer);
    const double rewrite_ms = metagraph_timer_ms(rewrite_timer);
    const bool total_valid = metagraph_timer_valid(total_timer);
    const double total_ms = metagraph_timer_ms(total_timer);

    metrics->ms_match = match_ms;
    metrics->ms_kernel = kernel_ms;
    metrics->ms_rewrite = rewrite_ms;
    if (total_valid) {
        metrics->ms_total = total_ms;
    } else {
        metrics->ms_total = match_ms + kernel_ms + rewrite_ms;
    }
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
        const size_t available_bytes =
            (aggregate->capacity - aggregate->count) * sizeof(mg_match_t);
        const size_t copied =
            mg_copy_bytes(&aggregate->data[aggregate->count], available_bytes,
                          per_rule.data, bytes_to_copy, bytes_to_copy);
        if (copied != bytes_to_copy) {
            mg_match_set_free(&per_rule);
            return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                                 "unable to append matches");
        }
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
        switch (match->rule_id) {
        case 1U:
            metagraph_qca_apply_rule_x(graph, hilbert, match);
            break;
        case 2U:
            metagraph_qca_apply_rule_cnot(graph, hilbert, match);
            break;
        case 3U:
            metagraph_qca_apply_rule_split_w(graph, match);
            break;
        default:
            break;
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

static metagraph_result_t
metagraph_qca_tick_body(mg_rmg_t *rmg, mg_hilbert_t *hilbert,
                        const mg_rule_t *rules, uint32_t rule_count,
                        mg_arena_t *arena, mg_epoch_t *epoch,
                        mg_tick_metrics_t *metrics) {
    mg_qca_timer_t total_timer = metagraph_timer_create();
    mg_qca_timer_t match_timer = metagraph_timer_create();
    mg_qca_timer_t kernel_timer = metagraph_timer_create();
    mg_qca_timer_t rewrite_timer = metagraph_timer_create();

    metagraph_timer_begin(&total_timer);

    mg_match_set_t aggregate = {0};
    if (!mg_match_set_init(&aggregate, 64U)) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "unable to allocate match set");
    }

    metagraph_result_t status = metagraph_qca_stage_match(
        rmg, rules, rule_count, arena, &aggregate, metrics, &match_timer);
    if (metagraph_result_is_error(status)) {
        goto failure;
    }

    status = metagraph_qca_stage_kernel(hilbert, rmg, rules, &aggregate,
                                        &kernel_timer);
    if (metagraph_result_is_error(status)) {
        goto failure;
    }

    status = metagraph_qca_stage_rewrite(rmg, rules, rule_count, &aggregate,
                                         epoch, &rewrite_timer);
    if (metagraph_result_is_error(status)) {
        goto failure;
    }

    metagraph_timer_end(&total_timer);
    metagraph_qca_finalize_metrics(&match_timer, &kernel_timer, &rewrite_timer,
                                   &total_timer, metrics);
    mg_match_set_free(&aggregate);
    return METAGRAPH_SUCCESS;

failure:
    metagraph_timer_end(&total_timer);
    metagraph_qca_zero_metrics(metrics);
    mg_match_set_free(&aggregate);
    return status;
}

metagraph_result_t mg_qca_tick_rmg(mg_rmg_t *rmg, mg_hilbert_t *hilbert,
                                   const mg_rule_t *rules, uint32_t rule_count,
                                   mg_arena_t *arena, mg_epoch_t *epoch,
                                   mg_tick_metrics_t *metrics) {
    METAGRAPH_VALIDATE_PTR(rmg, "rmg");
    METAGRAPH_VALIDATE_PTR(hilbert, "hilbert");
    METAGRAPH_VALIDATE_PTR(rules, "rules");
    METAGRAPH_VALIDATE_PTR(metrics, "metrics");

    return metagraph_qca_tick_body(rmg, hilbert, rules, rule_count, arena,
                                   epoch, metrics);
}
