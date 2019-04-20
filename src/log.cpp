/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/time.h>

#include "log.h"

// FIXME: This entire file is a stopgap until better logging is in place

static std::string_view log_level_aligned_name(log_level level) {
  switch (level) {
    case log_level_trace:
      return "TRACE";
    case log_level_debug:
      return "DEBUG";
    case log_level_info:
      return " INFO";
    case log_level_warn:
      return " WARN";
    case log_level_error:
      return "ERROR";
    case log_level_fatal:
      return "FATAL";
  }

  return nullptr;
}

void log_submit(const log_record* rec) {
  // TODO: Move all this to a logging thread with a wait-free input buffer

  // The initial argument vector
  std::vector<fmt::basic_format_arg<fmt::format_context>> args_vec;

  for (unsigned int i = 0; i < rec->msg_fmt_args_num; ++i) {
    const auto& arg = rec->msg_fmt_args[i];

    switch (arg.type) {
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_char:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_char));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_signed_char:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_signed_char));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_unsigned_char:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_unsigned_char));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_signed_short_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_signed_short_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_unsigned_short_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_unsigned_short_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_signed_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_signed_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_unsigned_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_unsigned_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_signed_long_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_signed_long_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_unsigned_long_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_unsigned_long_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_signed_long_long_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_signed_long_long_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_unsigned_long_long_int:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_unsigned_long_long_int));
        break;
      case log_record_msg_fmt_arg::log_record_msg_fmt_arg_type_string:
        args_vec.push_back(fmt::internal::make_arg<fmt::format_context>(arg.value.as_string));
        break;
    }
  }

  // The argument container
  fmt::basic_format_args<fmt::format_context> args(args_vec.data(), static_cast<unsigned int>(args_vec.size()));

  // TODO: Allow different log formats and outputs
  auto time = std::time(nullptr);
  fmt::print("[{:%Y-%m-%d %H:%m:%S}] ({}:{}) {}: {}\n", *std::localtime(&time), rec->src_file, rec->src_line,
    log_level_aligned_name(rec->level), fmt::vformat(rec->msg_fmt, args));
}
