/*
 * rim_capi_test_main.c - capi テストのランナー。
 *
 * 各テストが自分で RIManager_Create/Start/Stop/Destroy を行う(旧実装は
 * rim_init/rim_shutdown をここで1回だけ行っていたが、こちらの実装は
 * ハンドル単位で状態を持つため、テストごとに独立したハンドルを使うほうが
 * 他のテストの残存状態に依存せず安全)。
 *
 * 終了コード: 失敗0件なら 0、1件以上なら 1(ctest がそのまま合否判定に使う)。
 */

#include "rim_capi_test.h"

int         g_rim_test_failures = 0;
const char* g_rim_test_current  = "";

int main(void)
{
    printf("========== capi tests ==========\n");
    RimCapiSmokeTests();
    RimCapiValueTests();
    RimCapiPerfTests();
    RimCapiFaultSnapshotTests();
    printf("================================\n");

    if (g_rim_test_failures == 0) {
        printf("[  PASSED  ] 失敗 0 件\n");
        return 0;
    }
    printf("[  FAILED  ] 失敗 %d 件\n", g_rim_test_failures);
    return 1;
}
