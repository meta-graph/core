#include "metagraph/rule.h"
#include "metagraph/base.h"

#include <stdint.h>
#include <string.h>

void mg_rule_make_apply_x(mg_rule_t *rule, uint32_t rule_id) {
    memset(rule, 0, sizeof(*rule));
    rule->rule_id = rule_id;
    rule->L.node_count = 1;
    rule->L.node_type[0] = MG_TYPE_Q;
    rule->R = rule->L;
    rule->K_node_mask = 0x1U;
    rule->K_edge_mask = 0;
    rule->K2L_node[0] = 0;
    rule->K2R_node[0] = 0;
    rule->kernel = MG_KERNEL_X;
    rule->kernel_radius = 0;
    rule->L_boundary_mask = 0;
}

void mg_rule_make_cnot_qwq(mg_rule_t *rule, uint32_t rule_id) {
    memset(rule, 0, sizeof(*rule));
    rule->rule_id = rule_id;
    rule->L.node_count = 2;
    rule->L.node_type[0] = MG_TYPE_Q;
    rule->L.node_type[1] = MG_TYPE_Q;
    rule->L.edge_count = 1;
    rule->L.edge_u[0] = 0;
    rule->L.edge_v[0] = 1;
    rule->R = rule->L;
    rule->K_node_mask = 0x3U;
    rule->K_edge_mask = 0x1U;
    rule->K2L_node[0] = 0;
    rule->K2R_node[0] = 0;
    rule->K2L_node[1] = 1;
    rule->K2R_node[1] = 1;
    rule->K2L_edge[0] = 0;
    rule->K2R_edge[0] = 0;
    rule->kernel = MG_KERNEL_CNOT;
    rule->kernel_radius = 1;
    rule->L_boundary_mask = 0;
}

void mg_rule_make_split_w(mg_rule_t *rule, uint32_t rule_id) {
    memset(rule, 0, sizeof(*rule));
    rule->rule_id = rule_id;
    rule->L.node_count = 2;
    const mg_type_id_t l_types[] = {MG_TYPE_Q, MG_TYPE_Q};
    memcpy(rule->L.node_type, l_types, sizeof(l_types));
    rule->L.edge_count = 1;
    rule->L.edge_u[0] = 0;
    rule->L.edge_v[0] = 1;

    rule->R.node_count = 3;
    const mg_type_id_t r_types[] = {MG_TYPE_Q, MG_TYPE_Q, MG_TYPE_Q};
    memcpy(rule->R.node_type, r_types, sizeof(r_types));
    rule->R.edge_count = 2;
    const uint8_t r_edge_u[] = {0, 2};
    const uint8_t r_edge_v[] = {2, 1};
    memcpy(rule->R.edge_u, r_edge_u, sizeof(r_edge_u));
    memcpy(rule->R.edge_v, r_edge_v, sizeof(r_edge_v));

    rule->K_node_mask = 0x3U;
    rule->K_edge_mask = 0;
    rule->K2L_node[0] = 0;
    rule->K2R_node[0] = 0;
    rule->K2L_node[1] = 1;
    rule->K2R_node[1] = 1;
    rule->kernel = MG_KERNEL_ISOM_SPLIT;
    rule->kernel_radius = 1;
    rule->L_boundary_mask = 0;
}
