// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <print>
#include <source_location>
#include <string_view>
#include <utility>

///@name Error and warning logging
///@{
/**
 * @brief Emits a formatted error message when constructed.
 *
 * Messages include the source file, line, and function name.
 *
 * @tparam T Format argument types.
 */
template <typename... T>
struct LogError {
  explicit LogError(const std::format_string<T...> desc, T... args,
                    const std::source_location& location = std::source_location::current()) {
    std::print(stderr, "\033[31m{}:{} ({}):\033[0m\n{}\n", location.file_name(), location.line(),
               location.function_name(), std::format(desc, std::forward<T>(args)...));
  }
};

/**
 * @brief Deduction guide for `LogError`.
 *
 * @tparam T Format argument types.
 */
template <typename... T>
LogError(std::format_string<T...> desc, T&&... args) -> LogError<T...>;

/**
 * @brief Emits a formatted warning message when constructed.
 *
 * Messages include the source file, line, and function name.
 *
 * @tparam T Format argument types.
 */
template <typename... T>
struct LogWarn {
  explicit LogWarn(const std::format_string<T...> desc, T... args,
                   const std::source_location& location = std::source_location::current()) {
    std::print(stderr, "\033[33m{}:{} ({}):\033[0m\n{}\n", location.file_name(), location.line(),
               location.function_name(), std::format(desc, std::forward<T>(args)...));
  }
};

/**
 * @brief Deduction guide for `LogWarn`.
 *
 * @tparam T Format argument types.
 */
template <typename... T>
LogWarn(std::format_string<T...> desc, T&&... args) -> LogWarn<T...>;
///@}

///@name Verbosity-controlled logging
///@{
#ifndef VERBOSE
/** @brief Global verbosity threshold. */
#define VERBOSE 0
#endif

/**
 * @brief Logs a formatted message when `VERBOSE >= N`.
 *
 * @tparam N Verbosity level.
 * @tparam T Format argument types.
 * @param desc Format string.
 * @param args Format arguments.
 */
template <size_t N = 0, typename... T>
inline void LogC([[maybe_unused]] const std::format_string<T...> desc,
                 [[maybe_unused]] T&&... args) {
  if constexpr (VERBOSE >= N) {
    std::print(stderr, "{}", std::format(desc, std::forward<T>(args)...));
  }
}

/**
 * @brief Logs a formatted message when `VERBOSE >= N`.
 *
 * @tparam N Verbosity level.
 * @tparam T Format argument types.
 * @param desc Format string.
 * @param args Format arguments.
 */
template <size_t N = 0, typename... T>
inline void Log([[maybe_unused]] const std::format_string<T...> desc,
                [[maybe_unused]] T&&... args) {
  if constexpr (VERBOSE >= N) {
    std::println(stderr, "{}", std::format(desc, std::forward<T>(args)...));
  }
}
///@}

///@name Cut-flow logging
///@{
#ifndef LOG_CUT
/** @brief Enables or disables cut-flow logging when nonzero. */
#define LOG_CUT false
#endif

/**
 * @brief Logs that a named cut has been reached.
 *
 * @param desc Short cut description.
 */
inline void LogCut([[maybe_unused]] const std::string_view desc) {
  if constexpr (LOG_CUT) {
    std::println(stderr, "Cut ({})", desc);
  }
}

/**
 * @brief Logs a named cut together with a formatted condition.
 *
 * @tparam T Format argument types.
 * @param desc Short cut description.
 * @param condition Format string describing the cut outcome.
 * @param args Format arguments for `condition`.
 */
template <typename... T>
inline void LogCut([[maybe_unused]] const std::string_view desc,
                   [[maybe_unused]] const std::format_string<T...> condition,
                   [[maybe_unused]] T&&... args) {
  if constexpr (LOG_CUT) {
    std::println(stderr, "Cut ({}): {}", desc, std::format(condition, std::forward<T>(args)...));
  }
}
///@}
