#include <core/ac_mem.h>
#include <core/ac_trace.h>
#include <core/ac_log.h>
#include <ds/ac_map.h>

#include <stdlib.h>
#include <signal.h>

void segfaulter(int sig) {
	ac_log_fatal("Segmentation fault caught : %d\n", sig);
	ac_print_trace(2);
	exit(1);
}

int main() {
	printf("Hello, World!\n");
	signal(SIGSEGV, segfaulter);
	ac_map* map = ac_map_create(sizeof(int), AC_MEM_ENTRY_CORE);
	ac_map_print(map);
	
	int vals[17] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
					11, 12, 13, 14, 15, 100, 123};

	ac_map_insert(map, "one", &vals[0]);
	ac_map_insert(map, "two", &vals[1]);
	ac_map_insert(map, "three", &vals[2]);

	ac_map_print(map);

	ac_map_remove(map, "two");
	ac_map_print(map);

	ac_map_insert(map, "four", &vals[3]);
	ac_map_insert(map, "five", &vals[4]);
	ac_map_insert(map, "six", &vals[5]);
	ac_map_insert(map, "seven", &vals[6]);
	ac_map_insert(map, "eight", &vals[7]);
	ac_map_insert(map, "nine", &vals[8]);
	ac_map_insert(map, "ten", &vals[9]);
	ac_map_insert(map, "eleven", &vals[10]);
	ac_map_insert(map, "twelve", &vals[11]);
	ac_map_insert(map, "thirteen", &vals[12]);
	ac_map_insert(map, "fourteen", &vals[13]);
	ac_map_insert(map, "fifteen", &vals[14]);

	ac_map_print(map);

	ac_map_insert(map, "one", &vals[15]);
	ac_map_insert(map, "twelve", &vals[16]);

	ac_map_print(map);

	ac_map_destroy(map);
	ac_mem_exit();
	return 0;
}
