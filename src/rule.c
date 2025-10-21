#include "metagraph/rule.h"
#include "metagraph/base.h"

#include <limits.h>
#include <stdint.h>

static void mg_rule_init_port_caps(mg_rule_t *rule) {
    for (uint32_t i = 0; i < MG_RULE_MAX_NODES; ++i) {
        rule->L_port_caps[i].max_in = UINT16_MAX;
        rule->L_port_caps[i].max_out = UINT16_MAX;
    }
}

/**
 * Initialize a rule that applies a single-qubit X kernel.
 *
 * Configure `rule` as a one-node L graph (type MG_TYPE_Q) with R identical to
 * L; set K_node_mask to 0x1, K_edge_mask to 0, K2L_node/K2R_node mapping for
 * the node to 0, kernel to MG_KERNEL_X with kernel_radius 0, L_boundary_mask to
 * 0, and assign the provided rule_id.
 *
 * @param rule Pointer to the mg_rule_t to initialize.
 * @param rule_id Identifier to assign to the initialized rule.
 */
void mg_rule_make_apply_x(mg_rule_t *rule, uint32_t rule_id) {
    mg_zero_buffer(rule, sizeof(*rule));
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

/**
 * Initialize a rule representing a two-qubit CNOT pattern (Q–Q) where the
 * right-hand side is identical to the left-hand side.
 *
 * Sets the rule to a default/empty state, assigns the provided rule_id,
 * configures L as two qubit nodes connected by a single edge, copies L to R,
 * sets K_node_mask to 0x3, K_edge_mask to 0x1, establishes K2L/K2R node and
 * edge mappings for the kernel, sets kernel to MG_KERNEL_CNOT with
 * kernel_radius 1, and sets L_boundary_mask to 0.
 *
 * @param rule Pointer to the mg_rule_t to initialize.
 * @param rule_id Identifier to assign to the rule.
 */
void mg_rule_make_cnot_qwq(mg_rule_t *rule, uint32_t rule_id) {
    mg_zero_buffer(rule, sizeof(*rule));
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

/**
 * Initialize `rule` as the "split" (W) rewrite rule where a 2-qubit L graph
 * is replaced by a 3-qubit R graph.
 *
 * The initialized rule will have L configured with 2 qubit nodes and 1 edge,
 * R configured with 3 qubit nodes and 2 edges (edges: 0-2 and 2-1), K_node_mask
 * set to 0x3, K_edge_mask set to 0, K2L/K2R node mappings for nodes 0->0 and
 * 1->1, kernel set to MG_KERNEL_ISOM_SPLIT with kernel_radius 1,
 * L_boundary_mask set to 0, and rule_id assigned.
 *
 * @param rule Pointer to the mg_rule_t to initialize (output).
 * @param rule_id Identifier to assign to the rule.
 */
void mg_rule_make_split_w(mg_rule_t *rule, uint32_t rule_id) {
    mg_zero_buffer(rule, sizeof(*rule));
    mg_rule_init_port_caps(rule);
    rule->rule_id = rule_id;
    rule->L.node_count = 2;
    const mg_type_id_t l_types[] = {MG_TYPE_Q, MG_TYPE_Q};
    for (uint32_t i = 0; i < rule->L.node_count; ++i) {
        rule->L.node_type[i] = l_types[i];
    }
    rule->L.edge_count = 1;
    rule->L.edge_u[0] = 0;
    rule->L.edge_v[0] = 1;

    rule->R.node_count = 3;
    const mg_type_id_t r_types[] = {MG_TYPE_Q, MG_TYPE_Q, MG_TYPE_Q};
    for (uint32_t i = 0; i < rule->R.node_count; ++i) {
        rule->R.node_type[i] = r_types[i];
    }
    rule->R.edge_count = 2;
    const mg_node_id_t r_edge_u[] = {0U, 2U};
    const mg_node_id_t r_edge_v[] = {2U, 1U};
    for (uint32_t i = 0; i < rule->R.edge_count; ++i) {
        rule->R.edge_u[i] = r_edge_u[i];
        rule->R.edge_v[i] = r_edge_v[i];
    }

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