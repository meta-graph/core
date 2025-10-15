#include "metagraph/rmg.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * Retrieve the hydrated attachment and its kind for a node in an RMG.
 *
 * If the RMG has no node attachments or node_index is out of range, sets
 * `*attachment` to NULL and `*kind` to `MG_ATT_NONE`. If an attachment
 * reference exists, sets `*kind` to the reference's kind and sets
 * `*attachment` to NULL (no pointer hydration is performed by this function).
 *
 * @param rmg Pointer to the RMG instance to query.
 * @param node_index Index of the node whose attachment is requested.
 * @param[out] attachment Receives the hydrated attachment pointer or NULL.
 * @param[out] kind Receives the attachment kind; set to `MG_ATT_NONE` when no attachment is present.
 * @returns `true` on successful query (including missing/out-of-range attachments), `false` if input pointers are NULL.
 */
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

/**
 * Retrieve hydration information for an edge attachment of an RMG.
 *
 * @param rmg RMG instance to query.
 * @param edge_index Index of the edge whose attachment to hydrate.
 * @param attachment Output pointer set to the hydrated attachment pointer, or NULL if no attachment exists or the index is out of range.
 * @returns `true` on successful query (including when no attachment is present), `false` if input pointers are NULL.
 */
bool mg_rmg_hydrate_edge_att(const mg_rmg_t *rmg, uint32_t edge_index,
                             const void **attachment) {
    if (!rmg || !attachment) {
        return false;
    }
    if (!rmg->edge_att || edge_index >= rmg->skel->edge_count) {
        *attachment = NULL;
        return true;
    }
    const mg_attach_ref_t *ref = &rmg->edge_att[edge_index];
    *attachment = NULL;
    (void)ref;
    return true;
}