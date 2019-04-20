/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef LOG_H
#define LOG_H

/** A log severity level. */
enum log_level {
  log_level_trace,
  log_level_debug,
  log_level_info,
  log_level_warn,
  log_level_error,
  log_level_fatal,
};

#define LOG(lvl, fmt, ...) printf("%s\n", fmt); // TODO

/**
 * Log with TRACE severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGT(fmt, ...) LOG(log_level_trace, (fmt), ##__VA_ARGS__)

/**
 * Log with DEBUG severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGD(fmt, ...) LOG(log_level_debug, (fmt), ##__VA_ARGS__)

/**
 * Log with INFO severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGI(fmt, ...) LOG(log_level_info, (fmt), ##__VA_ARGS__)

/**
 * Log with WARN severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGW(fmt, ...) LOG(log_level_warn, (fmt), ##__VA_ARGS__)

/**
 * Log with ERROR severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGE(fmt, ...) LOG(log_level_error, (fmt), ##__VA_ARGS__)

/**
 * Log with FATAL severity.
 *
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOGF(fmt, ...) LOG(log_level_fatal, (fmt), ##__VA_ARGS__)

#endif // #ifndef LOG_H
