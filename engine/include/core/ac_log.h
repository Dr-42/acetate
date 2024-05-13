#ifndef ACETATE_CORE_LOG_H
#define ACETATE_CORE_LOG_H

#include <stdbool.h>

typedef enum ac_log_level_t {
	AC_LOG_LEVEL_TRACE,
	AC_LOG_LEVEL_DEBUG,
	AC_LOG_LEVEL_INFO,
	AC_LOG_LEVEL_WARN,
	AC_LOG_LEVEL_ERROR,
	AC_LOG_LEVEL_FATAL
} ac_log_level_t;

void ac_log_enable_color(bool enable);
void ac_log_set_level(ac_log_level_t level);

void ac_log_trace(const char* fmt, ...);
void ac_log_debug(const char* fmt, ...);
void ac_log_info(const char* fmt, ...);
void ac_log_warn(const char* fmt, ...);
void ac_log_error(const char* fmt, ...);
void ac_log_fatal(const char* fmt, ...);

#endif // ACETATE_CORE_LOG_H
