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
    char as_char;
    signed char as_signed_char;
    unsigned char as_unsigned_char;
    signed short int as_signed_short_int;
    unsigned short int as_unsigned_short_int;
    signed int as_signed_int;
    unsigned int as_unsigned_int;
    signed long int as_signed_long_int;
    unsigned long int as_unsigned_long_int;
    signed long long int as_signed_long_long_int;
    unsigned long long int as_unsigned_long_long_int;
    float as_float;
    double as_double;
    long double as_long_double;
    const char* as_string;
  } value;

  /** The argument type. */
  enum {
    log_record_msg_fmt_arg_type_char,
    log_record_msg_fmt_arg_type_signed_char,
    log_record_msg_fmt_arg_type_unsigned_char,
    log_record_msg_fmt_arg_type_signed_short_int,
    log_record_msg_fmt_arg_type_unsigned_short_int,
    log_record_msg_fmt_arg_type_signed_int,
    log_record_msg_fmt_arg_type_unsigned_int,
    log_record_msg_fmt_arg_type_signed_long_int,
    log_record_msg_fmt_arg_type_unsigned_long_int,
    log_record_msg_fmt_arg_type_signed_long_long_int,
    log_record_msg_fmt_arg_type_unsigned_long_long_int,
    log_record_msg_fmt_arg_type_float,
    log_record_msg_fmt_arg_type_double,
    log_record_msg_fmt_arg_type_long_double,
    log_record_msg_fmt_arg_type_string,
  } type;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Submit a log record.
 *
 * @param rec The record
 */
void log_submit(const struct log_record* rec);

#ifdef __cplusplus
} // extern "C"
#endif

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
 * Annotate a char format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_C(x)                            \
    (struct log_record_msg_fmt_arg) {           \
      .value.as_char = (x),                     \
      .type = log_record_msg_fmt_arg_type_char, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _c LOG_ARG_C
#endif

/**
 * Annotate a signed char format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_SC(x)                                   \
    (struct log_record_msg_fmt_arg) {                   \
      .value.as_signed_char = (x),                      \
      .type = log_record_msg_fmt_arg_type_signed_char,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _sc LOG_ARG_SC
#endif

/**
 * Annotate an unsigned char format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_UC(x)                                     \
    (struct log_record_msg_fmt_arg) {                     \
      .value.as_unsigned_char = (x),                      \
      .type = log_record_msg_fmt_arg_type_unsigned_char,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _uc LOG_ARG_UC
#endif

/**
 * Annotate a signed short int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_S(x)                                        \
    (struct log_record_msg_fmt_arg) {                       \
      .value.as_signed_short_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_signed_short_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _s LOG_ARG_S
#endif

/**
 * Annotate an unsigned short int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_US(x)                                         \
    (struct log_record_msg_fmt_arg) {                         \
      .value.as_unsigned_short_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_unsigned_short_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _us LOG_ARG_US
#endif

/**
 * Annotate a signed int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_I(x)                                  \
    (struct log_record_msg_fmt_arg) {                 \
      .value.as_signed_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_signed_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _i LOG_ARG_I
#endif

/**
 * Annotate an unsigned int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_UI(x)                                   \
    (struct log_record_msg_fmt_arg) {                   \
      .value.as_unsigned_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_unsigned_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _ui LOG_ARG_UI
#endif

/**
 * Annotate a signed long int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_L(x)                                        \
    (struct log_record_msg_fmt_arg) {                       \
      .value.as_signed_long_int = (x),                      \
      .type = log_record_msg_fmt_arg_type_signed_long_int,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _l LOG_ARG_L
#endif

/**
 * Annotate an unsigned long int argument.
 *
 * @param x The argument
 */
#define LOG_ARG_UL(x)                                         \
    (struct log_record_msg_fmt_arg) {                         \
      .value.as_unsigned_long_int = (x),                      \
      .type = log_record_msg_fmt_arg_type_unsigned_long_int,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _ul LOG_ARG_UL
#endif

/**
 * Annotate a signed long long int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_LL(x)                                           \
    (struct log_record_msg_fmt_arg) {                           \
      .value.as_signed_long_long_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_signed_long_long_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _ll LOG_ARG_LL
#endif

/**
 * Annotate an unsigned long long int format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_ULL(x)                                            \
    (struct log_record_msg_fmt_arg) {                             \
      .value.as_unsigned_long_long_int = (x),                     \
      .type = log_record_msg_fmt_arg_type_unsigned_long_long_int, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _ull LOG_ARG_ULL
#endif

/**
 * Annotate a float format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_F(x)                              \
    (struct log_record_msg_fmt_arg) {             \
      .value.as_float = (x),                      \
      .type = log_record_msg_fmt_arg_type_float,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _f LOG_ARG_F
#endif

/**
 * Annotate a double format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_D(x)                              \
    (struct log_record_msg_fmt_arg) {             \
      .value.as_double = (x),                     \
      .type = log_record_msg_fmt_arg_type_double, \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _d LOG_ARG_D
#endif

/**
 * Annotate a long double format argument.
 *
 * @param x The argument
 */
#define LOG_ARG_LD(x)                                   \
    (struct log_record_msg_fmt_arg) {                   \
      .value.as_long_double = (x),                      \
      .type = log_record_msg_fmt_arg_type_long_double,  \
    }

#ifndef LOG_NO_SHORT_ARGS
#define _ld LOG_ARG_LD
#endif

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

#ifndef LOG_NO_SHORT_ARGS
#define _str LOG_ARG_STR
#endif

#endif // #ifndef LOG_H
