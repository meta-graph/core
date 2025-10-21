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

static inline size_t mg_copy_bytes(void *dst, size_t dst_size, const void *src,
                                   size_t src_size, size_t count) {
    if (!dst || !src || dst_size == 0U || src_size == 0U || count == 0U) {
        return 0U;
    }
    unsigned char *target = (unsigned char *)dst;
    const unsigned char *source = (const unsigned char *)src;
    size_t limit = count;
    if (limit > dst_size) {
        limit = dst_size;
    }
    if (limit > src_size) {
        limit = src_size;
    }
    for (size_t index = 0; index < limit; ++index) {
        target[index] = source[index];
    }
    return limit;
}

#endif /* METAGRAPH_BASE_H */
