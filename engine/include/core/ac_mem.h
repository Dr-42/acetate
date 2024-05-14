#ifndef AC_CORE_MEM_H
#define AC_CORE_MEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void* (*ac_malloc_t)(size_t size);
typedef void (*ac_free_t)(void *ptr);
typedef void* (*ac_calloc_t)(size_t nmemb, size_t size);
typedef void* (*ac_realloc_t)(void *ptr, size_t size);
typedef void* (*ac_reallocarray_t)(void *ptr, size_t nmemb, size_t size);

void set_custom_malloc(ac_malloc_t malloc);
void set_custom_free(ac_free_t free);
void set_custom_calloc(ac_calloc_t calloc);
void set_custom_realloc(ac_realloc_t realloc);
void set_custom_reallocarray(ac_reallocarray_t reallocarray);

void ac_mem_track_enabled(bool enabled);

typedef enum ac_mem_entry_state_t {
	AC_MEM_ENTRY_STATE_ALLOCATED,
	AC_MEM_ENTRY_STATE_FREED,
	AC_MEM_ENTRY_STATE_REALLOCATED
} ac_mem_entry_state_t;

typedef enum ac_mem_entry_type_t {
	AC_MEM_ENTRY_CORE,
	AC_MEM_ENTRY_DS,
} ac_mem_entry_type_t;

typedef struct ac_mem_entry_t {
	void *ptr;
	size_t size;
	ac_mem_entry_state_t state;
	ac_mem_entry_type_t type;
	void** alloc_trace;
	int32_t alloc_trace_size;
	void** free_trace;
	int32_t free_trace_size;
	void*** realloc_traces;
	int32_t realloc_traces_size;
	int32_t realloc_traces_capacity;
	int32_t* realloc_trace_sizes;
} ac_mem_entry_t;

void* ac_malloc(size_t size, ac_mem_entry_type_t type);
void ac_free(void *ptr);
void* ac_calloc(size_t nmemb, size_t size, ac_mem_entry_type_t type);
void* ac_realloc(void *ptr, size_t size, ac_mem_entry_type_t type);
void* ac_reallocarray(void *ptr, size_t nmemb, size_t size, ac_mem_entry_type_t type);

void ac_mem_exit(void);

#endif // AC_CORE_MEM_H
