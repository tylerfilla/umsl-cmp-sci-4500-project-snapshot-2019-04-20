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

/** A log record. */
struct log_record {
  /** The severity level. */
  enum log_level level;

  /** The message format string. */
  const char* msg_fmt;

  /** The message format arguments. */
  const struct log_record_msg_fmt_arg* msg_fmt_args;

  /** The number of message format arguments. */
  unsigned int msg_fmt_args_num;

  /** The source file name. */
  const char* src_file;

  /** The source line number. */
  unsigned int src_line;
};

/** A log record message format argument. */
struct log_record_msg_fmt_arg {
  /** The argument value. */
  union {
    const char* as_string;
  } value;

  /** The argument type. */
  enum {
    log_record_msg_fmt_arg_type_string,
  } type;
};

/**
 * Submit a log record.
 *
 * @param rec The record
 */
void log_submit(const struct log_record* rec);

/** @private */
#define LOG__MSG_FMT_ARGS(...) \
    ((struct log_record_msg_fmt_arg[]) { __VA_ARGS__ })

/** @private */
#define LOG__MSG_FMT_ARGS_NUM(...) \
    (sizeof LOG__MSG_FMT_ARGS(__VA_ARGS__) / sizeof(struct log_record_msg_fmt_arg))

/**
 * Log with the given severity.
 *
 * @param lvl The severity level
 * @param fmt The format string
 * @param ... The format arguments
 */
#define LOG(lvl, fmt, ...)                                    \
    log_submit(&(struct log_record) {                         \
      .level = (lvl),                                         \
      .msg_fmt = (fmt),                                       \
      .msg_fmt_args = LOG__MSG_FMT_ARGS(__VA_ARGS__),         \
      .msg_fmt_args_num = LOG__MSG_FMT_ARGS_NUM(__VA_ARGS__), \
      .src_file = __FILE__,                                   \
      .src_line = __LINE__,                                   \
    })

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

/**
 * Annotate a string format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_STR(x)                            \
    (struct log_record_msg_fmt_arg) {             \
      .value.as_string = (x),                     \
      .type = log_record_msg_fmt_arg_type_string, \
    }

#endif // #ifndef LOG_H
