#include "core/ac_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static ac_log_level_t ac_log_level = AC_LOG_LEVEL_DEBUG;
static bool ac_log_color = true;

static const char *ac_log_level_colors[] = {
    "\x1b[37m",     // TRACE
    "\x1b[36m",     // DEBUG
    "\x1b[32m",     // INFO
    "\x1b[33m",     // WARN
    "\x1b[31m",     // ERROR
    "\x1b[41;1;30m" // FATAL
};

static const char *ac_log_level_reset_color = "\x1b[0m";

void ac_log_enable_color(bool enable) {
  ac_log_color = enable;
}
void ac_log_set_level(ac_log_level_t level) {
  ac_log_level = level;
}

void ac_log(FILE *fd, ac_log_level_t level, const char *fmt, ...) {
  if (level < ac_log_level) {
    return;
  }

  if (ac_log_color) {
    fprintf(fd, "%s", ac_log_level_colors[level]);
  }
  switch (level) {
  case AC_LOG_LEVEL_TRACE:
    fprintf(fd, "TRACE: ");
    break;
  case AC_LOG_LEVEL_DEBUG:
    fprintf(fd, "DEBUG: ");
    break;
  case AC_LOG_LEVEL_INFO:
    fprintf(fd, "INFO: ");
    break;
  case AC_LOG_LEVEL_WARN:
    fprintf(fd, "WARN: ");
    break;
  case AC_LOG_LEVEL_ERROR:
    fprintf(fd, "ERROR: ");
    break;
  case AC_LOG_LEVEL_FATAL:
    fprintf(fd, "FATAL: ");
    break;
  }
  if (ac_log_color) {
    fprintf(fd, "%s", ac_log_level_reset_color);
  }
  va_list args;
  va_start(args, fmt);
  vfprintf(fd, fmt, args);
  va_end(args);
}
