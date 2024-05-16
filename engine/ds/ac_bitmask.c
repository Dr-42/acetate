#include <stdarg.h>

#include "ds/ac_bitmask.h"

ac_bitmask_t* ac_bitmask_create(size_t num_bits, ac_mem_entry_type_t mem_type) {
    size_t num_bytes = num_bits / 8;
    if (num_bits % 8 != 0) {
        num_bytes++;
    }
    size_t num_words = num_bytes / sizeof(uint64_t);
    if (num_bytes % sizeof(uint64_t) != 0) {
        num_words++;
    }
    ac_bitmask_t* bitmask = ac_malloc(sizeof(ac_bitmask_t), mem_type);
    bitmask->bits = ac_malloc(num_words * sizeof(uint64_t), mem_type);
    bitmask->num_bits = num_bits;
    return bitmask;
}

void ac_bitmask_destroy(ac_bitmask_t* bitmask) {
    ac_free(bitmask->bits);
    ac_free(bitmask);
}

void ac_bitmask_set(ac_bitmask_t* bitmask, size_t index) {
    size_t word_index = index / 64;
    size_t bit_index = index % 64;
    bitmask->bits[word_index] |= (1 << bit_index);
}

void ac_bitmask_clear(ac_bitmask_t* bitmask, size_t index) {
    size_t word_index = index / 64;
    size_t bit_index = index % 64;
    bitmask->bits[word_index] &= ~(1 << bit_index);
}

bool ac_bitmask_get(ac_bitmask_t* bitmask, size_t index) {
    size_t word_index = index / 64;
    size_t bit_index = index % 64;
    return bitmask->bits[word_index] & (1 << bit_index);
}

bool ac_bitmask_get_any(ac_bitmask_t* bitmask) {
    size_t num_words = bitmask->num_bits / 64;
    if (bitmask->num_bits % 64 != 0) {
        num_words++;
    }
    for (size_t i = 0; i < num_words; i++) {
        if (bitmask->bits[i] != 0) {
            return true;
        }
    }
    return false;
}
