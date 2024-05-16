#ifndef AC_CORE_MEM_H
#define AC_CORE_MEM_H

/**
 * @file ac_mem.h
 * @brief Memory management functions.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Custom memory management functions.
// These functions can be set to use custom memory management functions.

/**
 * Custom malloc function.
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block.
 * @see ac_malloc
 */
typedef void *(*ac_malloc_t)(size_t size);

/**
 * Custom free function.
 * @param ptr A pointer to the memory block to free.
 * @see ac_free
 */
typedef void (*ac_free_t)(void *ptr);

/**
 * Custom calloc function.
 * @param nmemb The number of elements to allocate.
 * @param size The size of each element.
 * @return A pointer to the allocated memory block.
 */
typedef void *(*ac_calloc_t)(size_t nmemb, size_t size);

/**
 * Custom realloc function.
 * @param ptr A pointer to the memory block to reallocate.
 * @param size The new size of the memory block.
 * @return A pointer to the reallocated memory block.
 */
typedef void *(*ac_realloc_t)(void *ptr, size_t size);

/**
 * Custom reallocarray function.
 * @param ptr A pointer to the memory block to reallocate.
 * @param nmemb The new number of elements.
 * @param size The size of each element.
 * @return A pointer to the reallocated memory block.
 */
typedef void *(*ac_reallocarray_t)(void *ptr, size_t nmemb, size_t size);

/**
 * Set custom malloc
 * @param malloc The custom malloc function.
 * @see ac_malloc
 */
void set_custom_malloc(ac_malloc_t malloc);

/**
 * Set custom free
 * @param free The custom free function.
 * @see ac_free
 */
void set_custom_free(ac_free_t free);

/**
 * Set custom calloc
 * @param calloc The custom calloc function.
 * @see ac_calloc
 */
void set_custom_calloc(ac_calloc_t calloc);

/**
 * Set custom realloc
 * @param realloc The custom realloc function.
 * @see ac_realloc
 */
void set_custom_realloc(ac_realloc_t realloc);

/**
 * Set custom reallocarray
 * @param reallocarray The custom reallocarray function.
 * @see ac_reallocarray
 */
void set_custom_reallocarray(ac_reallocarray_t reallocarray);

/**
 * Enable/Disable memory tracking.
 * Memory tracking is enabled by default.
 * @param enabled Whether to enable memory tracking.
 */
void ac_mem_track_enabled(bool enabled);

/**
 * State of a memory entry.
 * @see ac_mem_entry_t
 */
typedef enum ac_mem_entry_state_t {
    AC_MEM_ENTRY_STATE_ALLOCATED,
    AC_MEM_ENTRY_STATE_FREED,
    AC_MEM_ENTRY_STATE_REALLOCATED
} ac_mem_entry_state_t;

/**
 * Type of a memory entry.
 * @see ac_mem_entry_t
 */
typedef enum ac_mem_entry_type_t {
    AC_MEM_ENTRY_CORE,
    AC_MEM_ENTRY_DS,
    AC_MEM_ENTRY_INTERFACE,
    AC_MEM_ENTRY_RENDERER,
    AC_MEM_ENTRY_VULKAN,
} ac_mem_entry_type_t;

/**
 * The struct holding the details of a memory entry.
 * You won't need to use this struct directly.
 * @brief A memory entry.
 * @see ac_mem_entry_state_t
 * @see ac_mem_entry_type_t
 * @see ac_mem_entry_t
 */
typedef struct ac_mem_entry_t {
    /**
     * The pointer to the memory block.
     */
    void *ptr;
    /**
     * The size of the memory block.
     */
    size_t size;
    /**
     * The state of the memory entry.
     * @see ac_mem_entry_state_t
     */
    ac_mem_entry_state_t state;
    /**
     * The type of the memory entry.
     * @see ac_mem_entry_type_t
     */
    ac_mem_entry_type_t type;
    /**
     * The stack trace of the allocation.
     */
    void **alloc_trace;
    /**
     * The size of the stack trace.
     */
    int32_t alloc_trace_size;
    /**
     * The stack trace of the free.
     */
    void **free_trace;
    /**
     * The size of the stack trace.
     */
    int32_t free_trace_size;
    /**
     * The stack traces of the reallocation, its a darray
     */
    void ***realloc_traces;
    /**
     * size of the realloc_traces darray
     */
    int32_t realloc_traces_size;
    /**
     * The capacity of the realloc_traces darray
     */
    int32_t realloc_traces_capacity;
    /**
     * The sizes of the reallocated memory blocks.
     */
    int32_t *realloc_trace_sizes;
} ac_mem_entry_t;

/**
 * Malloc function.
 * @param size The size of the memory block to allocate.
 * @param type The type of the memory block.
 * @see ac_mem_entry_type_t
 * @return A pointer to the allocated memory block.
 * @see ac_malloc_t
 */
void *ac_malloc(size_t size, ac_mem_entry_type_t type);

/**
 * Free function.
 * @param ptr A pointer to the memory block to free.
 * @see ac_free_t
 */
void ac_free(void *ptr);

/**
 * Calloc function.
 * @param nmemb The number of elements to allocate.
 * @param size The size of each element.
 * @param type The type of the memory block.
 * @see ac_mem_entry_type_t
 * @return A pointer to the allocated memory block.
 * @see ac_calloc_t
 */
void *ac_calloc(size_t nmemb, size_t size, ac_mem_entry_type_t type);

/**
 * Realloc function.
 * @param ptr A pointer to the memory block to reallocate.
 * @param size The new size of the memory block.
 * @param type The type of the memory block.
 * @see ac_mem_entry_type_t
 * @return A pointer to the reallocated memory block.
 * @see ac_realloc_t
 */
void *ac_realloc(void *ptr, size_t size, ac_mem_entry_type_t type);

/**
 * Reallocarray function.
 * @param ptr A pointer to the memory block to reallocate.
 * @param nmemb The new number of elements.
 * @param size The size of each element.
 * @param type The type of the memory block.
 * @see ac_mem_entry_type_t
 * @return A pointer to the reallocated memory block.
 * @see ac_reallocarray_t
 */
void *ac_reallocarray(void *ptr, size_t nmemb, size_t size, ac_mem_entry_type_t type);

/**
 * Memcpy function.
 * @param dest The destination memory block.
 * @param src The source memory block.
 * @param n The number of bytes to copy.
 */
void ac_memcpy(void *dest, const void *src, size_t n);

/**
 * Memmove function.
 * @param dest The destination memory block.
 * @param src The source memory block.
 * @param n The number of bytes to copy.
 */
void ac_memmove(void *dest, const void *src, size_t n);

/**
 * Memset function.
 * @param s The memory block.
 * @param c The byte to set.
 * @param n The number of bytes to set.
 */
void ac_memset(void *s, int c, size_t n);

/**
 * Memzero function.
 * @param s The memory block.
 * @param n The number of bytes to set to zero.
 */
void ac_memzero(void *s, size_t n);

/**
 * Exit function for memory management.
 * This function should be called at the end of the program.
 * It will print the memory leaks if there are any.
 */
void ac_mem_exit(void);

#endif  // AC_CORE_MEM_H
