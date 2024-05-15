#ifndef AC_DS_DMAP_H
#define AC_DS_DMAP_H

#include "core/ac_mem.h"

typedef enum ac_map_entry_life {
	AC_MAP_ENTRY_LIFE_EMPTY = 0,
	AC_MAP_ENTRY_LIFE_ALIVE = 1,
	AC_MAP_ENTRY_LIFE_TOMBSTONE = 2,
} ac_map_entry_life;

typedef struct ac_map_entry {
	char* key;
	void* value;
	ac_map_entry_life life;
} ac_map_entry;

typedef struct ac_map {
	ac_mem_entry_type_t entry_type;
	ac_map_entry* entries;
	size_t elem_size;
	size_t capacity;
	size_t size;
	void* (*map_malloc)(size_t size, ac_mem_entry_type_t type);
	void (*map_free)(void *ptr);
	void* (*map_calloc)(size_t nmemb, size_t size, ac_mem_entry_type_t type);
	int (*map_cmp)(void* element1, void* element2);
	size_t (*map_hash)(void* key, size_t capacity);
} ac_map;

ac_map* ac_map_create(size_t elem_size, ac_mem_entry_type_t entry_type);

ac_map* ac_map_create_custom(
	size_t elem_size,
	ac_mem_entry_type_t entry_type,
	void* (*map_malloc)(size_t size, ac_mem_entry_type_t type),
	void (*map_free)(void *ptr),
	void* (*map_calloc)(size_t nmemb, size_t size, ac_mem_entry_type_t type),
    int (*cmp)(void* element1, void* element2),
	size_t (*hash)(void* key, size_t capacity)
);

void ac_map_destroy(ac_map* map);

void ac_map_insert(ac_map* map, const char* key, void* value);
void* ac_map_get(ac_map* map, const char* key);
void ac_map_remove(ac_map* map, const char* key);

void ac_map_print(ac_map* map);

#endif // AC_DS_DMAP_H
