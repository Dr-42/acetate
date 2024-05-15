#include "ds/ac_map.h"
#include "core/ac_log.h"
#include "core/ac_mem.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

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
static uint64_t SIP64(const uint8_t *in, const size_t inlen, uint64_t seed0, uint64_t seed1) {
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
#include <stdlib.h>

static float grow_load_factor = 0.75f;
static float shrink_load_factor = 0.25f;
static size_t ac_map_default_capacity = 16;
static size_t ac_map_min_capacity = 8;
static float ac_map_grow_factor = 1.6f;

static float load_factor(ac_map_t* map) {
	return (float)map->size / (float)map->capacity;
}

static void ac_map_grow(ac_map_t* map) {
	if (map->capacity >= SIZE_MAX / ac_map_grow_factor) {
		ac_log_fatal_exit("Map capacity overflow");
		return;
	}
	float map_load_factor = load_factor(map);
	if (map_load_factor < grow_load_factor) {
		return;
	}
	size_t new_capacity = (size_t)ceilf((float)map->capacity * ac_map_grow_factor);
	ac_map_entry_t* new_entries = (ac_map_entry_t*)map->mem_ops.map_calloc(new_capacity, sizeof(ac_map_entry_t), map->entry_type);
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			size_t hash = entry.hash;
			size_t index = hash % new_capacity;
			ac_map_entry_t new_entry = new_entries[index];
			while (new_entry.key != NULL) {
				index = (index + 1) % new_capacity;
				new_entry = new_entries[index];
			}
			new_entries[index] = entry;
		}
	}
	map->mem_ops.map_free(map->entries);
	map->entries = new_entries;
	map->capacity = new_capacity;
}

static void ac_map_shrink(ac_map_t* map) {
	if (map->capacity <= ac_map_min_capacity) {
		return;
	}
	float map_load_factor = load_factor(map);
	if (map_load_factor > shrink_load_factor) {
		return;
	}
	size_t new_capacity = (size_t)ceilf((float)map->capacity / ac_map_grow_factor);
	ac_map_entry_t* new_entries = (ac_map_entry_t*)map->mem_ops.map_calloc(new_capacity, sizeof(ac_map_entry_t), map->entry_type);
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			size_t hash = entry.hash;
			size_t index = hash % new_capacity;
			ac_map_entry_t new_entry = new_entries[index];
			while (new_entry.key != NULL) {
				index = (index + 1) % new_capacity;
				new_entry = new_entries[index];
			}
			new_entries[index] = entry;
		}
	}
	map->mem_ops.map_free(map->entries);
	map->entries = new_entries;
	map->capacity = new_capacity;
}

// String map setup
static int ac_map_str_cmp(void* element1, void* element2) {
	return strcmp((const char*)element1, (const char*)element2);
}

static size_t ac_map_str_hash(void* key) {
	const uint8_t* k = key;
	const size_t kn = strlen(key);
	srand(time(NULL));
	uint64_t seed0 = (uint64_t)(rand());
	uint64_t seed1 = (uint64_t)(rand());
	return SIP64(k, kn, seed0, seed1);
}

static void* ac_map_str_cpy(void* key) {
	void* new_key = ac_malloc(strlen((const char*)key) + 1, AC_MEM_ENTRY_DS);
	strncpy((char*)new_key, (const char*)key, strlen((const char*)key) + 1);
	return new_key;
}

static void ac_map_str_free(void* key) {
	ac_free(key);
}

static int ac_map_str_display(void* key, char* buffer, size_t size) {
	return snprintf(buffer, size, "%s", (const char*)key);
}

ac_map_t* ac_map_new_strmap(ac_map_value_ops_t value_ops, ac_mem_entry_type_t entry_type) {
	ac_map_mem_ops_t mem_ops = {
		.map_malloc = ac_malloc,
		.map_free = ac_free,
		.map_calloc = ac_calloc
	};
	ac_map_key_ops_t key_ops = {
		.cmp = ac_map_str_cmp,
		.hash = ac_map_str_hash,
		.copy = ac_map_str_cpy,
		.free = ac_map_str_free,
		.display = ac_map_str_display
	};
	return ac_map_create_custom(ac_map_default_capacity, entry_type, mem_ops, key_ops, value_ops);
}

ac_map_t* ac_map_create_custom(
	size_t capacity,
	ac_mem_entry_type_t entry_type,
	ac_map_mem_ops_t mem_ops,
	ac_map_key_ops_t key_ops,
	ac_map_value_ops_t value_ops
) {
	ac_map_t* map = (ac_map_t*)mem_ops.map_malloc(sizeof(ac_map_t), entry_type);
	map->entry_type = entry_type;
	map->capacity = capacity;
	map->size = 0;
	map->mem_ops = mem_ops;
	map->key_ops = key_ops;
	map->value_ops = value_ops;
	map->entries = (ac_map_entry_t*)mem_ops.map_calloc(capacity, sizeof(ac_map_entry_t), entry_type);
	return map;
}

void ac_map_destroy(ac_map_t* map) {
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			map->key_ops.free(entry.key);
			map->value_ops.free(entry.value);
		}
	}
	map->mem_ops.map_free(map->entries);
	map->mem_ops.map_free(map);
}

void ac_map_clear(ac_map_t* map) {
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			map->key_ops.free(entry.key);
			map->value_ops.free(entry.value);
		}
	}
	map->size = 0;
	memset(map->entries, 0, map->capacity * sizeof(ac_map_entry_t));
}

void* ac_map_get(ac_map_t* map, void* key) {
	size_t hash = map->key_ops.hash(key);
	size_t index = hash % map->capacity;
	ac_map_entry_t entry = map->entries[index];
	if (entry.key == NULL) {
		return NULL;
	}
	do {
		if (entry.key != NULL && map->key_ops.cmp(entry.key, key) == 0) {
			return entry.value;
		}
		index = (index + 1) % map->capacity;
		entry = map->entries[index];
	} while (entry.key != NULL);
	return NULL;
}

void ac_map_set(ac_map_t* map, void* key, void* value) {
	ac_map_grow(map);
	size_t hash = map->key_ops.hash(key);
	size_t index = hash % map->capacity;
	ac_map_entry_t entry = map->entries[index];
	if (entry.key == NULL) {
		map->entries[index].key = map->key_ops.copy(key);
		map->entries[index].value = map->value_ops.copy(value);
		map->entries[index].hash = hash;
		map->size++;
		return;
	}
	do {
		if (map->key_ops.cmp(entry.key, key) == 0) {
			map->value_ops.free(entry.value);
			map->entries[index].value = map->value_ops.copy(value);
			return;
		}
		index = (index + 1) % map->capacity;
		entry = map->entries[index];
	} while (entry.key != NULL);
	map->entries[index].key = map->key_ops.copy(key);
	map->entries[index].value = map->value_ops.copy(value);
	map->entries[index].hash = hash;
	map->size++;
}

void ac_map_remove(ac_map_t* map, void* key) {
	size_t hash = map->key_ops.hash(key);
	size_t index = hash % map->capacity;
	ac_map_entry_t entry = map->entries[index];
	if (entry.key == NULL) {
		return;
	}
	do {
		if (map->key_ops.cmp(entry.key, key) == 0) {
			map->key_ops.free(entry.key);
			map->value_ops.free(entry.value);
			map->entries[index].key = NULL;
			map->entries[index].value = NULL;
			map->size--;
			ac_map_shrink(map);
			return;
		}
		index = (index + 1) % map->capacity;
		entry = map->entries[index];
	} while (entry.key != NULL);
}

size_t ac_map_size(ac_map_t* map) {
	return map->size;
}

size_t ac_map_capacity(ac_map_t* map) {
	return map->capacity;
}

void ac_map_print(ac_map_t* map) {
	ac_log_info("Map size: %zu, Map capacity: %zu\n", map->size, map->capacity);
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			char key_buffer[256];
			char value_buffer[256];
			map->key_ops.display(entry.key, key_buffer, sizeof(key_buffer));
			map->value_ops.display(entry.value, value_buffer, sizeof(value_buffer));
			ac_log_info("Key: %s, Value: %s\n", key_buffer, value_buffer);
		}
	}
	ac_log_info("\n");
}

void ac_map_iter(ac_map_t* map, void (*callback)(void* key, void* value)) {
	for (size_t i = 0; i < map->capacity; i++) {
		ac_map_entry_t entry = map->entries[i];
		if (entry.key != NULL) {
			callback(entry.key, entry.value);
		}
	}
}
