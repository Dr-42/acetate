#ifndef AC_CORE_TRACE_H
#define AC_CORE_TRACE_H

#include <stdio.h>

#define print_trace() ac_print_trace(3);
#define fprint_trace(fp) ac_fprint_trace(fp, 2);
#define sprint_trace(buffer) ac_sprint_trace(buffer, 1);

int ac_get_intermediate_trace(void** stack, size_t size);
int ac_sprint_intermediate_trace(void** stack, char* buffer, size_t offset, size_t size);

// Private functions
void ac_print_trace(size_t offset);
int ac_fprint_trace(FILE* fp, size_t offset);
int ac_sprint_trace(char* buffer, size_t offset);

#endif // AC_CORE_TRACE_H
