#include "ds/ac_string.h"

#include "core/ac_mem.h"
#include "core/ac_log.h"

#include <string.h>

ac_string_t ac_string_new(ac_mem_entry_type_t mem_type) {
    ac_string_t str = {0};
    str.size = 0;
    str.capacity = 1;
    str.data = ac_malloc(str.capacity, mem_type);
    str.mem_type = mem_type;
    if (str.data == NULL) {
        ac_log_fatal_exit("Failed to allocate memory for string\n");
    }
    str.data[0] = '\0';
    return str;
}

ac_string_t ac_string_new_with_capacity(size_t capacity, ac_mem_entry_type_t entry_type) {
    ac_string_t str = {0};
    str.size = 0;
    str.capacity = capacity;
    str.data = ac_malloc(str.capacity, entry_type);
    str.mem_type = entry_type;
    if (str.data == NULL) {
        ac_log_fatal_exit("Failed to allocate memory for string\n");
    }
    str.data[0] = '\0';
    return str;
}

ac_string_t ac_string_new_from_str(const char* str, ac_mem_entry_type_t mem_type) {
    ac_string_t new_str = {0};
    new_str.size = strlen(str);
    new_str.capacity = new_str.size + 1;
    new_str.data = ac_malloc(new_str.capacity, mem_type);
    new_str.mem_type = mem_type;
    if (new_str.data == NULL) {
        ac_log_fatal_exit("Failed to allocate memory for string\n");
    }
    strcpy(new_str.data, str);
    return new_str;
}

void ac_string_free(ac_string_t* str) {
    if (str == NULL) {
        return;
    }
    ac_free(str->data);
}

void ac_string_append(ac_string_t* str, const char* suffix) {
    size_t suffix_len = strlen(suffix);
    if (str->size + suffix_len + 1 > str->capacity) {
        str->capacity = str->size + suffix_len + 1;
        str->data = ac_realloc(str->data, str->capacity, str->mem_type);
    }
    strcpy(str->data + str->size, suffix);
    str->size += suffix_len;
}

void ac_string_append_char(ac_string_t* str, char c) {
    if (str->size + 2 > str->capacity) {
        str->capacity = str->size + 2;
        str->data = ac_realloc(str->data, str->capacity, str->mem_type);
    }
    str->data[str->size] = c;
    str->data[str->size + 1] = '\0';
    str->size++;
}
