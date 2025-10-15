#ifndef METAGRAPH_RMG_H
#define METAGRAPH_RMG_H

#include "metagraph/base.h"
#include "metagraph/graph.h"

typedef enum {
    MG_ATT_NONE = 0,
    MG_ATT_ATOM = 1,
    MG_ATT_GRAPH = 2
} mg_att_kind_t;

typedef struct {
    mg_att_kind_t kind;
    uint64_t offset;
    uint32_t flags;
} mg_attach_ref_t;

typedef struct {
    uint16_t in_count;
    uint16_t out_count;
    const mg_type_id_t *in_types;
    const mg_type_id_t *out_types;
} mg_iface_t;

typedef struct {
    mg_graph_t *skel;
    mg_attach_ref_t *node_att;
    mg_attach_ref_t *edge_att;
    mg_iface_t *edge_ifc;
} mg_rmg_t;

bool mg_rmg_hydrate_node_att(const mg_rmg_t *rmg, uint32_t node_index,
                             const void **attachment, mg_att_kind_t *kind);

bool mg_rmg_hydrate_edge_att(const mg_rmg_t *rmg, uint32_t edge_index,
                             const void **attachment);

#endif /* METAGRAPH_RMG_H */
