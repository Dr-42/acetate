#ifndef AC_DS_DMAP_H
#define AC_DS_DMAP_H

/**
 * @file ac_map.h
 * @brief Hash map interface.
 */

#include "core/ac_mem.h"

/**
 * @brief Hash map entry life.
 */
typedef enum ac_map_entry_life_t {
    AC_MAP_ENTRY_LIFE_EMPTY = 0,
    AC_MAP_ENTRY_LIFE_ALIVE = 1,
    AC_MAP_ENTRY_LIFE_TOMBSTONE = 2,
} ac_map_entry_life_t;

/**
 * Hash map entry.
 * You don't need to use this structure directly.
 * @brief Hash map entry.
 */
typedef struct ac_map_entry_t {
    /**
     * The key.
     */
    void* key;
    /**
     * The value.
     */
    void* value;
    /**
     * The hash of the key.
     * Prevents rehashing the key.
     */
    uint64_t hash;
    /**
     * The life of the entry.
     * @see ac_map_entry_life_t
     */
    ac_map_entry_life_t life;
} ac_map_entry_t;

/**
 * Hash map memory operations collection.
 * Needed for custom memory operations.
 * @brief Hash map memory operations.
 * @see ac_map_new_custom
 */
typedef struct ac_map_mem_ops_t {
    /**
     * Malloc function.
     * @param size The size of the memory block.
     * @param type The memory type.
     * @return The pointer to the memory block.
     * @see ac_mem_entry_type_t
     * @see ac_mem_malloc
     */
    void* (*map_malloc)(size_t size, ac_mem_entry_type_t type);
    /**
     * Free function.
     * @param ptr The pointer to the memory block.
     * @see ac_mem_free
     */
    void (*map_free)(void* ptr);
    /**
     * Realloc function.
     * @param ptr The pointer to the memory block.
     * @param size The new size of the memory block.
     * @param type The memory type.
     * @return The pointer to the memory block.
     * @see ac_mem_realloc
     * @see ac_mem_entry_type_t
     */
    void* (*map_calloc)(size_t nmemb, size_t size, ac_mem_entry_type_t type);
} ac_map_mem_ops_t;

/**
 * Hash map key operations collection.
 * @brief Hash map key operations.
 */
typedef struct ac_map_key_ops_t {
    /**
     * Compare function.
     * @param element1 The first element.
     * @param element2 The second element.
     * @return The comparison result. 0 if equal, negative if element1 is less
     * than element2, positive if element1 is greater than element2.
     * @see ac_map_new_custom
     */
    int (*cmp)(const void* element1, const void* element2);
    /**
     * Hash function.
     * @param key The key.
     * @return The hash of the key.
     * @see ac_map_new_custom
     */
    size_t (*hash)(const void* key);
    /**
     * Copy function.
     * @param key The key.
     * @return The copy of the key.
     * @see ac_map_new_custom
     */
    void* (*copy)(const void* key, ac_mem_entry_type_t type);
    /**
     * Free function.
     * @param key The key.
     * @see ac_map_new_custom
     */
    void (*free)(void* key);
    /**
     * Display function.
     * @param key The key.
     * @param buffer The buffer to print the key to.
     * @param size The size of the buffer.
     * @return The number of characters printed.
     * @see ac_map_new_custom
     */
    int (*display)(const void* key, char* buffer, size_t size);
} ac_map_key_ops_t;

/**
 * Hash map value operations collection.
 * @brief Hash map value operations.
 */
typedef struct ac_map_value_ops_t {
    /**
     * Copy function.
     * @param value The value.
     * @return The copy of the value.
     * @see ac_map_new_custom
     * @see ac_map_new_strmap
     */
    void* (*copy)(const void* value, ac_mem_entry_type_t type);
    /**
     * Free function.
     * @param value The value.
     * @see ac_map_new_custom
     * @see ac_map_new_strmap
     */
    void (*free)(void* value);
    /**
     * Display function.
     * @param value The value.
     * @param buffer The buffer to print the value to.
     * @param size The size of the buffer.
     * @return The number of characters printed.
     * @see ac_map_new_custom
     * @see ac_map_new_strmap
     */
    int (*display)(const void* value, char* buffer, size_t size);
} ac_map_value_ops_t;

/**
 * Hash map.
 * @brief Hash map.
 * You don't need to use this structure directly.
 */
typedef struct ac_map_t {
    /**
     * @brief The type of the entry.
     * @see ac_mem_entry_type_t
     */
    ac_mem_entry_type_t entry_type;
    /**
     * @brief The capacity of the map.
     * @see ac_map_capacity
     */
    size_t capacity;
    /**
     * @brief The size of the map.
     * @see ac_map_size
     */
    size_t size;
    /**
     * @brief The entries of the map.
     */
    ac_map_entry_t* entries;
    /**
     * @brief The memory operations.
     * @see ac_map_mem_ops_t
     */
    ac_map_mem_ops_t mem_ops;
    /**
     * @brief The key operations.
     * @see ac_map_key_ops_t
     */
    ac_map_key_ops_t key_ops;
    /**
     * @brief The value operations.
     * @see ac_map_value_ops_t
     */
    ac_map_value_ops_t value_ops;
} ac_map_t;

/**
 * @brief Create a new hash map with string keys.
 * @param value_ops The value operations.
 * @param entry_type The memory entry type.
 * @return The pointer to the new hash map.
 */
ac_map_t* ac_map_new_strmap(ac_map_value_ops_t value_ops, ac_mem_entry_type_t entry_type);

/**
 * @brief Create a new hash map with custom parameters.
 * @param capacity The initial capacity of the map.
 * @param entry_type The memory entry type.
 * @param mem_ops The memory operations.
 * @param key_ops The key operations.
 * @param value_ops The value operations.
 * @return The pointer to the new hash map.
 */
ac_map_t* ac_map_new_custom(size_t capacity, ac_mem_entry_type_t entry_type, ac_map_mem_ops_t mem_ops, ac_map_key_ops_t key_ops,
                            ac_map_value_ops_t value_ops);

/**
 * @brief Destroy the hash map.
 * @param map The hash map to destroy.
 */
void ac_map_destroy(ac_map_t* map);

/**
 * @brief Clear the hash map.
 * @param map The hash map to clear.
 */
void ac_map_clear(ac_map_t* map);

/**
 * @brief Get the value of the given key.
 * @param map The hash map.
 * @param key The key.
 * @return The value of the key.
 * Returns NULL if the key is not found.
 */
void* ac_map_get(ac_map_t* map, void* key);

/**
 * @brief Set the value of the given key.
 * @param map The hash map.
 * @param key The key.
 * @param value The value.
 */
void ac_map_set(ac_map_t* map, void* key, void* value);

/**
 * @brief Remove the key from the hash map.
 * @param map The hash map.
 * @param key The key.
 */
void ac_map_remove(ac_map_t* map, void* key);

/**
 * @brief Get the size of the hash map.
 * @param map The hash map.
 */
size_t ac_map_size(ac_map_t* map);

/**
 * @brief Get the capacity of the hash map.
 * @param map The hash map.
 */
size_t ac_map_capacity(ac_map_t* map);

/**
 * @brief Print the hash map.
 * @param map The hash map.
 * @see ac_map_key_ops_t::display
 * @see ac_map_value_ops_t::display
 */
void ac_map_print(ac_map_t* map);

/**
 * @brief Iterate over the hash map.
 * @param map The hash map.
 * @param callback The callback function.
 * The callback function should have the following signature:
 * void callback(void* key, void* value);
 */
void ac_map_iter(ac_map_t* map, void (*callback)(const void* key, const void* value));

/**
 * SIP hash function.
 * Provided so that the hash function can be used in other modules.
 * @param in The input data.
 * @param inlen The length of the input data.
 * @param seed0 The first seed.
 * @param seed1 The second seed.
 * @return The hash of the input data.
 */
uint64_t SIP64(const uint8_t* in, const size_t inlen, uint64_t seed0, uint64_t seed1);
#endif  // AC_DS_DMAP_H
