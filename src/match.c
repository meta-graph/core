#include "metagraph/match.h"
#include "metagraph/base.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Initialize an mg_match_set_t structure and optionally allocate its internal
 * storage.
 *
 * The function zeroes the provided structure and, if capacity > 0, allocates an
 * array of mg_match_t of the requested size and sets the structure's capacity
 * accordingly.
 *
 * @param set Pointer to the mg_match_set_t to initialize (must not be NULL).
 * @param capacity Initial number of elements to allocate; if zero no allocation
 * is performed.
 * @returns `true` if the structure was initialized and any requested allocation
 * succeeded, `false` otherwise.
 */
bool mg_match_set_init(mg_match_set_t *set, uint32_t capacity) {
    if (!set) {
        return false;
    }
    mg_zero_buffer(set, sizeof(*set));
    if (capacity > 0) {
        set->data = (mg_match_t *)calloc(capacity, sizeof(mg_match_t));
        if (!set->data) {
            return false;
        }
        set->capacity = capacity;
    }
    return true;
}

/**
 * Ensure the match set can hold at least the specified number of elements.
 *
 * Allocates or grows the internal storage so that the set can contain at least
 * min_capacity elements and updates set->capacity on success; no allocation is
 * performed if the current capacity already meets or exceeds min_capacity.
 *
 * @param set Pointer to the match set to modify; if NULL the function does
 * nothing and returns `false`.
 * @param min_capacity Minimum required number of elements the set must be able
 * to hold.
 * @returns `true` if the set has at least min_capacity capacity after the call,
 * `false` on allocation failure or when `set` is NULL.
 */
bool mg_match_set_reserve(mg_match_set_t *set, uint32_t min_capacity) {
    if (!set) {
        return false;
    }
    if (set->capacity >= min_capacity) {
        return true;
    }
    uint32_t new_capacity = set->capacity ? set->capacity : 8U;
    while (new_capacity < min_capacity) {
        if (new_capacity > UINT32_MAX / 2U) { // prevent wrap
            new_capacity = min_capacity;      // fall back to exact fit
            break;
        }
        new_capacity *= 2U;
    }
    // size_t byte-count overflow guard
    if ((size_t)new_capacity > SIZE_MAX / sizeof(mg_match_t)) {
        return false;
    }
    mg_match_t *next = (mg_match_t *)realloc(set->data, (size_t)new_capacity *
                                                            sizeof(mg_match_t));
    if (!next) {
        return false;
    }
    set->data = next;
    set->capacity = new_capacity;
    return true;
}

/**
 * Append a match to the end of a match set, growing the set if necessary.
 *
 * Ensures the set has space for one more element and copies the provided
 * match into the next slot. Does nothing if `set` or `match` is NULL.
 *
 * @param set Destination match set to append to.
 * @param match Match to append (copied into the set).
 * @returns `true` if the match was appended successfully, `false` on failure.
 */
bool mg_match_set_push(mg_match_set_t *set, const mg_match_t *match) {
    if (!set || !match) {
        return false;
    }
    if (set->count == UINT32_MAX) {
        return false;
    }
    const uint32_t next_count = set->count + 1U;
    if (!mg_match_set_reserve(set, next_count)) {
        return false;
    }
    set->data[set->count++] = *match;
    return true;
}

/**
 * Reset the set to be empty without releasing its allocated storage.
 *
 * Clears the element count so the set contains no matches while preserving
 * any previously allocated internal buffer for reuse. If `set` is NULL,
 * no action is taken.
 *
 * @param set Match set to clear.
 */
void mg_match_set_clear(mg_match_set_t *set) {
    if (!set) {
        return;
    }
    set->count = 0;
}

/**
 * Release resources held by a match set and reset it to an empty state.
 *
 * Frees the internal data buffer if present and sets `data` to NULL, `count`
 * to 0, and `capacity` to 0. Safe to call with a NULL pointer.
 *
 * @param set Pointer to the mg_match_set_t to free; may be NULL.
 */
void mg_match_set_free(mg_match_set_t *set) {
    if (!set) {
        return;
    }
    free(set->data);
    set->data = NULL;
    set->count = 0;
    set->capacity = 0;
}