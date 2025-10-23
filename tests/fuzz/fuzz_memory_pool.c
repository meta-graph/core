#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"

static int fuzz_memory_pool_entry(const uint8_t *data, size_t size) {
    mg_fuzz_drive_arena(data, size, 128U);

    const uint8_t selector =
        (size > 0U) ? (uint8_t)(data[size - 1U] ^ 0xAAU) : 0U;
    mg_fuzz_drive_graph(data, size, selector);

    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    return fuzz_memory_pool_entry(data, size);
}
