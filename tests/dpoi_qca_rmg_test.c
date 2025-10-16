#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "metagraph/dpoi.h"
#include "metagraph/graph.h"
#include "metagraph/hilbert.h"
#include "metagraph/match.h"
#include "metagraph/qca.h"
#include "metagraph/result.h"
#include "metagraph/rmg.h"
#include "metagraph/rule.h"

static void init_rmg(mg_graph_t *graph, mg_rmg_t *rmg,
                     mg_attach_ref_t *node_att, mg_attach_ref_t *edge_att,
                     mg_edge_ifc_t *edge_ifc) {
    graph->edge_count = 0U;
    rmg->skel = graph;
    rmg->node_att = node_att;
    rmg->edge_att = edge_att;
    rmg->edge_ifc = edge_ifc;
    rmg->skel_epoch = NULL;
    rmg->att_epoch = NULL;

    for (size_t i = 0; i < graph->node_count; ++i) {
        node_att[i].kind = MG_ATT_NONE;
        node_att[i].offset = 0;
        node_att[i].flags = 0;
    }
    if (graph->edge_count > 0U && edge_att && edge_ifc) {
        for (size_t i = 0; i < graph->edge_count; ++i) {
            edge_att[i].kind = MG_ATT_NONE;
            edge_att[i].offset = 0;
            edge_att[i].flags = 0;
            edge_ifc[i].src.port_count = 0;
            edge_ifc[i].src.ports = NULL;
            edge_ifc[i].dst.port_count = 0;
            edge_ifc[i].dst.ports = NULL;
        }
    }
}

static void test_dpoi_apply_x(void) {
    mg_graph_t graph;
    mg_graph_init_empty(&graph);
    mg_graph_make_path_qwqwq(&graph);

    mg_attach_ref_t node_att[4];
    mg_attach_ref_t edge_att[1];
    mg_edge_ifc_t edge_ifc[1];
    mg_rmg_t rmg;
    init_rmg(&graph, &rmg, node_att, edge_att, edge_ifc);
    assert(rmg.skel_epoch == NULL);
    assert(rmg.att_epoch == NULL);
    for (uint32_t i = 0; i < graph.edge_count; ++i) {
        assert(rmg.edge_ifc[i].src.port_count == 0);
        assert(rmg.edge_ifc[i].dst.port_count == 0);
    }

    mg_rule_t rule;
    mg_rule_make_apply_x(&rule, 1);
    assert(rule.L_port_caps[0].min_in == 0);
    assert(rule.L_port_caps[0].max_in == UINT16_MAX);
    assert(rule.L_port_caps[0].min_out == 0);
    assert(rule.L_port_caps[0].max_out == UINT16_MAX);

    mg_match_set_t matches;
    assert(mg_match_set_init(&matches, 8));

    metagraph_result_t res = mg_dpoi_match_rmg(&rmg, &rule, NULL, &matches);
    assert(!metagraph_result_is_error(res));
    assert(matches.count == 3);

    mg_match_set_free(&matches);
    mg_graph_free(&graph);
}

static void test_qca_tick_apply_x(void) {
    mg_graph_t graph;
    mg_graph_init_empty(&graph);
    mg_graph_make_path_qwqwq(&graph);

    mg_attach_ref_t node_att[4];
    mg_attach_ref_t edge_att[1];
    mg_edge_ifc_t edge_ifc[1];
    mg_rmg_t rmg;
    init_rmg(&graph, &rmg, node_att, edge_att, edge_ifc);

    mg_rule_t rule;
    mg_rule_make_apply_x(&rule, 1);

    mg_hilbert_t hilbert;
    assert(!metagraph_result_is_error(
        mg_hilbert_init(&hilbert, graph.node_count)));
    if (graph.node_count > 1U) {
        hilbert.node_bits[1] = 1U;
    }

    mg_tick_metrics_t metrics = {0};
    metagraph_result_t res =
        mg_qca_tick_rmg(&rmg, &hilbert, &rule, 1, NULL, NULL, &metrics);
    assert(!metagraph_result_is_error(res));
    assert(metrics.matches_found == 3);
    assert(metrics.matches_kept == 3);
    assert(metrics.conflicts_dropped == 0);

    mg_hilbert_free(&hilbert);
    mg_graph_free(&graph);
}

int main(void) {
    test_dpoi_apply_x();
    test_qca_tick_apply_x();
    return 0;
}
