#ifndef METAGRAPH_DPOI_H
#define METAGRAPH_DPOI_H

#include <stdbool.h>

#include "metagraph/arena.h"
#include "metagraph/match.h"
#include "metagraph/rmg.h"
#include "metagraph/rule.h"

metagraph_result_t mg_dpoi_match_rmg(const mg_rmg_t *rmg, const mg_rule_t *rule,
                                     mg_arena_t *arena,
                                     mg_match_set_t *out_matches);

void mg_dpoi_schedule_maximal(mg_match_set_t *matches);

metagraph_result_t mg_dpo_commit(mg_graph_t *graph, const mg_rule_t *rules,
                                 uint32_t rule_count,
                                 const mg_match_set_t *schedule);

#endif /* METAGRAPH_DPOI_H */
