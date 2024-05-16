#ifndef AC_DS_BITMASK_H
#define AC_DS_BITMASK_H

/**
 * @file ac_bitmask.h
 * @brief A bitmask data structure.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/ac_mem.h"

/**
 * A bitmask that can store an arbitrary number of bits.
 */
typedef struct ac_bitmask {
    /**
     * The bits in the bitmask.
     */
    uint64_t* bits;
    /**
     * The number of bits in the bitmask.
     */
    size_t num_bits;
} ac_bitmask_t;

/**
 * Creates a new bitmask.
 *
 * @param num_bits The number of bits in the bitmask.
 * @param mem_type The memory entry type to use.
 * @return The new bitmask.
 * @see ac_mem_entry_type_t
 */
ac_bitmask_t* ac_bitmask_create(size_t num_bits, ac_mem_entry_type_t mem_type);

/**
 * Destroys a bitmask.
 *
 * @param bitmask The bitmask to destroy.
 */
void ac_bitmask_destroy(ac_bitmask_t* bitmask);

/**
 * Sets a bit in the bitmask.
 *
 * @param bitmask The bitmask.
 * @param index The index of the bit to set.
 */
void ac_bitmask_set(ac_bitmask_t* bitmask, size_t index);

/**
 * Clears a bit in the bitmask.
 *
 * @param bitmask The bitmask.
 * @param index The index of the bit to clear.
 */
void ac_bitmask_clear(ac_bitmask_t* bitmask, size_t index);

/**
 * Gets a bit in the bitmask.
 *
 * @param bitmask The bitmask.
 * @param index The index of the bit to get.
 * @return The value of the bit.
 */
bool ac_bitmask_get(ac_bitmask_t* bitmask, size_t index);

/**
 * Gets whether any bit in the bitmask is set.
 *
 * @param bitmask The bitmask.
 * @return Whether any bit in the bitmask is set.
 */
bool ac_bitmask_get_any(ac_bitmask_t* bitmask);

#endif  // AC_DS_BITMASK_H
