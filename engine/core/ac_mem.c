#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "core/ac_log.h"
#include "core/ac_mem.h"
#include "core/ac_trace.h"
#include "ds/ac_map.h"

static ac_malloc_t ac_malloc_func = malloc;
static ac_free_t ac_free_func = free;
static ac_calloc_t ac_calloc_func = calloc;
static ac_realloc_t ac_realloc_func = realloc;

static bool ac_mem_track = true;

static ac_map_t* ac_mem_map = NULL;

#define ALLOC_TRACE_SIZE 32

void set_custom_malloc(ac_malloc_t malloc) { ac_malloc_func = malloc; }

void set_custom_free(ac_free_t free) { ac_free_func = free; }

void set_custom_calloc(ac_calloc_t calloc) { ac_calloc_func = calloc; }

void set_custom_realloc(ac_realloc_t realloc) { ac_realloc_func = realloc; }

void ac_mem_track_enabled(bool enabled) { ac_mem_track = enabled; }

static int ac_mem_ptr_cmp(const void* a, const void* b) {
    if (a == b) {
        return 0;
    }
    return a < b ? -1 : 1;
}

static size_t ac_mem_ptr_hash(const void* key) {
    size_t hash = SIP64((void*)&key, sizeof(void*), 0, 1);
    // size_t hash = (size_t)key;
    return hash;
}

static void* ac_mem_ptr_copy(const void* key, ac_mem_entry_type_t type) {
    (void)type;
    return (void*)key;
}

static void ac_mem_ptr_free(void* key) { (void)key; }

static int ac_mem_ptr_display(const void* key, char* buffer, size_t size) { return snprintf(buffer, size, "%p", key); }

static ac_map_key_ops_t ac_mem_key_ops = {.cmp = ac_mem_ptr_cmp,
                                          .hash = ac_mem_ptr_hash,
                                          .copy = ac_mem_ptr_copy,
                                          .free = ac_mem_ptr_free,
                                          .display = ac_mem_ptr_display};

static void* ac_mem_entry_copy(const void* value, ac_mem_entry_type_t type) {
    (void)type;
    ac_mem_entry_t* entry = (ac_mem_entry_t*)value;
    ac_mem_entry_t* copy = ac_malloc_func(sizeof(ac_mem_entry_t));
    memcpy(copy, entry, sizeof(ac_mem_entry_t));

    copy->alloc_trace = ac_malloc_func(entry->alloc_trace_size * sizeof(void*));
    memcpy(copy->alloc_trace, entry->alloc_trace, entry->alloc_trace_size * sizeof(void*));
    copy->alloc_trace_size = entry->alloc_trace_size;

    if (entry->free_trace == NULL) {
        copy->free_trace = NULL;
    } else {
        copy->free_trace = ac_malloc_func(entry->free_trace_size * sizeof(void*));
        memcpy(copy->free_trace, entry->free_trace, entry->free_trace_size * sizeof(void*));
        copy->free_trace_size = entry->free_trace_size;
    }

    if (entry->realloc_traces == NULL) {
        copy->realloc_traces = NULL;
        copy->realloc_trace_sizes = NULL;
        copy->realloc_traces_size = 0;
        copy->realloc_traces_capacity = 0;
        copy->realloc_count = 0;
        copy->realloc_trace_sizes_capacity = 0;
    } else {
        copy->realloc_traces = ac_malloc_func(entry->realloc_traces_capacity * sizeof(void*));
        memcpy(copy->realloc_traces, entry->realloc_traces, entry->realloc_traces_capacity * sizeof(void*));
        copy->realloc_trace_sizes = ac_malloc_func(entry->realloc_trace_sizes_capacity * sizeof(int32_t));
        memcpy(copy->realloc_trace_sizes, entry->realloc_trace_sizes, entry->realloc_trace_sizes_capacity * sizeof(int32_t));
        copy->realloc_traces_size = entry->realloc_traces_size;
        copy->realloc_traces_capacity = entry->realloc_traces_capacity;
        copy->realloc_count = entry->realloc_count;
        copy->realloc_trace_sizes_capacity = entry->realloc_trace_sizes_capacity;
    }
    return copy;
}

static void ac_mem_entry_free(void* value) {
    ac_mem_entry_t* entry = (ac_mem_entry_t*)value;
    if (entry->alloc_trace != NULL) {
        ac_free_func(entry->alloc_trace);
        entry->alloc_trace = NULL;
    }
    if (entry->free_trace != NULL) {
        ac_free_func(entry->free_trace);
        entry->free_trace = NULL;
    }

    if (entry->realloc_traces != NULL) {
        ac_free_func(entry->realloc_traces);
        entry->realloc_traces = NULL;
        ac_free_func(entry->realloc_trace_sizes);
        entry->realloc_trace_sizes = NULL;
    }
    ac_free_func(entry);
}

static int ac_mem_entry_display(const void* value, char* buffer, size_t size) {
    ac_mem_entry_t* entry = (ac_mem_entry_t*)value;
    int offset = 0;
    offset += snprintf(buffer + offset, size - offset, "ptr: %p\n", entry->ptr);
    offset += snprintf(buffer + offset, size - offset, "size: %zu\n", entry->size);
    offset += snprintf(buffer + offset, size - offset, "state: %d\n", entry->state);
    offset += snprintf(buffer + offset, size - offset, "type: %d\n", entry->type);
    offset += snprintf(buffer + offset, size - offset, "alloc_trace: ");
    char trace_buffer[1024];
    ac_sprint_intermediate_trace(entry->alloc_trace, trace_buffer, 0, entry->alloc_trace_size);
    offset += snprintf(buffer + offset, size - offset, "%s", trace_buffer);
    offset += snprintf(buffer + offset, size - offset, "\n");
    offset += snprintf(buffer + offset, size - offset, "free_trace: ");
    ac_sprint_intermediate_trace(entry->free_trace, trace_buffer, 0, entry->free_trace_size);
    offset += snprintf(buffer + offset, size - offset, "%s", trace_buffer);
    offset += snprintf(buffer + offset, size - offset, "\n");
    offset += snprintf(buffer + offset, size - offset, "realloc_traces: ");
    int32_t trace_offset = 0;
    for (int32_t i = 0; i < entry->realloc_count; i++) {
        ac_sprint_intermediate_trace(entry->realloc_traces + trace_offset, trace_buffer, 0, entry->realloc_trace_sizes[i]);
        offset += snprintf(buffer + offset, size - offset, "%s", trace_buffer);
        trace_offset += entry->realloc_trace_sizes[i];
    }
    offset += snprintf(buffer + offset, size - offset, "\n");
    return offset;
}

static ac_map_value_ops_t ac_mem_value_ops = {
    .copy = ac_mem_entry_copy,
    .free = ac_mem_entry_free,
    .display = ac_mem_entry_display,
};

static void ac_mem_map_free_op(void* ptr) { ac_free_func(ptr); }

static void* ac_mem_map_malloc_op(size_t size, ac_mem_entry_type_t entry_type) {
    (void)entry_type;
    return ac_malloc_func(size);
}

static void* ac_mem_map_calloc_op(size_t nmemb, size_t size, ac_mem_entry_type_t entry_type) {
    (void)entry_type;
    return ac_calloc_func(nmemb, size);
}

static ac_map_mem_ops_t map_mem_ops = {
    .map_free = ac_mem_map_free_op,
    .map_malloc = ac_mem_map_malloc_op,
    .map_calloc = ac_mem_map_calloc_op,
};

void ac_mem_init(void) {
    if (!ac_mem_track) {
        return;
    }
    ac_mem_map = ac_map_new_custom(16, AC_MEM_ENTRY_CORE, map_mem_ops, ac_mem_key_ops, ac_mem_value_ops);
}

void* ac_malloc(size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_malloc_func(size);
    }

    void* ptr = ac_malloc_func(size);
    ac_mem_entry_t entry = {0};
    entry.ptr = ptr;
    entry.size = size;
    entry.state = AC_MEM_ENTRY_STATE_ALLOCATED;
    entry.type = type;

    void* trace[ALLOC_TRACE_SIZE];
    entry.alloc_trace_size = ac_get_intermediate_trace(trace, ALLOC_TRACE_SIZE);
    entry.alloc_trace = ac_malloc_func(entry.alloc_trace_size * sizeof(void*));
    memcpy(entry.alloc_trace, trace, entry.alloc_trace_size * sizeof(void*));
    entry.free_trace = NULL;
    entry.free_trace_size = 0;
    entry.realloc_traces = NULL;
    entry.realloc_traces_size = 0;
    entry.realloc_traces_capacity = 0;
    entry.realloc_trace_sizes = NULL;
    entry.realloc_count = 0;
    entry.realloc_trace_sizes_capacity = 0;

    ac_mem_entry_t* sus_entry = ac_map_get(ac_mem_map, ptr);
    if (sus_entry) {
        if (sus_entry->state != AC_MEM_ENTRY_STATE_FREED) {
            ac_log_fatal("Memory corruption detected\n");
            ac_log_fatal("Allocated at:\n");
            char buffer[1024];
            ac_sprint_intermediate_trace(sus_entry->alloc_trace, buffer, 0, sus_entry->alloc_trace_size);
            ac_log_fatal("%s\n", buffer);
            ac_log_fatal("Current malloc at:\n");
            ac_print_trace(2);
            ac_log_fatal_exit("Exiting");
        }
        ac_mem_entry_free(sus_entry);
    }

    ac_map_set(ac_mem_map, ptr, &entry);
    ac_free_func(entry.alloc_trace);
    return ptr;
}

void ac_free(void* ptr) {
    if (!ac_mem_track) {
        ac_free_func(ptr);
    }

    ac_mem_entry_t* entry = ac_map_get(ac_mem_map, ptr);
    if (!entry) {
        ac_log_info("Free trace: ");
        ac_print_trace(2);
        ac_log_fatal("Ptr: %p\n", ptr);
        ac_log_fatal("Trying to free a ptr not in the records.. Entry not found... Exiting\n");
        return;
    }
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
        void* trace[ALLOC_TRACE_SIZE];
        entry->free_trace_size = ac_get_intermediate_trace(trace, ALLOC_TRACE_SIZE);
        entry->free_trace = ac_malloc_func(entry->free_trace_size * sizeof(void*));
        memcpy(entry->free_trace, trace, entry->free_trace_size * sizeof(void*));
        ac_free_func(ptr);
        ac_map_set(ac_mem_map, ptr, entry);
        ac_mem_entry_free(entry);
        return;
    }
    ac_log_fatal("Entry pointer mismatch... \n");
    ac_log_fatal("Entry ptr: %p\n", entry->ptr);
    ac_log_fatal("Free ptr: %p\n", ptr);
    ac_log_fatal_exit("Exiting");
}

void* ac_calloc(size_t nmemb, size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_calloc_func(size, nmemb);
    }

    void* ptr = ac_calloc_func(nmemb, size);
    ac_mem_entry_t entry = {0};
    entry.ptr = ptr;
    entry.size = size;
    entry.state = AC_MEM_ENTRY_STATE_ALLOCATED;
    entry.type = type;

    void* trace[ALLOC_TRACE_SIZE];
    entry.alloc_trace_size = ac_get_intermediate_trace(trace, ALLOC_TRACE_SIZE);
    entry.alloc_trace = ac_malloc_func(entry.alloc_trace_size * sizeof(void*));
    memcpy(entry.alloc_trace, trace, entry.alloc_trace_size * sizeof(void*));
    entry.free_trace = NULL;
    entry.free_trace_size = 0;
    entry.realloc_traces = NULL;
    entry.realloc_traces_size = 0;
    entry.realloc_traces_capacity = 0;
    entry.realloc_trace_sizes = NULL;
    entry.realloc_count = 0;
    entry.realloc_trace_sizes_capacity = 0;

    ac_mem_entry_t* sus_entry = ac_map_get(ac_mem_map, ptr);
    if (sus_entry) {
        if (sus_entry->state != AC_MEM_ENTRY_STATE_FREED) {
            ac_log_fatal("Memory corruption detected\n");
            ac_log_fatal("Allocated at:\n");
            char buffer[1024];
            ac_sprint_intermediate_trace(sus_entry->alloc_trace, buffer, 0, sus_entry->alloc_trace_size);
            ac_log_fatal("%s\n", buffer);
            ac_log_fatal("Current calloc at:\n");
            ac_print_trace(2);
            ac_log_fatal_exit("Exiting");
        }
        ac_mem_entry_free(sus_entry);
    }

    ac_map_set(ac_mem_map, ptr, &entry);
    ac_free_func(entry.alloc_trace);
    return entry.ptr;
}

void* ac_realloc(void* ptr, size_t size, ac_mem_entry_type_t type) {
    if (!ac_mem_track) {
        return ac_realloc_func(ptr, size);
    }
    if (ptr == NULL) {
        return ac_malloc(size, type);
    }

    ac_mem_entry_t* entry = NULL;
    entry = ac_map_get(ac_mem_map, ptr);

    if (!entry) {
        ac_log_warn("Trying to realloc a ptr not in the records... Returning NULL");
        ac_log_info("Reallocation trace: ");
        ac_print_trace(2);
        return NULL;
    }

    if (entry->ptr == ptr) {
        entry->state = AC_MEM_ENTRY_STATE_REALLOCATED;
        void* realloc_trace[ALLOC_TRACE_SIZE];
        int32_t realloc_trace_size = ac_get_intermediate_trace(realloc_trace, ALLOC_TRACE_SIZE);
        if (entry->realloc_traces_capacity < realloc_trace_size + entry->realloc_traces_size) {
            entry->realloc_traces_capacity = (realloc_trace_size + entry->realloc_traces_size) * 2;
            void** new_realloc_traces = ac_realloc_func(entry->realloc_traces, entry->realloc_traces_capacity * sizeof(void*));
            if (new_realloc_traces) {
                entry->realloc_traces = new_realloc_traces;
            } else {
                ac_log_fatal("Realloc failed\n");
                ac_log_fatal_exit("Exiting");
            }
        }
        if (entry->realloc_count == entry->realloc_trace_sizes_capacity) {
            entry->realloc_trace_sizes_capacity =
                entry->realloc_trace_sizes_capacity == 0 ? 1 : entry->realloc_trace_sizes_capacity * 2;
            int32_t* new_realloc_trace_sizes =
                ac_realloc_func(entry->realloc_trace_sizes, entry->realloc_trace_sizes_capacity * sizeof(int32_t));
            if (new_realloc_trace_sizes) {
                entry->realloc_trace_sizes = new_realloc_trace_sizes;
            } else {
                ac_log_fatal("Realloc failed\n");
                ac_log_fatal_exit("Exiting");
            }
        }
        entry->realloc_trace_sizes[entry->realloc_count] = realloc_trace_size;
        memcpy(entry->realloc_traces + entry->realloc_traces_size, realloc_trace, realloc_trace_size * sizeof(void*));
        entry->realloc_traces_size += realloc_trace_size;
        entry->realloc_count++;
        void* new_ptr = ac_realloc_func(ptr, size);

        ac_mem_entry_t* sus_entry = ac_map_get(ac_mem_map, new_ptr);
        if (sus_entry) {
            if (sus_entry->state != AC_MEM_ENTRY_STATE_FREED) {
                ac_log_fatal("Memory corruption detected\n");
                ac_log_fatal("Allocated at:\n");
                char buffer[1024];
                ac_sprint_intermediate_trace(sus_entry->alloc_trace, buffer, 0, sus_entry->alloc_trace_size);
                ac_log_fatal("%s\n", buffer);
                ac_log_fatal("Current realloc at:\n");
                ac_print_trace(2);
                ac_log_fatal_exit("Exiting");
            }
            ac_mem_entry_free(sus_entry);
        }
        entry->ptr = new_ptr;
        ac_map_remove(ac_mem_map, ptr);
        ac_map_set(ac_mem_map, new_ptr, entry);

        ac_mem_entry_free(entry);

        return new_ptr;
    }
    ac_log_warn("Trying to realloc a ptr not in the records... Returning NULL");
    ac_log_info("Reallocation trace: ");
    ac_print_trace(2);
    return NULL;
}

void ac_mem_map_exit_iter_cb(const void* key, const void* value) {
    (void)key;
    ac_mem_entry_t* entry = (ac_mem_entry_t*)value;
    if (entry->state == AC_MEM_ENTRY_STATE_ALLOCATED) {
        ac_log_warn("----------------------------\n");
        ac_log_warn("Memory leak detected\n");
        ac_log_warn("Pointer: %p\n", entry->ptr);
        ac_log_warn("Allocated at:\n");
        char buffer[1024];
        ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
        ac_log_warn("\n---\n%s\n---\n", buffer);
        ac_log_warn("Not freed\n");
        ac_log_warn("----------------------------\n");
    } else if (entry->state == AC_MEM_ENTRY_STATE_REALLOCATED) {
        ac_log_warn("----------------------------\n");
        ac_log_warn("Memory leak detected\n");
        ac_log_warn("Pointer: %p\n", entry->ptr);
        ac_log_warn("Allocated at:\n");
        char buffer[1024];
        ac_sprint_intermediate_trace(entry->alloc_trace, buffer, 0, entry->alloc_trace_size);
        ac_log_warn("\n---\n%s\n---\n", buffer);
        int32_t trace_offset = 0;
        for (int32_t j = 0; j < entry->realloc_count; j++) {
            ac_log_warn("Reallocated at:\n");
            ac_sprint_intermediate_trace(entry->realloc_traces + trace_offset, buffer, 0, entry->realloc_trace_sizes[j]);
            ac_log_warn("\n---\n%s\n---\n", buffer);
            trace_offset += entry->realloc_trace_sizes[j];
        }
        ac_log_warn("Reallocated but not freed\n");
        ac_log_warn("----------------------------\n");
    }
}

void ac_mem_exit(void) {
    if (!ac_mem_track) {
        return;
    }
    ac_map_iter(ac_mem_map, ac_mem_map_exit_iter_cb);
    ac_map_destroy(ac_mem_map);
}

void ac_memcpy(void* dest, const void* src, size_t n) { memcpy(dest, src, n); }

void ac_memmove(void* dest, const void* src, size_t n) { memmove(dest, src, n); }

void ac_memset(void* s, int c, size_t n) { memset(s, c, n); }

void ac_memzero(void* s, size_t n) { memset(s, 0, n); }

static const char* mem_entry_type_str(ac_mem_entry_type_t type) {
    switch (type) {
        case AC_MEM_ENTRY_CORE:
            return "AC_MEM_ENTRY_CORE";
        case AC_MEM_ENTRY_DS:
            return "AC_MEM_ENTRY_DS";
        case AC_MEM_ENTRY_INTERFACE:
            return "AC_MEM_ENTRY_INTERFACE";
        case AC_MEM_ENTRY_RENDERER:
            return "AC_MEM_ENTRY_RENDERER";
        case AC_MEM_ENTRY_VULKAN:
            return "AC_MEM_ENTRY_VULKAN";
        case AC_MEM_ENTRY_COUNT:
            return "AC_MEM_ENTRY_COUNT";
        default:
            return "Unknown";
    }
}

static size_t mem_entry_sizes[AC_MEM_ENTRY_COUNT] = {0};

void ac_mem_map_count_iter_cb(const void* key, const void* value) {
    (void)key;
    ac_mem_entry_t* entry = (ac_mem_entry_t*)value;
    if (entry->state == AC_MEM_ENTRY_STATE_FREED) {
        return;
    }
    mem_entry_sizes[entry->type] += entry->size;
}

void ac_mem_show_usage(void) {
    memset(mem_entry_sizes, 0, sizeof(mem_entry_sizes));
    ac_log_info("Memory usage:\n");
    ac_log_info("Memory map size: %zu\n", ac_map_size(ac_mem_map));
    ac_log_info("Memory map capacity: %zu\n", ac_map_capacity(ac_mem_map));
    ac_log_info("Memory map load factor: %f\n", (float)ac_mem_map->size / (float)ac_mem_map->capacity);
    ac_map_iter(ac_mem_map, ac_mem_map_count_iter_cb);
    for (size_t i = 0; i < AC_MEM_ENTRY_COUNT; i++) {
        ac_log_info("Memory entry type: %s\n", mem_entry_type_str(i));
        ac_log_info("Size: %zu\n", mem_entry_sizes[i]);
    }
    memset(mem_entry_sizes, 0, sizeof(mem_entry_sizes));
}
