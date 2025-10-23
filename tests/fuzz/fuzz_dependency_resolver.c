#include <stddef.h>
#include <stdint.h>

#include "metagraph/graph.h"

#include "fuzz_common.h"

static void mg_fuzz_resolve_edges(const uint8_t *data, size_t size) {
    mg_graph_t graph;
    mg_graph_init_empty(&graph);

    if ((size > 1U ? data[1] : 0U) & 0x1U) {
        mg_graph_make_path_qwqwq(&graph);
    } else {
        mg_graph_make_path_qwqwq2(&graph);
    }

    const mg_graph_snapshot_t snapshot = mg_graph_snapshot_view(&graph);
    for (size_t i = 0U; i < snapshot.node_count; ++i) {
        (void)mg_graph_degree(&graph, (uint32_t)i);
    }

    mg_graph_free(&graph);
}

static int fuzz_dependency_resolver_entry(const uint8_t *data, size_t size) {
    mg_fuzz_resolve_edges(data, size);
    mg_fuzz_drive_arena(data, size, 80U);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    return fuzz_dependency_resolver_entry(data, size);
}
