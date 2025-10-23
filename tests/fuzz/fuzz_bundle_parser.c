#include <stddef.h>
#include <stdint.h>

#include "fuzz_common.h"

static int fuzz_bundle_parser_entry(const uint8_t *data, size_t size) {
    const uint8_t selector = (size > 0U) ? data[0] : 0U;
    mg_fuzz_drive_graph(data, size, (uint8_t)(selector ^ 0x3U));
    mg_fuzz_drive_arena(data, size, 64U);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    return fuzz_bundle_parser_entry(data, size);
}
