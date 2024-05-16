#ifndef AC_DS_STRING_H
#define AC_DS_STRING_H

#include <stddef.h>
#include "core/ac_mem.h"

typedef struct ac_string_t {
    size_t size;
    size_t capacity;
    char* data;
    ac_mem_entry_type_t mem_type;
} ac_string_t;

ac_string_t ac_string_new(ac_mem_entry_type_t entry_type);
ac_string_t ac_string_new_with_capacity(size_t capacity, ac_mem_entry_type_t entry_type);
ac_string_t ac_string_new_from_str(const char* str, ac_mem_entry_type_t entry_type);

void ac_string_free(ac_string_t* str);

void ac_string_append(ac_string_t* str, const char* suffix);
void ac_string_append_char(ac_string_t* str, char c);

#endif  // AC_DS_STRING_H
