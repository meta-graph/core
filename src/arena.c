#include "metagraph/arena.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

static size_t metagraph_align_up(size_t value, size_t alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

void mg_arena_init(mg_arena_t *arena, void *buffer, size_t capacity) {
    arena->base = (uint8_t *)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

void mg_arena_reset(mg_arena_t *arena) { arena->offset = 0; }

void *mg_arena_alloc(mg_arena_t *arena, size_t size) {
    const size_t align = alignof(max_align_t);
    size_t offset = metagraph_align_up(arena->offset, align);
    if (offset + size > arena->capacity) {
        return NULL;
    }
    void *ptr = arena->base + offset;
    arena->offset = offset + size;
    return ptr;
}
