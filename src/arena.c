#include "metagraph/arena.h"

#include <limits.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

static size_t metagraph_align_up(size_t value, size_t alignment) {
    if (alignment == 0) {
        return value;
    }
    const size_t mask = alignment - 1;
    if (value > SIZE_MAX - mask) {
        return SIZE_MAX;
    }
    return (value + mask) & ~mask;
}

void mg_arena_init(mg_arena_t *arena, void *buffer, size_t capacity) {
    if (!arena) {
        return;
    }
    arena->base = (uint8_t *)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

void mg_arena_reset(mg_arena_t *arena) {
    if (!arena) {
        return;
    }
    arena->offset = 0;
}

void *mg_arena_alloc(mg_arena_t *arena, size_t size) {
    if (!arena || !arena->base) {
        return NULL;
    }
    const size_t align = alignof(max_align_t);
    size_t offset = metagraph_align_up(arena->offset, align);
    if (size == 0) {
        return arena->base + (offset < arena->capacity ? offset : 0U);
    }
    if (offset > arena->capacity || size > arena->capacity - offset) {
        return NULL;
    }
    void *ptr = arena->base + offset;
    arena->offset = offset + size;
    return ptr;
}
