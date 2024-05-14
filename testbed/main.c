#include <core/ac_log.h>
#include <core/ac_mem.h>
#include <core/ac_trace.h>

int main() {
	int* a = ac_malloc(sizeof(int), AC_MEM_ENTRY_CORE);
	*a = 5;
	int* b = ac_calloc(20, sizeof(int), AC_MEM_ENTRY_CORE);
	for (int i = 0; i < 20; i++) {
		b[i] = i;
	}
	for (int i = 0; i < 20; i++) {
		ac_log_info("%d\n", b[i]);
	}
	b = ac_reallocarray(b, 40, sizeof(int), AC_MEM_ENTRY_CORE);
	for (int i = 20; i < 40; i++) {
		b[i] = i;
	}
	ac_free(a);
	ac_mem_exit();
	return 0;
}
