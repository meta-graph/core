#ifndef METAGRAPH_EPOCH_H
#define METAGRAPH_EPOCH_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct {
    _Atomic(uint64_t) epoch;
} mg_epoch_t;

/**
 * Initialize an epoch counter to 1.
 * @param e Epoch object to initialize (its internal atomic counter will be set to 1).
 */
static inline void mg_epoch_init(mg_epoch_t *e) { atomic_store(&e->epoch, 1); }
/**
 * Get the current epoch value from the epoch counter.
 * @param e Pointer to the epoch object to read.
 * @returns The current epoch value.
 */
static inline uint64_t mg_epoch_load(const mg_epoch_t *e) {
    return atomic_load(&e->epoch);
}
/**
 * Atomically advance the epoch counter by one.
 *
 * @param e Epoch object whose counter will be incremented.
 */
static inline void mg_epoch_flip(mg_epoch_t *e) {
    atomic_fetch_add(&e->epoch, 1);
}

#endif /* METAGRAPH_EPOCH_H */