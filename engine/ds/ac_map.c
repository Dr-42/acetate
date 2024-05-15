#include "ds/ac_map.h"
#include "core/ac_log.h"
#include "core/ac_mem.h"

#include <stdint.h>
#include <stddef.h>

//-----------------------------------------------------------------------------
// SipHash reference C implementation
//
// Copyright (c) 2012-2016 Jean-Philippe Aumasson
// <jeanphilippe.aumasson@gmail.com>
// Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// default: SipHash-2-4
//-----------------------------------------------------------------------------
static uint64_t SIP64(const uint8_t *in, const size_t inlen, uint64_t seed0,
uint64_t seed1) 
{
#define U8TO64_LE(p) \
	{  (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
		((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
		((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
		((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
	{ U32TO8_LE((p), (uint32_t)((v))); \
		U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
	{ (p)[0] = (uint8_t)((v)); \
		(p)[1] = (uint8_t)((v) >> 8); \
		(p)[2] = (uint8_t)((v) >> 16); \
		(p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
	{ v0 += v1; v1 = ROTL(v1, 13); \
		v1 ^= v0; v0 = ROTL(v0, 32); \
		v2 += v3; v3 = ROTL(v3, 16); \
		v3 ^= v2; \
		v0 += v3; v3 = ROTL(v3, 21); \
		v3 ^= v0; \
		v2 += v1; v1 = ROTL(v1, 17); \
		v1 ^= v2; v2 = ROTL(v2, 32); }
	uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
	uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
	uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
	uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
	uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
	uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
	const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
	for (; in != end; in += 8) {
		uint64_t m = U8TO64_LE(in);
		v3 ^= m;
		SIPROUND; SIPROUND;
		v0 ^= m;
	}
	const int left = inlen & 7;
	uint64_t b = ((uint64_t)inlen) << 56;
	switch (left) {
		case 7: b |= ((uint64_t)in[6]) << 48; /* fall through */
		case 6: b |= ((uint64_t)in[5]) << 40; /* fall through */
		case 5: b |= ((uint64_t)in[4]) << 32; /* fall through */
		case 4: b |= ((uint64_t)in[3]) << 24; /* fall through */
		case 3: b |= ((uint64_t)in[2]) << 16; /* fall through */
		case 2: b |= ((uint64_t)in[1]) << 8; /* fall through */
		case 1: b |= ((uint64_t)in[0]); break;
		case 0: break;
	}
	v3 ^= b;
	SIPROUND; SIPROUND;
	v0 ^= b;
	v2 ^= 0xff;
	SIPROUND; SIPROUND; SIPROUND; SIPROUND;
	b = v0 ^ v1 ^ v2 ^ v3;
	uint64_t out = 0;
	U64TO8_LE((uint8_t*)&out, b);
	return out;
}

//-----------------------------------------------------------------------------

#include <string.h>
#include <math.h>

static float grow_load_factor = 0.75f;
static float shrink_load_factor = 0.25f;
static size_t ac_map_default_capacity = 16;
static size_t ac_map_min_capacity = 8;
static float ac_map_grow_factor = 1.6f;

static float load_factor(ac_map* map) {
	return (float)map->size / (float)map->capacity;
}


static size_t ac_map_hash(const char* key, size_t capacity) {
	return SIP64((const uint8_t*)key, strlen(key), 0, 0) % capacity;
}

static void ac_map_grow(ac_map* map) {
	if (load_factor(map) < grow_load_factor) {
		return;
	}
	size_t new_capacity = (size_t)ceilf(map->capacity * ac_map_grow_factor);
	ac_map_entry* new_entries = (ac_map_entry*)map->map_calloc(new_capacity, sizeof(ac_map_entry), map->entry_type);
	for (size_t i = 0; i < new_capacity; i++) {
		ac_memzero(&new_entries[i], sizeof(ac_map_entry));
	}
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry* entry = &map->entries[i];
		if (entry->key != NULL) {
			size_t hash = ac_map_hash(entry->key, new_capacity);
			while (new_entries[hash].key != NULL) {
				hash = (hash + 1) % new_capacity;
			}
			new_entries[hash] = *entry;
		}
	}
	map->map_free(map->entries);
	map->entries = new_entries;
	map->capacity = new_capacity;
}

static void ac_map_shrink(ac_map* map) {
	if (load_factor(map) > shrink_load_factor && map->capacity > ac_map_min_capacity) {
		size_t new_capacity = (size_t)ceilf(map->capacity / ac_map_grow_factor);
		ac_map_entry* new_entries = (ac_map_entry*)map->map_calloc(new_capacity, sizeof(ac_map_entry), map->entry_type);
		for (size_t i = 0; i < new_capacity; i++) {
			ac_memzero(&new_entries[i], sizeof(ac_map_entry));
		}
		for (size_t i = 0; i < map->capacity; i++) {
			ac_map_entry* entry = &map->entries[i];
			if (entry->key != NULL) {
				size_t hash = ac_map_hash(entry->key, new_capacity);
				while (new_entries[hash].key != NULL) {
					hash = (hash + 1) % new_capacity;
				}
				new_entries[hash] = *entry;
			}
		}
		map->map_free(map->entries);
		map->entries = new_entries;
		map->capacity = new_capacity;
	}
}

ac_map* ac_map_create(size_t elem_size, ac_mem_entry_type_t entry_type) {
	ac_map* map = (ac_map*)ac_malloc(sizeof(ac_map), AC_MEM_ENTRY_DS);

	map->map_malloc = ac_malloc;
	map->map_free = ac_free;
	map->map_calloc = ac_calloc;
	map->elem_size = elem_size;
	map->capacity = ac_map_default_capacity;
	map->size = 0;
	map->entry_type = entry_type;
	map->entries = (ac_map_entry*)map->map_calloc(map->capacity, sizeof(ac_map_entry), entry_type);


	for (size_t i = 0; i < map->capacity; i++) {
		ac_memzero(&map->entries[i], sizeof(ac_map_entry));
	}
	return map;
}

ac_map* ac_map_create_custom(size_t elem_size, ac_mem_entry_type_t entry_type, void* (*map_malloc)(size_t size, ac_mem_entry_type_t type), void (*map_free)(void *ptr), void* (*map_calloc)(size_t nmemb, size_t size, ac_mem_entry_type_t type)) {
	ac_map* map = (ac_map*)map_malloc(sizeof(ac_map), AC_MEM_ENTRY_DS);
	map->elem_size = elem_size;
	map->capacity = ac_map_default_capacity;
	map->size = 0;
	map->entry_type = entry_type;

	map->map_malloc = map_malloc;
	map->map_free = map_free;
	map->map_calloc = map_calloc;

	map->entries = (ac_map_entry*)map->map_calloc(map->capacity, sizeof(ac_map_entry), entry_type);

	for (size_t i = 0; i < map->capacity; i++) {
		ac_memzero(&map->entries[i], sizeof(ac_map_entry));
	}
	return map;
}

void ac_map_destroy(ac_map* map) {
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry* entry = &map->entries[i];
		if (entry->life == AC_MAP_ENTRY_LIFE_ALIVE) {
			map->map_free(entry->key);
			map->map_free(entry->value);
		}
	}
	map->map_free(map->entries);
	map->map_free(map);
}


void ac_map_insert(ac_map* map, const char* key, void* value) {
	ac_map_grow(map);
	size_t hash = ac_map_hash(key, map->capacity);
	do {
		if (map->entries[hash].key == NULL) {
			break;
		}
		if (strcmp(map->entries[hash].key, key) == 0) {
			map->map_free(map->entries[hash].key);
			map->map_free(map->entries[hash].value);
			break;
		}
		hash = (hash + 1) % map->capacity;
	} while (1);
	map->entries[hash].key = map->map_malloc(strlen(key) + 1, map->entry_type);
	strcpy(map->entries[hash].key, key);
	map->entries[hash].value = map->map_malloc(map->elem_size, map->entry_type);
	memcpy(map->entries[hash].value, value, map->elem_size);
	map->entries[hash].life = AC_MAP_ENTRY_LIFE_ALIVE;
	map->size++;
}

void* ac_map_get(ac_map* map, const char* key) {
	size_t hash = ac_map_hash(key, map->capacity);
	while (map->entries[hash].key != NULL) {
		if (strcmp(map->entries[hash].key, key) == 0) {
			return map->entries[hash].value;
		}
		hash = (hash + 1) % map->capacity;
	}
	return NULL;
}

void ac_map_remove(ac_map* map, const char* key) {
	size_t hash = ac_map_hash(key, map->capacity);
	while (map->entries[hash].key != NULL) {
		if (strcmp(map->entries[hash].key, key) == 0) {
			map->map_free(map->entries[hash].key);
			map->map_free(map->entries[hash].value);
			map->entries[hash].key = NULL;
			map->entries[hash].value = NULL;
			map->entries[hash].life = AC_MAP_ENTRY_LIFE_TOMBSTONE;
			map->size--;
			break;
		}
		hash = (hash + 1) % map->capacity;
	}
	ac_map_shrink(map);
}


#include <stdio.h>

void ac_map_print(ac_map* map) {
	ac_log_debug("|%10s|%3s|%6s|\n", "KEY", "VAL", "LIFE");
	ac_log_debug("|----------|---|------|\n");
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry entry = map->entries[i];
		char* life_str;
		switch (entry.life) {
			case AC_MAP_ENTRY_LIFE_EMPTY:
				life_str = "EMPTY";
			break;
			case AC_MAP_ENTRY_LIFE_ALIVE:
				life_str = "ALIVE";
			break;
			case AC_MAP_ENTRY_LIFE_TOMBSTONE:
				life_str = "TOMB";
			break;
		}
		if (entry.key == NULL) {
			ac_log_debug("|%10s|%3s|%6s|\n", "NULL", "n/a", life_str);
		} else {
			ac_log_debug("|%10s|%3d|%6s|\n", entry.key, *(int*)entry.value, life_str);
		}
	}
	ac_log_debug("\n");
}

