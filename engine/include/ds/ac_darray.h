#ifndef ACETATE_DS_DARRAY_H
#define ACETATE_DS_DARRAY_H

/**
 * @file ac_darray.h
 * @brief Dynamic array functions.
 */

#include <stdint.h>

#include "core/ac_mem.h"

/**
 * @brief Dynamic array structure.
 * @see ac_darray_create
 */
typedef struct ac_darray_t {
    /**
     * @brief The size of each element in the array.
     */
    uint64_t element_size;
    /**
     * @brief The capacity of the array.
     * @see ac_darray_create
     */
    uint64_t capacity;
    /**
     * @brief The number of elements in the array.
     */
    uint64_t size;
    /**
     * @brief The memory type of the array.
     */
    ac_mem_entry_type_t mem_type;
    /**
     * @brief The data of the array.
     */
    void* data;
} ac_darray_t;

/**
 * @brief Set the resize factor of the dynamic array.
 * @param factor The resize factor.
 */
void ac_darray_set_resize_factor(float factor);

/**
 * @brief Create a new dynamic array.
 * @param element_size The size of each element in the array.
 * @param capacity The initial capacity of the array.
 * @param mem_type The memory type of the array.
 * @return A pointer to the new dynamic array.
 */
ac_darray_t* ac_darray_create(uint64_t element_size, uint64_t capacity, ac_mem_entry_type_t mem_type);

/**
 * @brief Destroy the dynamic array.
 * @param darray The dynamic array to destroy.
 */
void ac_darray_destroy(ac_darray_t* darray);

/**
 * @brief Push an element to the dynamic array.
 * @param darray The dynamic array.
 * @param element The element to push.
 */
void ac_darray_push(ac_darray_t* darray, void* element);

/**
 * @brief Pop an element from the dynamic array.
 * @param darray The dynamic array.
 * @param element The element to pop.
 */
void ac_darray_pop(ac_darray_t* darray, void* element);

/**
 * @brief Insert an element to the dynamic array.
 * @param darray The dynamic array.
 * @param index The index to insert the element.
 * @param element The element to insert.
 */
void ac_darray_insert(ac_darray_t* darray, uint64_t index, void* element);

/**
 * @brief Remove an element from the dynamic array.
 * @param darray The dynamic array.
 * @param index The index to remove the element.
 * @param element The element to remove.
 */
void ac_darray_remove(ac_darray_t* darray, uint64_t index, void* element);

/**
 * @brief Get an element from the dynamic array.
 * @param darray The dynamic array.
 * @param index The index to get the element.
 * @param element The element to get.
 */
void ac_darray_get(ac_darray_t* darray, uint64_t index, void* element);

/**
 * @brief Set an element in the dynamic array.
 * @param darray The dynamic array.
 * @param index The index to set the element.
 * @param element The element to set.
 */
void ac_darray_set(ac_darray_t* darray, uint64_t index, void* element);

/**
 * @brief Clear the dynamic array.
 * @param darray The dynamic array to clear.
 */
void ac_darray_clear(ac_darray_t* darray);

#endif  // ACETATE_DS_DARRAY_H
