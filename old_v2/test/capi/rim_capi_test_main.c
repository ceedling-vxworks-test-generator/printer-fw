/*
 * rim_capi_test_main.c - capi テストのランナー。
 *
 * 各テストが自分で RIManager_Create/Start/Stop/Destroy を行う。RIManager は
 * プロセス内に常に1つのシングルトンだが、各テストが末尾で必ず Destroy する
 * ため、次のテストの Create は新しい状態から始まる(テスト間で状態が
 * 残存しない)。
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
