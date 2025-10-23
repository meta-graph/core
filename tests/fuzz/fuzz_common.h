#ifndef METAGRAPH_FUZZ_COMMON_H
#define METAGRAPH_FUZZ_COMMON_H

#include "metagraph/arena.h"
#include "metagraph/graph.h"

#include <stddef.h>
#include <stdint.h>

static inline void mg_fuzz_drive_graph(const uint8_t *data, size_t size,
                                       uint8_t selector) {
    mg_graph_t graph;
    mg_graph_init_empty(&graph);

    if (((selector ^ (size ? data[0] : 0U)) & 0x1U) != 0U) {
        mg_graph_make_path_qwqwq(&graph);
    } else {
        mg_graph_make_path_qwqwq2(&graph);
    }

    if (graph.node_count > 0U) {
        uint32_t idx = 0U;
        if (size > 0U) {
            idx = (uint32_t)(data[0] % graph.node_count);
        }
        (void)mg_graph_degree(&graph, idx);
    } else {
        (void)mg_graph_degree(&graph, 0U);
    }

    (void)mg_graph_snapshot_view(&graph);
    mg_graph_free(&graph);
}

static inline void mg_fuzz_drive_arena(const uint8_t *data, size_t size,
                                       size_t max_request) {
    if (max_request == 0U) {
        max_request = 1U;
    }

    uint8_t buffer[256];
    mg_arena_t arena;
    mg_arena_init(&arena, buffer, sizeof buffer);

    size_t cursor = 0U;
    while (cursor < size) {
        uint8_t byte = data[cursor];
        size_t request = (size_t)(byte % max_request);
        (void)mg_arena_alloc(&arena, request);
        cursor += (byte == 0U) ? 1U : (size_t)byte;
    }

    /* Exercise the zero-size path as well. */
    (void)mg_arena_alloc(&arena, 0U);
    mg_arena_reset(&arena);
}

#endif /* METAGRAPH_FUZZ_COMMON_H */
