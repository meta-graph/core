#ifndef METAGRAPH_HILBERT_H
#define METAGRAPH_HILBERT_H

#include <stddef.h>
#include <stdint.h>

#include "metagraph/result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file hilbert.h
 * @brief Hilbert-space register used by the QCA runtime.
 *
 * A Hilbert register stores a bit-per-node state vector for the active
 * metagraph. The register owns its backing buffer and is not thread-safe.
 */
typedef struct {
    uint8_t *node_bits; /**< Heap-allocated bit array (little-endian). */
    size_t node_count;  /**< Number of logical nodes represented. */
} mg_hilbert_t;

/**
 * @brief Initialise a Hilbert register with the given node count.
 * @param hilbert Register to initialise (must be non-NULL).
 * @param count Number of nodes; newly allocated bits are zeroed.
 * @return METAGRAPH_OK on success, or an allocation error code.
 */
metagraph_result_t mg_hilbert_init(mg_hilbert_t *hilbert, size_t count);

/**
 * @brief Release resources held by a Hilbert register.
 * @param hilbert Register to free (may be NULL).
 */
void mg_hilbert_free(mg_hilbert_t *hilbert);

/**
 * @brief Resize an existing Hilbert register.
 * @param hilbert Register to resize (must be initialised).
 * @param new_count New node count; preserves existing bits up to the smaller
 *                  of the old and new sizes, zeroing any newly allocated tail.
 * @return METAGRAPH_OK on success, or an allocation error code.
 */
metagraph_result_t mg_hilbert_resize(mg_hilbert_t *hilbert, size_t new_count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* METAGRAPH_HILBERT_H */
