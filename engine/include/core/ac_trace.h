#ifndef AC_CORE_TRACE_H
#define AC_CORE_TRACE_H

/**
 * @file ac_trace.h
 * @brief Stack trace functions.
 */

#include <stdio.h>

/**
 * Get the current stack trace.
 * @param stack The stack trace.
 * @param size The size of the stack trace.
 * @return The number of stack frames.
 */
int ac_get_intermediate_trace(void** stack, size_t size);

/**
 * Print the given stack trace to the buffer received from
 * ac_get_intermediate_trace.
 * @param stack The stack trace.
 * @param size The size of the stack trace.
 * @param buffer The buffer to print the stack trace to.
 * @param offset The offset to start printing from.
 * @return The number of characters printed.
 * @see ac_get_intermediate_trace
 */
int ac_sprint_intermediate_trace(void** stack, char* buffer, size_t offset,
                                 size_t size);

/**
 * Print the current stack trace to stdout.
 * @param offset The offset the stack trace by this amount.
 */
void ac_print_trace(size_t offset);

/**
 * Print the current stack trace to the given file.
 * @param fp The file to print the stack trace to.
 * @param offset The offset the stack trace by this amount.
 */
int ac_fprint_trace(FILE* fp, size_t offset);

/**
 * Print the current stack trace to the given buffer.
 * @param buffer The buffer to print the stack trace to.
 * @param offset The offset the stack trace by this amount.
 */
int ac_sprint_trace(char* buffer, size_t offset);

#endif  // AC_CORE_TRACE_H
