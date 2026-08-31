#pragma once

#include <cstdarg>

namespace rim
{

enum class LogLevel
{
    kDebug,
    kInfo,
    kWarning,
    kError
};

void LogMessage(
    LogLevel level,
    const char* file,
    int line,
    const char* format,
    ...);

} // namespace rim

#define RIM_LOG_DEBUG(...) \
    ::rim::LogMessage( \
        ::rim::LogLevel::kDebug, \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__)

#define RIM_LOG_INFO(...) \
    ::rim::LogMessage( \
        ::rim::LogLevel::kInfo, \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__)

#define RIM_LOG_WARNING(...) \
    ::rim::LogMessage( \
        ::rim::LogLevel::kWarning, \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__)

#define RIM_LOG_ERROR(...) \
    ::rim::LogMessage( \
        ::rim::LogLevel::kError, \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__)
