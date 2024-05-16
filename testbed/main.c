#include <core/ac_log.h>
#include <core/ac_mem.h>
#include <core/ac_trace.h>
#include <ds/ac_map.h>
#include <signal.h>
#include <stdlib.h>

void segfaulter(int sig) {
    ac_log_fatal("Segmentation fault caught : %d\n", sig);
    ac_print_trace(2);
    exit(1);
}

void* int_copy(void* value, ac_mem_entry_type_t entry_type) {
    int* val = (int*)value;
    int* new_val = (int*)ac_malloc(sizeof(int), entry_type);
    *new_val = *val;
    return new_val;
}

void int_free(void* value) { ac_free(value); }

int int_display(void* value, char* buffer, size_t size) {
    int* val = (int*)value;
    return snprintf(buffer, size, "%d", *val);
}

int main() {
    signal(SIGSEGV, segfaulter);
    ac_map_value_ops_t value_ops = {
        .copy = int_copy, .free = int_free, .display = int_display};

    ac_map_t* map = ac_map_new_strmap(value_ops, AC_MEM_ENTRY_CORE);
    ac_map_print(map);

    ac_map_print(map);

    int nums[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                  11, 12, 13, 14, 15, 16, 17, 20, 30, 80};

    ac_map_set(map, "one", &nums[0]);
    ac_map_set(map, "two", &nums[1]);
    ac_map_set(map, "three", &nums[2]);

    ac_map_print(map);

    ac_map_set(map, "one", &nums[9]);
    ac_map_set(map, "four", &nums[3]);
    ac_map_set(map, "five", &nums[4]);
    ac_map_set(map, "six", &nums[5]);
    ac_map_set(map, "seven", &nums[6]);
    ac_map_set(map, "eight", &nums[7]);
    ac_map_set(map, "nine", &nums[8]);
    ac_map_set(map, "ten", &nums[9]);
    ac_map_set(map, "eleven", &nums[10]);
    ac_map_set(map, "twelve", &nums[11]);
    ac_map_set(map, "thirteen", &nums[12]);
    ac_map_set(map, "fourteen", &nums[13]);
    ac_map_set(map, "fifteen", &nums[14]);
    ac_map_set(map, "sixteen", &nums[15]);

    ac_map_print(map);

    ac_map_remove(map, "three");
    ac_map_remove(map, "eight");

    ac_map_print(map);

    ac_map_set(map, "seventeen", &nums[16]);
    ac_map_set(map, "two", &nums[18]);
    ac_map_set(map, "three", &nums[19]);
    ac_map_set(map, "eight", &nums[20]);

    ac_map_print(map);

    ac_map_destroy(map);

    ac_mem_exit();
    return 0;
}
