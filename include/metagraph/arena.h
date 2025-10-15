#ifndef METAGRAPH_ARENA_H
#define METAGRAPH_ARENA_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/result.h"

typedef struct {
    uint8_t *base;
    size_t capacity;
    size_t offset;
} mg_arena_t;

void mg_arena_init(mg_arena_t *arena, void *buffer, size_t capacity);
void mg_arena_reset(mg_arena_t *arena);
void *mg_arena_alloc(mg_arena_t *arena, size_t size);

#endif /* METAGRAPH_ARENA_H */
