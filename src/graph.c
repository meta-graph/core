#include "metagraph/graph.h"
#include "metagraph/epoch.h"
#include "metagraph/rule.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void mg_graph_init_empty(mg_graph_t *graph) {
    memset(graph, 0, sizeof(*graph));
    mg_epoch_init(&graph->epoch);
}

void mg_graph_free(mg_graph_t *graph) {
    if (!graph) {
        return;
    }
    free(graph->nodes);
    free(graph->edges);
    free(graph->nbr_ids);
    memset(graph, 0, sizeof(*graph));
}

mg_graph_snapshot_t mg_graph_snapshot_view(const mg_graph_t *graph) {
    mg_graph_snapshot_t snapshot = {
        .nodes = graph ? graph->nodes : NULL,
        .nbr_ids = graph ? graph->nbr_ids : NULL,
        .node_count = graph ? graph->node_count : 0,
        .nbr_count = graph ? graph->nbr_count : 0,
        .edges = graph ? graph->edges : NULL,
        .edge_count = graph ? graph->edge_count : 0,
        .epoch = graph ? mg_epoch_load(&graph->epoch) : 0};
    return snapshot;
}

int mg_graph_degree(const mg_graph_t *graph, uint32_t node_index) {
    if (!graph || node_index >= graph->node_count) {
        return -1;
    }
    const mg_node_rec_t *node = &graph->nodes[node_index];
    const size_t start = node->adj_offset;
    size_t end = graph->nbr_count;
    if (node_index + 1 < graph->node_count) {
        end = graph->nodes[node_index + 1].adj_offset;
    }
    return (int)(end - start);
}

static void metagraph_graph_alloc_nodes(mg_graph_t *graph, size_t count) {
    graph->nodes = (mg_node_rec_t *)calloc(count, sizeof(mg_node_rec_t));
    graph->node_count = count;
}

static void metagraph_graph_alloc_neighbors(mg_graph_t *graph, size_t count) {
    graph->nbr_ids = (uint32_t *)calloc(count, sizeof(uint32_t));
    graph->nbr_count = count;
}

void mg_graph_make_path_qwqwq(mg_graph_t *graph) {
    mg_graph_free(graph);
    mg_graph_init_empty(graph);

    metagraph_graph_alloc_nodes(graph, 3);
    metagraph_graph_alloc_neighbors(graph, 4);

    graph->nbr_ids[0] = 1;
    graph->nbr_ids[1] = 0;
    graph->nbr_ids[2] = 2;
    graph->nbr_ids[3] = 1;

    graph->nodes[0].id = 0;
    graph->nodes[0].type = MG_TYPE_Q;
    graph->nodes[0].adj_offset = 0;
    graph->nodes[0].degree = 1;

    graph->nodes[1].id = 1;
    graph->nodes[1].type = MG_TYPE_Q;
    graph->nodes[1].adj_offset = 2;
    graph->nodes[1].degree = 2;

    graph->nodes[2].id = 2;
    graph->nodes[2].type = MG_TYPE_Q;
    graph->nodes[2].adj_offset = 3;
    graph->nodes[2].degree = 1;
}

void mg_graph_make_path_qwqwq2(mg_graph_t *graph) {
    mg_graph_free(graph);
    mg_graph_init_empty(graph);

    metagraph_graph_alloc_nodes(graph, 3);
    metagraph_graph_alloc_neighbors(graph, 6);

    graph->nbr_ids[0] = 1;
    graph->nbr_ids[1] = 0;
    graph->nbr_ids[2] = 2;
    graph->nbr_ids[3] = 1;
    graph->nbr_ids[4] = 2;
    graph->nbr_ids[5] = 1;

    graph->nodes[0].id = 0;
    graph->nodes[0].type = MG_TYPE_Q;
    graph->nodes[0].adj_offset = 0;
    graph->nodes[0].degree = 1;

    graph->nodes[1].id = 1;
    graph->nodes[1].type = MG_TYPE_Q;
    graph->nodes[1].adj_offset = 2;
    graph->nodes[1].degree = 2;

    graph->nodes[2].id = 2;
    graph->nodes[2].type = MG_TYPE_Q;
    graph->nodes[2].adj_offset = 4;
    graph->nodes[2].degree = 1;
}
