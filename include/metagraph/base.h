#ifndef METAGRAPH_BASE_H
#define METAGRAPH_BASE_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/result.h"

typedef uint64_t mg_node_id_t;
typedef uint64_t mg_edge_id_t;
typedef uint32_t mg_type_id_t;

typedef mg_node_id_t NodeId;
typedef mg_edge_id_t EdgeId;
typedef mg_type_id_t TypeId;

static inline void mg_zero_buffer(void *ptr, size_t size) {
    if (!ptr || size == 0U) {
        return;
    }
    unsigned char *bytes = (unsigned char *)ptr;
    for (size_t index = 0; index < size; ++index) {
        bytes[index] = 0U;
    }
}

#endif /* METAGRAPH_BASE_H */
