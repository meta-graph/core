#ifndef METAGRAPH_RULE_H
#define METAGRAPH_RULE_H

#include <stdint.h>
#include <string.h>

#include "metagraph/base.h"
#include "metagraph/rmg.h"

enum { MG_TYPE_Q = 1, MG_TYPE_W = 2 };

typedef struct {
    uint8_t node_count;
    mg_type_id_t node_type[16];
    uint8_t edge_count;
    uint8_t edge_u[24];
    uint8_t edge_v[24];
} mg_pattern_t;

typedef enum {
    MG_KERNEL_X = 1,
    MG_KERNEL_CNOT = 10,
    MG_KERNEL_ISOM_SPLIT = 20
} mg_kernel_id_t;

typedef struct {
    uint16_t min_in;
    uint16_t max_in;
    uint16_t min_out;
    uint16_t max_out;
} mg_rule_port_cap_t;

typedef struct {
    mg_edge_ifc_t edge_ifc;
    uint8_t l_edge_index;
} mg_rule_edge_iface_t;

typedef struct {
    uint16_t in_count;
    uint16_t out_count;
    const mg_type_id_t *in_types;
    const mg_type_id_t *out_types;
    uint8_t in_nodes[16];
    uint8_t out_nodes[16];
} mg_iface_stub_t;

typedef struct {
    uint32_t rule_id;
    mg_pattern_t L;
    mg_pattern_t R;
    uint16_t K_node_mask;
    uint32_t K_edge_mask;
    uint8_t K2L_node[16];
    uint8_t K2R_node[16];
    uint8_t K2L_edge[24];
    uint8_t K2R_edge[24];
    mg_iface_stub_t in_iface;
    mg_iface_stub_t out_iface;
    uint16_t L_boundary_mask;
    mg_rule_port_cap_t L_port_caps[16];
    mg_rule_edge_iface_t preserved_edge_ifc[24];
    mg_kernel_id_t kernel;
    uint16_t kernel_radius;
    uint32_t flags;
} mg_rule_t;

void mg_rule_make_apply_x(mg_rule_t *rule, uint32_t rule_id);
void mg_rule_make_cnot_qwq(mg_rule_t *rule, uint32_t rule_id);
void mg_rule_make_split_w(mg_rule_t *rule, uint32_t rule_id);

#endif /* METAGRAPH_RULE_H */
