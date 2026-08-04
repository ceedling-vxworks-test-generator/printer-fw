/*
 * rim_capi_fault_snapshot_test.c - RIManager_PushFaultSnapshot(FaultInfoList 相当)の
 * C API から実際にリンク・実行して検証する試験。
 *
 * 単発の RIManager_AddError 等(1件ずつのイベント通知)とは別経路(併存)である。
 * 呼び出しのたびに「今存在する異常の全件」を渡す想定で、
 *   - 一覧にあり未登録のコード         → 新規登録する(HEALED でも登録する)
 *   - 一覧にあり登録済みのコード       → state を更新する
 *   - 一覧に無い登録済みのコード       → 削除する(回復扱いにはしない)
 * を確認する。
 */
#include "rim_capi_test.h"
#include "rim_capi_test_helpers.h"

RIM_TEST(FaultSnapshotRegistersActiveAndHealedEntries)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_fault_snapshot_entry_t entries[2];
    entries[0].error_code = 0x100;
    entries[0].state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;
    entries[1].error_code = 0x101;
    entries[1].state      = RIM_FAULT_SNAPSHOT_STATE_HEALED;

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(entries, 2), RI_SUCCESS);

    rim_error_capability_t error;
    RIM_EXPECT(RimWaitErrorCount(2, &error));

    const rim_error_info_t* active = RimFindError(&error, 0x100);
    RIM_EXPECT(active != NULL);
    if (active) RIM_EXPECT_EQ_I(active->state, RIM_ERROR_STATE_ACTIVE);

    const rim_error_info_t* healed = RimFindError(&error, 0x101);
    RIM_EXPECT(healed != NULL);
    if (healed) RIM_EXPECT_EQ_I(healed->state, RIM_ERROR_STATE_RECOVERED);

    RIManager_Stop();
    RIManager_Destroy();
}

/* HEALED だけが送られてきて、その異常コードがまだ未登録の場合は
 * 回復済みとして新規登録する(方針として確認済み)。 */
RIM_TEST(FaultSnapshotRegistersUnknownHealedCodeAsRecovered)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_fault_snapshot_entry_t entry;
    entry.error_code = 0x200;
    entry.state      = RIM_FAULT_SNAPSHOT_STATE_HEALED;

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(&entry, 1), RI_SUCCESS);

    rim_error_capability_t error;
    RIM_EXPECT(RimWaitErrorCount(1, &error));

    const rim_error_info_t* found = RimFindError(&error, 0x200);
    RIM_EXPECT(found != NULL);
    if (found) RIM_EXPECT_EQ_I(found->state, RIM_ERROR_STATE_RECOVERED);

    RIManager_Stop();
    RIManager_Destroy();
}

/* 一覧に無いコードは、次のスナップショットで削除される(回復扱いにはしない)。 */
RIM_TEST(FaultSnapshotRemovesCodeMissingFromNextSnapshot)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_fault_snapshot_entry_t first[2];
    first[0].error_code = 0x300;
    first[0].state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;
    first[1].error_code = 0x301;
    first[1].state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(first, 2), RI_SUCCESS);

    rim_error_capability_t error;
    RIM_EXPECT(RimWaitErrorCount(2, &error));

    /* 次のスナップショットには 0x300 しか無い -> 0x301 は削除される。 */
    rim_fault_snapshot_entry_t second[1];
    second[0].error_code = 0x300;
    second[0].state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(second, 1), RI_SUCCESS);

    RIM_EXPECT(RimWaitErrorCount(1, &error));
    RIM_EXPECT(RimFindError(&error, 0x300) != NULL);
    RIM_EXPECT(RimFindError(&error, 0x301) == NULL);

    RIManager_Stop();
    RIManager_Destroy();
}

/* 既存(単発 API で登録済み)のコードも、スナップショットで state を更新できる。 */
RIM_TEST(FaultSnapshotUpdatesStateOfCodeRegisteredViaAddError)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RIManager_AddError(0x400), RI_SUCCESS);

    rim_error_capability_t error;
    RIM_EXPECT(RimWaitErrorCount(1, &error));
    RIM_EXPECT_EQ_I(RimFindError(&error, 0x400)->state, RIM_ERROR_STATE_ACTIVE);

    rim_fault_snapshot_entry_t entry;
    entry.error_code = 0x400;
    entry.state      = RIM_FAULT_SNAPSHOT_STATE_HEALED;

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(&entry, 1), RI_SUCCESS);

    /* count 自体は変わらないので、状態が更新されるまで少し待ってから読む。 */
    RimSettle();
    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(RIM_CAP_ERROR, &error, sizeof error, NULL),
        RI_SUCCESS);
    RIM_EXPECT_EQ_U(error.count, 1u);
    RIM_EXPECT_EQ_I(RimFindError(&error, 0x400)->state, RIM_ERROR_STATE_RECOVERED);

    RIManager_Stop();
    RIManager_Destroy();
}

/* 空のスナップショット(count=0)は「今は異常が無い」を意味し、全件削除される。 */
RIM_TEST(FaultSnapshotEmptyListClearsAll)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RIManager_AddError(0x500), RI_SUCCESS);

    rim_error_capability_t error;
    RIM_EXPECT(RimWaitErrorCount(1, &error));

    RIM_EXPECT_EQ_I(RIManager_PushFaultSnapshot(NULL, 0), RI_SUCCESS);

    RIM_EXPECT(RimWaitErrorCount(0, &error));

    RIManager_Stop();
    RIManager_Destroy();
}

/* 不正引数(ハンドル不正 / 上限超過)は拒否される。上限を超えても切り詰めない。 */
RIM_TEST(FaultSnapshotInvalidArgumentsAreRejected)
{
    rim_fault_snapshot_entry_t entry;
    entry.error_code = 1;
    entry.state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;

    /* 未生成(Create 前)の状態で呼ぶと RI_NOT_INITIALIZED になること。 */
    RIM_EXPECT_EQ_I(
        RIManager_PushFaultSnapshot(&entry, 1),
        RI_NOT_INITIALIZED);

    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* NULL 配列で count != 0 は拒否される。 */
    RIM_EXPECT_EQ_I(
        RIManager_PushFaultSnapshot(NULL, 1),
        RI_INVALID_PARAMETER);

    /* 内部上限(kMaxErrors=32)を超える count は、黙って切り詰めず拒否する。 */
    {
        rim_fault_snapshot_entry_t many[33];
        size_t i;
        for (i = 0; i < 33; ++i) {
            many[i].error_code = (uint32_t)(1000 + i);
            many[i].state      = RIM_FAULT_SNAPSHOT_STATE_ACTIVE;
        }

        RIM_EXPECT_EQ_I(
            RIManager_PushFaultSnapshot(many, 33),
            RI_INVALID_PARAMETER);
    }

    RIManager_Stop();
    RIManager_Destroy();
}

void RimCapiFaultSnapshotTests(void)
{
    RIM_RUN(FaultSnapshotRegistersActiveAndHealedEntries);
    RIM_RUN(FaultSnapshotRegistersUnknownHealedCodeAsRecovered);
    RIM_RUN(FaultSnapshotRemovesCodeMissingFromNextSnapshot);
    RIM_RUN(FaultSnapshotUpdatesStateOfCodeRegisteredViaAddError);
    RIM_RUN(FaultSnapshotEmptyListClearsAll);
    RIM_RUN(FaultSnapshotInvalidArgumentsAreRejected);
}
