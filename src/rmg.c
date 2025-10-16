#include "metagraph/rmg.h"

#include <stdbool.h>
#include <stdint.h>

bool mg_rmg_hydrate_node_att(const mg_rmg_t *rmg, uint32_t node_index,
                             const void **attachment, mg_att_kind_t *kind) {
    if (!rmg || !attachment || !kind) {
        return false;
    }
    if (!rmg->node_att || node_index >= rmg->skel->node_count) {
        *attachment = NULL;
        *kind = MG_ATT_NONE;
        return true;
    }
    const mg_attach_ref_t *ref = &rmg->node_att[node_index];
    *kind = ref->kind;
    *attachment = NULL; // real implementation should hydrate offset -> pointer
    (void)ref;
    return true;
}

bool mg_rmg_hydrate_edge_att(const mg_rmg_t *rmg, uint32_t edge_index,
                             const void **attachment, mg_att_kind_t *kind) {
    if (!rmg || !attachment || !kind) {
        return false;
    }
    if (!rmg->edge_att || edge_index >= rmg->skel->edge_count) {
        *attachment = NULL;
        *kind = MG_ATT_NONE;
        return true;
    }
    const mg_attach_ref_t *ref = &rmg->edge_att[edge_index];
    *kind = ref->kind;
    *attachment = NULL;
    // TODO: hydrate attachment offsets once caching layer is implemented
    (void)ref;
    return true;
}
