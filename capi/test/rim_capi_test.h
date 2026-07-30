#pragma once

/*
 * rim_capi_test.h - capi テスト用の最小ハーネス。
 *
 * capi のテストは「C言語から呼べること」自体が検証対象なので、C++専用の gtest は使えない。
 * assert() は最初の失敗で即 abort してしまい1回の実行で1件しか分からないため、
 * 失敗を数えて続行する EXPECT 方式にしている。
 *
 * 使い方:
 *   RIM_TEST(MyCase) { RIM_EXPECT_EQ_I(actual, expected); }
 *   ... 実行側(rim_capi_test_main.c)で RIM_RUN(MyCase);
 */

#include <stdio.h>
#include <stdint.h>

/* 失敗件数(実体は rim_capi_test_main.c) */
extern int g_rim_test_failures;
extern const char* g_rim_test_current;

#define RIM_TEST(name) void name(void)
#define RIM_TEST_DECL(name) void name(void)

#define RIM_RUN(name)                          \
    do {                                       \
        g_rim_test_current = #name;            \
        printf("[ RUN      ] %s\n", #name);    \
        name();                                \
    } while (0)

#define RIM_FAIL_(fmt, ...)                                                  \
    do {                                                                     \
        printf("  [  FAILED  ] %s:%d: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
        ++g_rim_test_failures;                                               \
    } while (0)

#define RIM_EXPECT(cond)                                        \
    do {                                                        \
        if (!(cond)) RIM_FAIL_("expected: %s", #cond);          \
    } while (0)

/* 符号付き整数の比較(値をログに出す) */
#define RIM_EXPECT_EQ_I(actual, expected)                                        \
    do {                                                                         \
        long long a_ = (long long)(actual), e_ = (long long)(expected);          \
        if (a_ != e_) RIM_FAIL_("%s: actual=%lld expected=%lld", #actual, a_, e_); \
    } while (0)

/* 符号なし整数の比較 */
#define RIM_EXPECT_EQ_U(actual, expected)                                                     \
    do {                                                                                      \
        unsigned long long a_ = (unsigned long long)(actual), e_ = (unsigned long long)(expected); \
        if (a_ != e_) RIM_FAIL_("%s: actual=%llu expected=%llu", #actual, a_, e_);             \
    } while (0)

/* 各テストファイルが公開するエントリ(rim_capi_test_main.c から呼ぶ) */
void RimCapiSmokeTests(void);
void RimCapiValueTests(void);
void RimCapiPerfTests(void);
