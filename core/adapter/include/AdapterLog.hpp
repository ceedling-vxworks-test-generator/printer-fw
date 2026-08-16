#pragma once

#include <cstdarg>
#include <cstdio>

// Adapterレイヤ専用の最小ロガー。
//
// MainFW_Mock 等から実行した際に、
//   - どの関数(Dispatch/Initialize/Poll/Shutdown等)を通ったか
//   - エラー発生時にそれとひと目でわかるか
// を標準出力/標準エラーへ即座に出すためのもの。
// 依存ライブラリを増やさないよう fprintf ベースで実装する。
//
// 使い方:
//   RIM_ADAPTER_LOG_TRACE("id=%d value=%.2f", static_cast<int>(id), value);
//   RIM_ADAPTER_LOG_ERROR("DataItemDefinition not found: id=%d", static_cast<int>(id));
//
// 関数名は __func__ から自動取得するため、呼び出し側でタグ付けを誤る心配はない。

namespace rim
{

enum class AdapterLogLevel
{
    kTrace,
    kError,
};

inline void AdapterLogV(
    AdapterLogLevel level,
    const char* function,
    const char* format,
    va_list args)
{
    std::FILE* const stream =
        (level == AdapterLogLevel::kError)
            ? stderr
            : stdout;

    const char* const tag =
        (level == AdapterLogLevel::kError)
            ? "[Adapter][ERROR]"
            : "[Adapter][TRACE]";

    std::fprintf(
        stream,
        "%s %s: ",
        tag,
        function);

    std::vfprintf(
        stream,
        format,
        args);

    std::fprintf(
        stream,
        "\n");
}

inline void AdapterLog(
    AdapterLogLevel level,
    const char* function,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);

    AdapterLogV(
        level,
        function,
        format,
        args);

    va_end(args);
}

} // namespace rim

#define RIM_ADAPTER_LOG_TRACE(format, ...) \
    ::rim::AdapterLog(::rim::AdapterLogLevel::kTrace, __func__, format, ##__VA_ARGS__)

#define RIM_ADAPTER_LOG_ERROR(format, ...) \
    ::rim::AdapterLog(::rim::AdapterLogLevel::kError, __func__, format, ##__VA_ARGS__)
