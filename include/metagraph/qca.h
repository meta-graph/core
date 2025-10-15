#ifndef METAGRAPH_QCA_H
#define METAGRAPH_QCA_H

#include "metagraph/dpoi.h"
#include "metagraph/epoch.h"
#include "metagraph/hilbert.h"
#include "metagraph/rmg.h"

typedef struct {
    uint32_t matches_found;
    uint32_t matches_kept;
    uint32_t conflicts_dropped;
    double ms_match;
    double ms_kernel;
    double ms_rewrite;
    double ms_total;
} mg_tick_metrics_t;

metagraph_result_t mg_qca_apply_kernels(mg_hilbert_t *hilbert,
                                        const mg_rmg_t *rmg,
                                        const mg_rule_t *rules,
                                        const mg_match_set_t *schedule);

metagraph_result_t mg_qca_tick_rmg(mg_rmg_t *rmg, mg_hilbert_t *hilbert,
                                   const mg_rule_t *rules, uint32_t rule_count,
                                   mg_arena_t *arena, mg_epoch_t *epoch,
                                   mg_tick_metrics_t *metrics);

#endif /* METAGRAPH_QCA_H */
