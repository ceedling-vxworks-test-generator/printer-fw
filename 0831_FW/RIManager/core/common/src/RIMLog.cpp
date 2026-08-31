#include "RIMLog.hpp"

#include <cstdio>
#include <cstdarg>
#include <mutex>

namespace
{

const char* ToString(
    const rim::LogLevel level)
{
    switch (level)
    {
    case rim::LogLevel::kDebug:
        return "DEBUG";

    case rim::LogLevel::kInfo:
        return "INFO ";

    case rim::LogLevel::kWarning:
        return "WARN ";

    case rim::LogLevel::kError:
        return "ERROR";

    default:
        return "UNKNOWN";
    }
}

std::mutex g_logMutex;

} // namespace

namespace rim
{

void LogMessage(
    const LogLevel level,
    const char* file,
    const int line,
    const char* format,
    ...)
{
    std::lock_guard<std::mutex> lock(
        g_logMutex);

    std::fprintf(
        stdout,
        "[%s] %s:%d ",
        ToString(level),
        file,
        line);

    va_list args;

    va_start(
        args,
        format);

    std::vfprintf(
        stdout,
        format,
        args);

    va_end(args);

    std::fprintf(
        stdout,
        "\n");

    std::fflush(
        stdout);
}

} // namespace rim