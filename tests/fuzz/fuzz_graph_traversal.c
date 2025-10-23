#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"

static int fuzz_graph_traversal_entry(const uint8_t *data, size_t size) {
    uint8_t selector = 0U;
    for (size_t i = 0U; i < size && i < 8U; ++i) {
        selector ^= data[i];
        mg_fuzz_drive_graph(data + i, size - i, selector);
    }

    mg_fuzz_drive_arena(data, size, 48U);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    return fuzz_graph_traversal_entry(data, size);
}
