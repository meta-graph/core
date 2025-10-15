#include "metagraph/match.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

bool mg_match_set_init(mg_match_set_t *set, uint32_t capacity) {
    if (!set) {
        return false;
    }
    memset(set, 0, sizeof(*set));
    if (capacity > 0) {
        set->data = (mg_match_t *)calloc(capacity, sizeof(mg_match_t));
        if (!set->data) {
            return false;
        }
        set->capacity = capacity;
    }
    return true;
}

bool mg_match_set_reserve(mg_match_set_t *set, uint32_t min_capacity) {
    if (!set) {
        return false;
    }
    if (set->capacity >= min_capacity) {
        return true;
    }
    uint32_t new_capacity = set->capacity ? set->capacity : 8U;
    while (new_capacity < min_capacity) {
        new_capacity *= 2U;
    }
    mg_match_t *next =
        (mg_match_t *)realloc(set->data, new_capacity * sizeof(mg_match_t));
    if (!next) {
        return false;
    }
    set->data = next;
    set->capacity = new_capacity;
    return true;
}

bool mg_match_set_push(mg_match_set_t *set, const mg_match_t *match) {
    if (!set || !match) {
        return false;
    }
    if (!mg_match_set_reserve(set, set->count + 1U)) {
        return false;
    }
    set->data[set->count++] = *match;
    return true;
}

void mg_match_set_clear(mg_match_set_t *set) {
    if (!set) {
        return;
    }
    set->count = 0;
}

void mg_match_set_free(mg_match_set_t *set) {
    if (!set) {
        return;
    }
    free(set->data);
    set->data = NULL;
    set->count = 0;
    set->capacity = 0;
}
