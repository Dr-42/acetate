#ifndef AC_DS_DMAP_H
#define AC_DS_DMAP_H

#include "core/ac_mem.h"

typedef enum ac_map_entry_life_t {
	AC_MAP_ENTRY_LIFE_EMPTY = 0,
	AC_MAP_ENTRY_LIFE_ALIVE = 1,
	AC_MAP_ENTRY_LIFE_TOMBSTONE = 2,
} ac_map_entry_life_t;

typedef struct ac_map_entry_t {
	void* key;
	void* value;
	uint64_t hash;
	ac_map_entry_life_t life;
} ac_map_entry_t;

typedef struct ac_map_mem_ops_t {
	void* (*map_malloc)(size_t size, ac_mem_entry_type_t type);
	void (*map_free)(void *ptr);
	void* (*map_calloc)(size_t nmemb, size_t size, ac_mem_entry_type_t type);
} ac_map_mem_ops_t;

typedef struct ac_map_key_ops_t {
	int (*cmp)(void* element1, void* element2);
	size_t (*hash)(void* key);
	void* (*copy)(void* key);
	void (*free)(void* key);
	int (*display)(void* key, char* buffer, size_t size);
} ac_map_key_ops_t;

typedef struct ac_map_value_ops_t {
	void* (*copy)(void* value);
	void (*free)(void* value);
	int (*display)(void* value, char* buffer, size_t size);
} ac_map_value_ops_t;

typedef struct ac_map_t {
	ac_mem_entry_type_t entry_type;
	size_t capacity;
	size_t size;
	ac_map_entry_t* entries;
	ac_map_mem_ops_t mem_ops;
	ac_map_key_ops_t key_ops;
	ac_map_value_ops_t value_ops;
} ac_map_t;

ac_map_t* ac_map_new_strmap(ac_map_value_ops_t value_ops, ac_mem_entry_type_t entry_type);

ac_map_t* ac_map_create_custom(
	size_t capacity,
	ac_mem_entry_type_t entry_type,
	ac_map_mem_ops_t mem_ops,
	ac_map_key_ops_t key_ops,
	ac_map_value_ops_t value_ops
);

void ac_map_destroy(ac_map_t* map);
void ac_map_clear(ac_map_t* map);

void* ac_map_get(ac_map_t* map, void* key);
void ac_map_set(ac_map_t* map, void* key, void* value);
void ac_map_remove(ac_map_t* map, void* key);

size_t ac_map_size(ac_map_t* map);
size_t ac_map_capacity(ac_map_t* map);

void ac_map_print(ac_map_t* map);
void ac_map_iter(ac_map_t* map, void (*callback)(void* key, void* value));

#endif // AC_DS_DMAP_H
