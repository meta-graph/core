#ifndef METAGRAPH_RULE_H
#define METAGRAPH_RULE_H

#include <stdint.h>

#include "metagraph/base.h"
#include "metagraph/rmg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MG_RULE_MAX_NODES 16U
#define MG_RULE_MAX_EDGES 24U

/**
 * Built-in MetaGraph type identifiers used by helper rules.
 */
typedef enum { MG_TYPE_Q = 1, MG_TYPE_W = 2 } mg_builtin_type_id_t;

/**
 * Compact pattern graph describing the L/K/R legs of a rule.
 * Node and edge arrays are capped at MG_RULE_MAX_* to keep structures POD.
 */
typedef struct {
    uint8_t node_count; /**< Number of nodes in the pattern (<=
                           MG_RULE_MAX_NODES). */
    mg_type_id_t
        node_type[MG_RULE_MAX_NODES]; /**< Node types indexed by node ID. */
    uint8_t edge_count;               /**< Number of edges in the pattern (<=
                                         MG_RULE_MAX_EDGES). */
    mg_node_id_t
        edge_u[MG_RULE_MAX_EDGES]; /**< Edge source endpoints (node indices). */
    mg_node_id_t edge_v[MG_RULE_MAX_EDGES]; /**< Edge destination endpoints
                                               (node indices). */
} mg_pattern_t;

typedef enum {
    MG_KERNEL_X = 1,
    MG_KERNEL_CNOT = 10,
    MG_KERNEL_ISOM_SPLIT = 20
} mg_kernel_id_t;

/**
 * Node port capacity constraints enforced during matching.
 */
typedef struct {
    uint16_t min_in;
    uint16_t max_in;
    uint16_t min_out;
    uint16_t max_out;
} mg_rule_port_cap_t;

/**
 * Preserved edge interface data for edges that remain during rewrites.
 */
typedef struct {
    mg_edge_ifc_t edge_ifc;
    uint8_t l_edge_index;
} mg_rule_edge_iface_t;

/**
 * Interface stub describing preserved ports for a rule.
 * Arrays are fixed-size to avoid heap management in headers.
 */
typedef struct {
    uint16_t in_count;
    uint16_t out_count;
    mg_type_id_t in_types[MG_RULE_MAX_NODES];
    mg_type_id_t out_types[MG_RULE_MAX_NODES];
    uint8_t in_nodes[MG_RULE_MAX_NODES];
    uint8_t out_nodes[MG_RULE_MAX_NODES];
} mg_iface_stub_t;

/**
 * Fully materialised rule used by the matcher and QCA runtime.
 * Arrays follow MG_RULE_MAX_* limits and are POD for easy copying.
 */
typedef struct {
    uint32_t rule_id;
    mg_pattern_t L;
    mg_pattern_t R;
    uint16_t K_node_mask;
    uint32_t K_edge_mask;
    uint8_t K2L_node[MG_RULE_MAX_NODES];
    uint8_t K2R_node[MG_RULE_MAX_NODES];
    uint8_t K2L_edge[MG_RULE_MAX_EDGES];
    uint8_t K2R_edge[MG_RULE_MAX_EDGES];
    mg_iface_stub_t in_iface;
    mg_iface_stub_t out_iface;
    uint16_t L_boundary_mask;
    mg_rule_port_cap_t L_port_caps[MG_RULE_MAX_NODES];
    mg_rule_edge_iface_t preserved_edge_ifc[MG_RULE_MAX_EDGES];
    mg_kernel_id_t kernel;
    uint16_t kernel_radius;
    uint32_t flags;
} mg_rule_t;

void mg_rule_make_apply_x(mg_rule_t *rule, uint32_t rule_id);
void mg_rule_make_cnot_qwq(mg_rule_t *rule, uint32_t rule_id);
void mg_rule_make_split_w(mg_rule_t *rule, uint32_t rule_id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* METAGRAPH_RULE_H */
