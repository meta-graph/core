#ifndef METAGRAPH_MATCH_H
#define METAGRAPH_MATCH_H

#include <stdbool.h>

#include "metagraph/base.h"
#include "metagraph/rule.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MATCH_MAX_TOUCHED_NODES 128U

typedef struct {
    uint32_t rule_id;
    uint8_t L_n;
    mg_node_id_t L2G_node[MG_RULE_MAX_NODES];
    uint16_t tn;
    mg_node_id_t touched_nodes[MG_MATCH_MAX_TOUCHED_NODES];
    uint64_t key_hi;
    uint64_t key_lo;
} mg_match_t;

typedef struct {
    mg_match_t *data;
    uint32_t count;
    uint32_t capacity;
} mg_match_set_t;

bool mg_match_set_init(mg_match_set_t *set, uint32_t capacity);
bool mg_match_set_reserve(mg_match_set_t *set, uint32_t min_capacity);
bool mg_match_set_push(mg_match_set_t *set, const mg_match_t *match);
void mg_match_set_clear(mg_match_set_t *set);
void mg_match_set_free(mg_match_set_t *set);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* METAGRAPH_MATCH_H */
