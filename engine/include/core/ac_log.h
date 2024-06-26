#ifndef ACETATE_CORE_LOG_H
#define ACETATE_CORE_LOG_H

/**
 * @file ac_log.h
 * @brief Logging functions.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/// Log levels.
/// The log level determines the severity of a log message.
/// Log messages with a level lower than the set level will be ignored.
/// Default log level is INFO.
typedef enum ac_log_level_t {
    AC_LOG_LEVEL_TRACE,
    AC_LOG_LEVEL_DEBUG,
    AC_LOG_LEVEL_INFO,
    AC_LOG_LEVEL_WARN,
    AC_LOG_LEVEL_ERROR,
    AC_LOG_LEVEL_FATAL
} ac_log_level_t;

/// Whether to use ANSI color codes in log output.
/// This is enabled by default.
/// Disable color codes if you are logging to a file or a non-terminal output.
/// @param enable Whether to enable color codes.
void ac_log_enable_color(bool enable);

/// Set the log level.
/// Log messages with a level lower than the set level will be ignored.
/// @param level The log level.
void ac_log_set_level(ac_log_level_t level);

/// Log a message.
/// Not intended to be called directly, but nobody is stopping you.
/// @param fd The file descriptor to log to.
/// @param level The log level.
/// @param fmt The format string.
/// @param ... The format arguments.
void ac_log(FILE *fd, ac_log_level_t level, const char *fmt, ...);

/// Log a message with the TRACE level.
/// @param fmt The format string.
/// @param ... The format arguments.
#define ac_log_trace(fmt, ...) ac_log(stdout, AC_LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// Log a message with the DEBUG level.
/// @param fmt The format string.
/// @param ... The format arguments.
#define ac_log_debug(fmt, ...) ac_log(stdout, AC_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

/// Log a message with the INFO level.
/// @param fmt The format string.
/// @param ... The format arguments.
#define ac_log_info(fmt, ...) ac_log(stdout, AC_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

/// Log a message with the WARN level.
/// @param fmt The format string.
/// @param ... The format arguments.
#define ac_log_warn(fmt, ...) ac_log(stdout, AC_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

/// Log a message with the ERROR level.
/// @param fmt The format string.
/// @param ... The format arguments.
#define ac_log_error(fmt, ...) ac_log(stderr, AC_LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/// Log a message with the FATAL level.
/// Does not call exit() after logging the message.
/// Use ac_log_fatal_exit() to log a message with the FATAL level and call exit().
/// @param fmt The format string.
/// @param ... The format arguments.
/// @see ac_log_fatal_exit
#define ac_log_fatal(fmt, ...) ac_log(stderr, AC_LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

/// Log a message with the FATAL level and call exit().
/// @param fmt The format string.
/// @param ... The format arguments.
/// @see ac_log_fatal
#define ac_log_fatal_exit(fmt, ...)                             \
    do {                                                        \
        ac_log(stderr, AC_LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__); \
        exit(1);                                                \
    } while (0)
#endif  // ACETATE_CORE_LOG_H
