#ifndef METAGRAPH_RMG_H
#define METAGRAPH_RMG_H

#include "metagraph/base.h"
#include "metagraph/epoch.h"
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
    mg_type_id_t type;
    uint32_t flags;
} mg_port_sig_t;

typedef enum {
    MG_PORT_DIR_UNSPECIFIED = 0,
    MG_PORT_DIR_INBOUND = 1,
    MG_PORT_DIR_OUTBOUND = 2,
    MG_PORT_DIR_BIDIRECTIONAL = 3
} mg_port_dir_t;

typedef struct {
    mg_port_dir_t direction;
    mg_port_sig_t signature;
} mg_iface_port_t;

typedef struct {
    const mg_iface_port_t *ports;
    uint16_t port_count;
} mg_iface_sig_t;

typedef struct {
    mg_iface_sig_t src;
    mg_iface_sig_t dst;
} mg_edge_ifc_t;

typedef enum {
    MG_ATT_UPDATE_NODE = 0,
    MG_ATT_UPDATE_EDGE = 1
} mg_att_update_kind_t;

typedef struct {
    mg_att_update_kind_t kind;
    uint32_t index;
    mg_attach_ref_t before;
    mg_attach_ref_t after;
} metagraph_att_update_t;

typedef struct {
    mg_graph_t *skel;
    mg_attach_ref_t *node_att;
    mg_attach_ref_t *edge_att;
    mg_edge_ifc_t *edge_ifc;
    mg_epoch_t *skel_epoch;
    mg_attachment_epoch_t *att_epoch;
} mg_rmg_t;

bool mg_rmg_hydrate_node_att(const mg_rmg_t *rmg, uint32_t node_index,
                             const void **attachment, mg_att_kind_t *kind);

bool mg_rmg_hydrate_edge_att(const mg_rmg_t *rmg, uint32_t edge_index,
                             const void **attachment);

#endif /* METAGRAPH_RMG_H */
