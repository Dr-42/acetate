#include "ds/ac_darray.h"

#include "core/ac_mem.h"

#include <string.h>
#include <math.h>

static float AC_DARRAY_RESIZE_FACTOR = 1.6f;

void ac_darray_set_resize_factor(float factor) { AC_DARRAY_RESIZE_FACTOR = factor; }

ac_darray_t* ac_darray_create(uint64_t element_size, uint64_t capacity, ac_mem_entry_type_t mem_type) {
    ac_darray_t* darray = ac_malloc(sizeof(ac_darray_t), AC_MEM_ENTRY_DS);
    darray->element_size = element_size;
    darray->capacity = capacity;
    darray->size = 0;
    darray->mem_type = mem_type;
    darray->data = ac_malloc(element_size * capacity, mem_type);
    return darray;
}

void ac_darray_destroy(ac_darray_t* darray) {
    ac_free(darray->data);
    ac_free(darray);
}

void ac_darray_push(ac_darray_t* darray, void* element) {
    if (darray->size == darray->capacity) {
        darray->capacity = (uint64_t)ceilf((float)darray->capacity * AC_DARRAY_RESIZE_FACTOR);
        darray->data = ac_realloc(darray->data, darray->element_size * darray->capacity, AC_MEM_ENTRY_DS);
    }
    ac_memcpy((uint8_t*)darray->data + darray->size * darray->element_size, element, darray->element_size);
    darray->size++;
}

void ac_darray_pop(ac_darray_t* darray, void* element) {
    ac_memcpy(element, (uint8_t*)darray->data + (darray->size - 1) * darray->element_size, darray->element_size);
    darray->size--;
}

void ac_darray_insert(ac_darray_t* darray, uint64_t index, void* element) {
    if (darray->size == darray->capacity) {
        darray->capacity = (uint64_t)ceilf((float)darray->capacity * AC_DARRAY_RESIZE_FACTOR);
        darray->data = ac_realloc(darray->data, darray->element_size * darray->capacity, AC_MEM_ENTRY_DS);
    }
    ac_memmove((uint8_t*)darray->data + (index + 1) * darray->element_size, (uint8_t*)darray->data + index * darray->element_size,
               (darray->size - index) * darray->element_size);
    ac_memcpy((uint8_t*)darray->data + index * darray->element_size, element, darray->element_size);
    darray->size++;
}

void ac_darray_remove(ac_darray_t* darray, uint64_t index, void* element) {
    ac_memcpy(element, (uint8_t*)darray->data + index * darray->element_size, darray->element_size);
    ac_memmove((uint8_t*)darray->data + index * darray->element_size, (uint8_t*)darray->data + (index + 1) * darray->element_size,
               (darray->size - index - 1) * darray->element_size);
    darray->size--;
}

void ac_darray_get(ac_darray_t* darray, uint64_t index, void* element) {
    ac_memcpy(element, (uint8_t*)darray->data + index * darray->element_size, darray->element_size);
}

void ac_darray_set(ac_darray_t* darray, uint64_t index, void* element) {
    ac_memcpy((uint8_t*)darray->data + index * darray->element_size, element, darray->element_size);
}

void ac_darray_clear(ac_darray_t* darray) { darray->size = 0; }
