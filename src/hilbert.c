#include "metagraph/hilbert.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "metagraph/result.h"

metagraph_result_t mg_hilbert_init(mg_hilbert_t *hilbert, size_t count) {
    if (!hilbert) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_NULL_POINTER,
                             "hilbert handle is null");
    }
    hilbert->node_bits = (uint8_t *)calloc(count ? count : 1, sizeof(uint8_t));
    if (!hilbert->node_bits) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "failed to allocate hilbert register");
    }
    hilbert->node_count = count;
    return METAGRAPH_OK();
}

void mg_hilbert_free(mg_hilbert_t *hilbert) {
    if (!hilbert) {
        return;
    }
    free(hilbert->node_bits);
    hilbert->node_bits = NULL;
    hilbert->node_count = 0;
}

metagraph_result_t mg_hilbert_resize(mg_hilbert_t *hilbert, size_t new_count) {
    if (!hilbert) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_NULL_POINTER,
                             "hilbert handle is null");
    }
    if (hilbert->node_count == new_count) {
        return METAGRAPH_OK();
    }
    uint8_t *next =
        (uint8_t *)calloc(new_count ? new_count : 1, sizeof(uint8_t));
    if (!next) {
        return METAGRAPH_ERR(METAGRAPH_ERROR_OUT_OF_MEMORY,
                             "failed to grow hilbert register");
    }
    size_t copy =
        (hilbert->node_count < new_count) ? hilbert->node_count : new_count;
    if (hilbert->node_bits && copy > 0) {
        memcpy(next, hilbert->node_bits, copy);
    }
    free(hilbert->node_bits);
    hilbert->node_bits = next;
    hilbert->node_count = new_count;
    return METAGRAPH_OK();
}
