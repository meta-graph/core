#ifndef METAGRAPH_GRAPH_H
#define METAGRAPH_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/base.h"
#include "metagraph/epoch.h"

typedef struct {
    mg_node_id_t id;
    mg_type_id_t type;
    uint32_t degree;
    uint32_t adj_offset;
} mg_node_rec_t;

typedef struct {
    mg_edge_id_t id;
    mg_type_id_t type;
    mg_node_id_t src;
    mg_node_id_t dst;
} mg_edge_rec_t;

typedef struct {
    mg_node_rec_t *nodes;
    size_t node_count;
    uint32_t
        *nbr_ids; /* CSR neighbour list storing node indices into nodes[] */
    size_t nbr_count;
    mg_edge_rec_t *edges;
    size_t edge_count;
    mg_epoch_t epoch;
} mg_graph_t;

typedef struct {
    const mg_node_rec_t *nodes;
    const uint32_t *nbr_ids;
    size_t node_count;
    size_t nbr_count;
    const mg_edge_rec_t *edges;
    size_t edge_count;
    uint64_t epoch;
} mg_graph_snapshot_t;

void mg_graph_init_empty(mg_graph_t *graph);
void mg_graph_free(mg_graph_t *graph);
mg_graph_snapshot_t mg_graph_snapshot_view(const mg_graph_t *graph);

int mg_graph_degree(const mg_graph_t *graph, uint32_t node_index);

void mg_graph_make_path_qwqwq(mg_graph_t *graph);
void mg_graph_make_path_qwqwq2(mg_graph_t *graph);

#endif /* METAGRAPH_GRAPH_H */
