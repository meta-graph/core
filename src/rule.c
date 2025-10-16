#include "metagraph/rule.h"
#include "metagraph/base.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

static void mg_rule_init_port_caps(mg_rule_t *rule) {
    for (uint32_t i = 0; i < MG_RULE_MAX_NODES; ++i) {
        rule->L_port_caps[i].max_in = UINT16_MAX;
        rule->L_port_caps[i].max_out = UINT16_MAX;
    }
}

void mg_rule_make_apply_x(mg_rule_t *rule, uint32_t rule_id) {
    memset(rule, 0, sizeof(*rule));
    mg_rule_init_port_caps(rule);
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
    mg_rule_init_port_caps(rule);
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
    mg_rule_init_port_caps(rule);
    rule->rule_id = rule_id;
    rule->L.node_count = 2;
    const mg_type_id_t l_types[] = {MG_TYPE_Q, MG_TYPE_Q};
    memcpy(rule->L.node_type, l_types,
           rule->L.node_count * sizeof(rule->L.node_type[0]));
    rule->L.edge_count = 1;
    rule->L.edge_u[0] = 0;
    rule->L.edge_v[0] = 1;

    rule->R.node_count = 3;
    const mg_type_id_t r_types[] = {MG_TYPE_Q, MG_TYPE_Q, MG_TYPE_Q};
    memcpy(rule->R.node_type, r_types,
           rule->R.node_count * sizeof(rule->R.node_type[0]));
    rule->R.edge_count = 2;
    const uint8_t r_edge_u[] = {0U, 2U};
    const uint8_t r_edge_v[] = {2U, 1U};
    memcpy(rule->R.edge_u, r_edge_u,
           rule->R.edge_count * sizeof(rule->R.edge_u[0]));
    memcpy(rule->R.edge_v, r_edge_v,
           rule->R.edge_count * sizeof(rule->R.edge_v[0]));

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
