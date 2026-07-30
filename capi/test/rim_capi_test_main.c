/*
 * rim_capi_test_main.c - capi テストのランナー。
 *
 * rim_init/rim_shutdown はここで1回だけ行う(rim_capi.cpp の RIMSystem は
 * 静的シングルトンなので、テストごとの再初期化はしない)。
 * 各テストは他のテストの残存状態に依存しないよう、CurrentValue系(最新値上書き)の
 * idを使うか、使い終わったら自分で片付ける方針とする。
 *
 * 終了コード: 失敗0件なら 0、1件以上なら 1(ctest がそのまま合否判定に使う)。
 */

#include "rim_capi_test.h"
#include "rim_capi.h"

int         g_rim_test_failures = 0;
const char* g_rim_test_current  = "";

int main(void)
{
    if (rim_init() != RIM_OK) {
        printf("[  FAILED  ] rim_init() が失敗した\n");
        return 1;
    }

    printf("========== capi tests ==========\n");
    RimCapiSmokeTests();
    RimCapiValueTests();
    RimCapiPerfTests();
    printf("================================\n");

    rim_shutdown();

    if (g_rim_test_failures == 0) {
        printf("[  PASSED  ] 失敗 0 件\n");
        return 0;
    }
    printf("[  FAILED  ] 失敗 %d 件\n", g_rim_test_failures);
    return 1;
}
