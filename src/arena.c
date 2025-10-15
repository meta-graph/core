#include "metagraph/arena.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Round `value` up to the nearest multiple of `alignment`.
 *
 * @param value The input value to align.
 * @param alignment The alignment boundary to round up to; must be a power of two.
 * @returns The smallest value greater than or equal to `value` that is a multiple of `alignment`.
 */
static size_t metagraph_align_up(size_t value, size_t alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

/**
 * Initialize an arena to use a caller-provided memory buffer.
 *
 * Sets the arena's base pointer to the start of `buffer`, records `capacity`
 * in bytes, and resets the allocation offset to zero.
 *
 * @param arena Pointer to the arena object to initialize.
 * @param buffer Caller-managed memory region that the arena will allocate from.
 * @param capacity Size of `buffer` in bytes.
 */
void mg_arena_init(mg_arena_t *arena, void *buffer, size_t capacity) {
    arena->base = (uint8_t *)buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

/**
 * Reset the arena to its initial empty state.
 *
 * Sets the allocation offset to zero so subsequent allocations reuse the arena's buffer.
 *
 * @param arena Arena to reset.
 */
void mg_arena_reset(mg_arena_t *arena) { arena->offset = 0; }

/**
 * Allocate a block from an arena aligned to the platform's maximum alignment.
 *
 * Allocates `size` bytes from `arena`, advancing the arena's internal offset
 * by the allocated size. The returned pointer is aligned to `alignof(max_align_t)`.
 *
 * @param arena Arena to allocate from.
 * @param size Number of bytes to allocate.
 * @returns Pointer to the allocated memory if sufficient space remains, `NULL` otherwise.
 */
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