#include "core/ac_mem.h"

#include <stdlib.h>

#include "core/ac_log.h"
#include "core/ac_trace.h"

static ac_malloc_t ac_malloc_func = malloc;
static ac_free_t ac_free_func = free;
static ac_calloc_t ac_calloc_func = calloc;
static ac_realloc_t ac_realloc_func = realloc;
static ac_reallocarray_t ac_reallocarray_func = reallocarray;

static bool ac_mem_track = true;

static ac_mem_entry_t* ac_mem_entries = NULL;
static int32_t ac_mem_entries_size = 0;
static int32_t ac_mem_entries_capacity = 0;

#define ALLOC_TRACE_SIZE 32

void set_custom_malloc(ac_malloc_t malloc) {
    ac_malloc_func = malloc;
}

void set_custom_free(ac_free_t free) {
    ac_free_func = free;
}

void set_custom_calloc(ac_calloc_t calloc) {
    ac_calloc_func = calloc;
}

void set_custom_realloc(ac_realloc_t realloc) {
    ac_realloc_func = realloc;
}

void set_custom_reallocarray(ac_reallocarray_t reallocarray) {
    ac_reallocarray_func = reallocarray;
}

void ac_mem_track_enabled(bool enabled) {
    ac_mem_track = enabled;
}

void* ac_malloc(size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_malloc_func(size);
    }

    if (ac_mem_entries_size == ac_mem_entries_capacity) {
        ac_mem_entries_capacity = ac_mem_entries_capacity == 0 ? 1 : ac_mem_entries_capacity * 2;
        ac_mem_entries = ac_reallocarray_func(ac_mem_entries, ac_mem_entries_capacity, sizeof(ac_mem_entry_t));
    }
    ac_mem_entry_t* entry = &ac_mem_entries[ac_mem_entries_size++];
    entry->ptr = ac_malloc_func(size);
    entry->size = size;
    entry->state = AC_MEM_ENTRY_STATE_ALLOCATED;
    entry->type = type;

    entry->alloc_trace = ac_malloc_func(ALLOC_TRACE_SIZE * sizeof(void*));
    entry->alloc_trace_size = ac_get_intermediate_trace(entry->alloc_trace, ALLOC_TRACE_SIZE);
    entry->free_trace = NULL;
    entry->free_trace_size = 0;
    entry->realloc_traces = NULL;
    entry->realloc_traces_size = 0;
    entry->realloc_traces_capacity = 0;
    entry->realloc_trace_sizes = NULL;

    return entry->ptr;
}

void ac_free(void *ptr) {
    if (!ac_mem_track) {
        ac_free_func(ptr);
    }

    for (int32_t i = 0; i < ac_mem_entries_size; i++) {
        ac_mem_entry_t* entry = &ac_mem_entries[i];
        if (entry->ptr == ptr) {
            if (entry->state == AC_MEM_ENTRY_STATE_FREED) {
                ac_log_fatal("Double free detected\n");
                ac_log_fatal("Allocated at:\n");
                char buffer[1024];
                ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
                ac_log_fatal("%s\n", buffer);
                ac_log_fatal("Freed previously at :\n");
                ac_sprint_intermediate_trace(entry->free_trace, buffer, 0, entry->free_trace_size);
                ac_log_fatal("%s\n", buffer);
                ac_log_fatal("Current free at:\n");
                ac_print_trace(2);
                return;
            }
            entry->state = AC_MEM_ENTRY_STATE_FREED;
            entry->free_trace = ac_malloc_func(ALLOC_TRACE_SIZE * sizeof(void*));
            entry->free_trace_size = ac_get_intermediate_trace(entry->free_trace, ALLOC_TRACE_SIZE);
            ac_free_func(ptr);
            return;
        }
    }
}

void* ac_calloc(size_t nmemb, size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_calloc_func(nmemb, size);
    }

    if (ac_mem_entries_size == ac_mem_entries_capacity) {
        ac_mem_entries_capacity = ac_mem_entries_capacity == 0 ? 1 : ac_mem_entries_capacity * 2;
        ac_mem_entries = ac_reallocarray_func(ac_mem_entries, ac_mem_entries_capacity, sizeof(ac_mem_entry_t));
    }
    ac_mem_entry_t* entry = &ac_mem_entries[ac_mem_entries_size++];
    entry->ptr = ac_calloc_func(nmemb, size);
    entry->size = size;
    entry->state = AC_MEM_ENTRY_STATE_ALLOCATED;
    entry->type = type;

    entry->alloc_trace = ac_malloc_func(ALLOC_TRACE_SIZE * sizeof(void*));
    entry->alloc_trace_size = ac_get_intermediate_trace(entry->alloc_trace, ALLOC_TRACE_SIZE);

    ac_log_debug("Allocated at:\n");
    char buffer[1024];
    ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
    ac_log_debug("%s\n", buffer);

    entry->free_trace = NULL;
    entry->free_trace_size = 0;
    entry->realloc_traces = NULL;
    entry->realloc_traces_size = 0;
    entry->realloc_traces_capacity = 0;
    entry->realloc_trace_sizes = NULL;

    return entry->ptr;
}

void* ac_realloc(void *ptr, size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_realloc_func(ptr, size);
    }
    if (ptr == NULL) {
        return ac_malloc(size, type);
    }

    for (int32_t i = 0; i < ac_mem_entries_size; i++) {
        ac_mem_entry_t* entry = &ac_mem_entries[i];
        if (entry->ptr == ptr) {
            entry->state = AC_MEM_ENTRY_STATE_REALLOCATED;
            if (entry->realloc_traces_size == entry->realloc_traces_capacity) {
                entry->realloc_traces_capacity = entry->realloc_traces_capacity == 0 ? 1 : entry->realloc_traces_capacity * 2;
                entry->realloc_traces = ac_reallocarray_func(entry->realloc_traces, entry->realloc_traces_capacity, sizeof(void*));
                entry->realloc_trace_sizes = ac_reallocarray_func(entry->realloc_trace_sizes, entry->realloc_traces_capacity, sizeof(int32_t));
            }
            entry->realloc_traces[entry->realloc_traces_size] = ac_malloc_func(ALLOC_TRACE_SIZE * sizeof(void*));
            entry->realloc_trace_sizes[entry->realloc_traces_size] = ac_get_intermediate_trace(entry->realloc_traces[entry->realloc_traces_size], ALLOC_TRACE_SIZE);
            entry->realloc_traces_size++;
            entry->ptr = ac_realloc_func(ptr, size);
            return entry->ptr;
        }
    }
    ac_log_warn("Trying to realloc a ptr not in the records... Returning NULL");
    ac_log_info("Reallocation trace: ");
    ac_print_trace(2);
    return NULL;
}

void* ac_reallocarray(void *ptr, size_t nmemb, size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_reallocarray_func(ptr, nmemb, size);
    }
    if (ptr == NULL) {
        return ac_calloc(nmemb, size, type);
    }

    for (int32_t i = 0; i < ac_mem_entries_size; i++) {
        ac_mem_entry_t* entry = &ac_mem_entries[i];
        if (entry->ptr == ptr) {
            entry->state = AC_MEM_ENTRY_STATE_REALLOCATED;
            if (entry->realloc_traces_size == entry->realloc_traces_capacity) {
                entry->realloc_traces_capacity = entry->realloc_traces_capacity == 0 ? 1 : entry->realloc_traces_capacity * 2;
                entry->realloc_traces = ac_reallocarray_func(entry->realloc_traces, entry->realloc_traces_capacity, sizeof(void*));
                entry->realloc_trace_sizes = ac_reallocarray_func(entry->realloc_trace_sizes, entry->realloc_traces_capacity, sizeof(int32_t));
            }
            entry->realloc_traces[entry->realloc_traces_size] = ac_malloc_func(ALLOC_TRACE_SIZE * sizeof(void*));
            entry->realloc_trace_sizes[entry->realloc_traces_size] = ac_get_intermediate_trace(entry->realloc_traces[entry->realloc_traces_size], ALLOC_TRACE_SIZE);
            entry->realloc_traces_size++;
            entry->ptr = ac_reallocarray_func(ptr, nmemb, size);
            return entry->ptr;
        }
    }
    ac_log_warn("Trying to realloc a ptr not in the records... Returning NULL");
    ac_log_info("Reallocation trace: ");
    ac_print_trace(2);
    return NULL;
}

void ac_mem_exit(void) {
    for (int32_t i = 0; i < ac_mem_entries_size; i++) {
        ac_mem_entry_t* entry = &ac_mem_entries[i];
        if (entry->state == AC_MEM_ENTRY_STATE_ALLOCATED) {
            ac_log_warn("Memory leak detected\n");
            ac_log_warn("Allocated at:\n");
            char buffer[1024];
            ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
            ac_log_warn("\n---\n%s\n---\n", buffer);
            ac_log_warn("Not freed\n");
        } else if (entry->state == AC_MEM_ENTRY_STATE_REALLOCATED) {
            ac_log_warn("Memory leak detected\n");
            ac_log_warn("Allocated at:\n");
            char buffer[1024];
            ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
            ac_log_warn("\n---\n%s\n---\n", buffer);
            for (int32_t j = 0; j < entry->realloc_traces_size; j++) {
                ac_log_warn("Reallocated at:\n");
                ac_sprint_intermediate_trace(entry->realloc_traces[j], buffer, 0, entry->realloc_trace_sizes[j]);
                ac_log_warn("\n---\n%s\n---\n", buffer);
            }
            ac_log_warn("Reallocated but not freed\n");
        }
        ac_free_func(entry->alloc_trace);
        if (entry->free_trace != NULL)
            ac_free_func(entry->free_trace);
        
        if (entry->realloc_traces == NULL) {
            continue;
        }
        for (int32_t j = 0; j < entry->realloc_traces_size; j++) {
            ac_free_func(entry->realloc_traces[j]);
        }
        ac_free_func(entry->realloc_traces);
        ac_free_func(entry->realloc_trace_sizes);
    }
    ac_free_func(ac_mem_entries);
    ac_mem_entries = NULL;
    ac_mem_entries_size = 0;
    ac_mem_entries_capacity = 0;
}
