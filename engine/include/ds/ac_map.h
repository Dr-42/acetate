#ifndef AC_DS_DMAP_H
#define AC_DS_DMAP_H

#include "core/ac_mem.h"

typedef struct ac_map ac_map;

ac_map* ac_map_create(size_t elem_size, ac_mem_entry_type_t entry_type);
void ac_map_destroy(ac_map* map);

void ac_map_insert(ac_map* map, const char* key, void* value);
void* ac_map_get(ac_map* map, const char* key);
void ac_map_remove(ac_map* map, const char* key);

void ac_map_print(ac_map* map);

#endif // AC_DS_DMAP_H
