#ifndef METAGRAPH_ARENA_H
#define METAGRAPH_ARENA_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/result.h"

/**
 * @file arena.h
 * @brief Linear bump allocator for temporary allocations.
 *
 * The arena owns a caller-supplied buffer and hands out aligned allocations by
 * bumping an offset. All allocations can be discarded at once by calling
 * mg_arena_reset(). The implementation is single-threaded and does not take
 * ownership of the underlying buffer.
 */
typedef struct {
    uint8_t *base;   /**< Backing buffer (caller owned, writable). */
    size_t capacity; /**< Total buffer capacity in bytes. */
    size_t offset;   /**< Current bump pointer offset. */
} mg_arena_t;

/**
 * @brief Initialise an arena with a writable buffer.
 * @param arena Arena state to initialise (must be non-NULL).
 * @param buffer Backing buffer (must be non-NULL, writable, caller-owned).
 * @param capacity Size of the backing buffer in bytes.
 */
void mg_arena_init(mg_arena_t *arena, void *buffer, size_t capacity);

/**
 * @brief Reset the arena, invalidating all outstanding allocations.
 * @param arena Arena to reset (must be non-NULL).
 */
void mg_arena_reset(mg_arena_t *arena);

/**
 * @brief Allocate memory from the arena.
 * @param arena Arena to allocate from (must be non-NULL).
 * @param size Allocation size in bytes (may be zero).
 * @return Pointer to the allocated region, or NULL on failure.
 *
 * Returned allocations are aligned to alignof(max_align_t). A zero-size
 * allocation succeeds and returns a stable pointer while leaving the arena
 * offset unchanged.
 */
void *mg_arena_alloc(mg_arena_t *arena, size_t size);

#endif /* METAGRAPH_ARENA_H */
