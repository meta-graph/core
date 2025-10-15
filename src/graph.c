#include "metagraph/graph.h"
#include "metagraph/epoch.h"
#include "metagraph/rule.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Initialize a graph structure to an empty state and initialize its epoch.
 *
 * This zeroes all fields of the provided mg_graph_t and initializes the
 * embedded epoch value via mg_epoch_init.
 *
 * @param graph Pointer to the mg_graph_t to initialize; must point to a valid,
 *              writable mg_graph_t.
 */
void mg_graph_init_empty(mg_graph_t *graph) {
    memset(graph, 0, sizeof(*graph));
    mg_epoch_init(&graph->epoch);
}

/**
 * Release all memory held by a graph and reset its contents to zero.
 *
 * Frees dynamically allocated arrays within the graph (nodes, edges, neighbor ids)
 * and clears the graph structure. Safe to call with a NULL pointer.
 *
 * @param graph Pointer to the graph to free and reset; may be NULL.
 */
void mg_graph_free(mg_graph_t *graph) {
    if (!graph) {
        return;
    }
    free(graph->nodes);
    free(graph->edges);
    free(graph->nbr_ids);
    memset(graph, 0, sizeof(*graph));
}

/**
 * Create a snapshot view of the provided graph suitable for read-only inspection.
 *
 * If `graph` is NULL the snapshot contains NULL pointers and zero counts. The snapshot's
 * pointer fields reference the graph's internal arrays; they are not deep copies and
 * remain valid only as long as the underlying graph's memory is not modified or freed.
 *
 * @param graph Graph to snapshot; may be NULL.
 * @returns mg_graph_snapshot_t Snapshot containing pointers to node and neighbor arrays, edge data, their counts, and the graph epoch value.
 */
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

/**
 * Compute the degree (number of neighbors) of the node at the given index.
 *
 * @param graph Graph to query.
 * @param node_index Index of the node within the graph's node array.
 * @returns `-1` if `graph` is NULL or `node_index` is out of bounds, otherwise the node's degree (number of neighbor entries).
 */
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

/**
 * Allocate and zero-initialize the graph's node array and record the node count.
 *
 * Allocates an array of `count` `mg_node_rec_t` elements with `calloc` and assigns it
 * to `graph->nodes`. Sets `graph->node_count` to `count`. If allocation fails,
 * `graph->nodes` will be `NULL` and `graph->node_count` will still be set to `count`.
 *
 * @param graph Graph object whose node storage will be allocated.
 * @param count Number of nodes to allocate.
 */
static void metagraph_graph_alloc_nodes(mg_graph_t *graph, size_t count) {
    graph->nodes = (mg_node_rec_t *)calloc(count, sizeof(mg_node_rec_t));
    graph->node_count = count;
}

/**
 * Allocate and zero-initialize the neighbor ID array for a graph and record its size.
 *
 * Allocates an array of `count` uint32_t elements with `calloc`, assigns it to
 * `graph->nbr_ids`, and sets `graph->nbr_count` to `count`.
 *
 * @param graph Graph whose neighbor storage will be allocated.
 * @param count Number of neighbor IDs to allocate.
 */
static void metagraph_graph_alloc_neighbors(mg_graph_t *graph, size_t count) {
    graph->nbr_ids = (uint32_t *)calloc(count, sizeof(uint32_t));
    graph->nbr_count = count;
}

/**
 * Initialize the provided graph as a three-node path (0–1–2) with each node set to `MG_TYPE_Q`.
 *
 * The function frees any existing contents of `graph` and replaces them with three nodes and their
 * adjacency lists representing the path 0—1—2.
 *
 * @param graph Graph to populate; existing contents are freed and replaced. Must be non-NULL.
 */
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

/**
 * Construct a small three-node graph with a predetermined adjacency layout.
 *
 * This resets and reinitializes `graph`, allocates space for 3 nodes and 6 neighbor entries,
 * and populates node ids, node types (MG_TYPE_Q), adjacency offsets, degrees, and the
 * neighbor id array with a fixed pattern used for example/testing purposes.
 *
 * @param graph Graph object to initialize and populate; existing contents are freed.
 */
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