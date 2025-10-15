#include "metagraph/rule.h"
#include "metagraph/base.h"

#include <stdint.h>
#include <string.h>

/**
 * Initialize a rule that applies a single-qubit X kernel.
 *
 * Configure `rule` as a one-node L graph (type MG_TYPE_Q) with R identical to L;
 * set K_node_mask to 0x1, K_edge_mask to 0, K2L_node/K2R_node mapping for the node
 * to 0, kernel to MG_KERNEL_X with kernel_radius 0, L_boundary_mask to 0,
 * and assign the provided rule_id.
 *
 * @param rule Pointer to the mg_rule_t to initialize.
 * @param rule_id Identifier to assign to the initialized rule.
 */
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

/**
 * Initialize a rule representing a two-qubit CNOT pattern (Q–Q) where the right-hand side is identical to the left-hand side.
 *
 * Sets the rule to a default/empty state, assigns the provided rule_id, configures L as two qubit nodes connected by a single edge, copies L to R, sets K_node_mask to 0x3, K_edge_mask to 0x1, establishes K2L/K2R node and edge mappings for the kernel, sets kernel to MG_KERNEL_CNOT with kernel_radius 1, and sets L_boundary_mask to 0.
 *
 * @param rule Pointer to the mg_rule_t to initialize.
 * @param rule_id Identifier to assign to the rule.
 */
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

/**
 * Initialize `rule` as the "split" (W) rewrite rule where a 2-qubit L graph
 * is replaced by a 3-qubit R graph.
 *
 * The initialized rule will have L configured with 2 qubit nodes and 1 edge,
 * R configured with 3 qubit nodes and 2 edges (edges: 0-2 and 2-1), K_node_mask
 * set to 0x3, K_edge_mask set to 0, K2L/K2R node mappings for nodes 0->0 and
 * 1->1, kernel set to MG_KERNEL_ISOM_SPLIT with kernel_radius 1, L_boundary_mask
 * set to 0, and rule_id assigned.
 *
 * @param rule Pointer to the mg_rule_t to initialize (output).
 * @param rule_id Identifier to assign to the rule.
 */
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