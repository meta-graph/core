#ifndef METAGRAPH_EPOCH_H
#define METAGRAPH_EPOCH_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct {
    _Atomic(uint64_t) epoch;
} mg_epoch_t;

static inline void mg_epoch_init(mg_epoch_t *e) { atomic_store(&e->epoch, 1); }
static inline uint64_t mg_epoch_load(const mg_epoch_t *e) {
    return atomic_load(&e->epoch);
}
static inline void mg_epoch_flip(mg_epoch_t *e) {
    atomic_fetch_add(&e->epoch, 1);
}

#endif /* METAGRAPH_EPOCH_H */
