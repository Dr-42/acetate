#ifndef ACETATE_DS_DARRAY_H
#define ACETATE_DS_DARRAY_H
#include <stdint.h>

#include "core/ac_mem.h"

typedef struct ac_darray_t {
	uint64_t element_size;
	uint64_t capacity;
	uint64_t size;
	ac_mem_entry_type_t mem_type;
	void* data;
} ac_darray_t;

void ac_darray_set_resize_factor(float factor);

ac_darray_t* ac_darray_create(uint64_t element_size, uint64_t capacity,	ac_mem_entry_type_t mem_type);
void ac_darray_destroy(ac_darray_t* darray);

void ac_darray_push(ac_darray_t* darray, void* element);
void ac_darray_pop(ac_darray_t* darray, void* element);

void ac_darray_insert(ac_darray_t* darray, uint64_t index, void* element);
void ac_darray_remove(ac_darray_t* darray, uint64_t index, void* element);

void ac_darray_get(ac_darray_t* darray, uint64_t index, void* element);
void ac_darray_set(ac_darray_t* darray, uint64_t index, void* element);

void ac_darray_clear(ac_darray_t* darray);
#endif //ACETATE_DS_DARRAY_H
