#ifndef METAGRAPH_HILBERT_H
#define METAGRAPH_HILBERT_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/result.h"

typedef struct {
    uint8_t *node_bits;
    size_t node_count;
} mg_hilbert_t;

metagraph_result_t mg_hilbert_init(mg_hilbert_t *hilbert, size_t count);
void mg_hilbert_free(mg_hilbert_t *hilbert);
metagraph_result_t mg_hilbert_resize(mg_hilbert_t *hilbert, size_t new_count);

#endif /* METAGRAPH_HILBERT_H */
