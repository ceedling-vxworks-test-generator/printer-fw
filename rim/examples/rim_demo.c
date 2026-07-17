/*
 * rim_demo.c - RIM を「C言語から」使うデモ（要件3の実証）
 *
 * 本ファイルは .c（Cコンパイラでビルド）。RIMの公開IFはすべて extern "C" のため、
 * C側からはC++の存在を一切意識せず、純Cとして呼び出せる。
 * （実体はC++でビルドされるため、リンク時に -lstdc++ が必要）
 */
#include "rim/rim.h"
#include <stdio.h>

/* 購読者コールバック（C関数） */
static void on_capability(void* ctx, const rim_capability_set_t* cap) {
    (void)ctx;
    printf("[SUB] printable=%d temp_alert=%d\n",
           cap->has_printable ? cap->printable : -1,
           cap->has_temp_alert ? cap->temp_alert : -1);
}

int main(void) {
    if (rim_init() != RIM_OK) { printf("rim_init failed\n"); return 1; }
    rim_publisher_subscribe(on_capability, NULL);

    /* --- L1受理点の「型の自由さ」: 異なるnative型でpushできる（要件4-b） --- */
    printf("== push temperature=25.5 (double) ==\n");
    rim_adapter_push_f(RIM_ID_TEMPERATURE, 25.5, NULL);   /* 25.5℃ */
    rim_datastore_dispatch();                             /* 取り込み→Capability生成→Push */

    printf("== push temperature=70 (i32, over threshold) ==\n");
    rim_adapter_push_i32(RIM_ID_TEMPERATURE, 70, NULL);   /* 70℃ → temp_alert */
    rim_datastore_dispatch();

    printf("== raise fault code=0x1001 (u32) ==\n");
    rim_data_context_t fctx;
    fctx.has_fault_state = RIM_TRUE; fctx.fault_state = RIM_FS_RAISED;
    fctx.has_scale = RIM_FALSE; fctx.scale_x1000 = 1000;
    fctx.has_op = RIM_FALSE; fctx.op = RIM_OP_NONE;      /* 従来pushパス互換 */
    fctx.has_key = RIM_FALSE; fctx.key = 0u;
    rim_adapter_push_u32(RIM_ID_FAULT_CODE, 0x1001u, &fctx); /* 異常→印刷不可 */
    rim_datastore_dispatch();

    /* --- 管理配列（Faultコレクション）への add/remove/update ＋ 性質別キュー --- */
    printf("== collection ops on fault registry (FIFO queue + FixedMap) ==\n");
    rim_machine_snapshot_t snap;

    /* まず全消去して初期状態にする（0x1001含め） */
    rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_CLEAR_ALL, 0u, NULL, NULL);
    rim_datastore_dispatch();

    /* (1) 溢れ: dispatch前に DEPTH+1 件ADD → 先頭DEPTH件OK, 超過分は RIM_ERR_POST（無音上書きしない） */
    {
        int ok = 0, full = 0;
        unsigned i;
        for (i = 0; i < (unsigned)RIM_FAULT_QUEUE_DEPTH + 1u; ++i) {
            rim_result_t r = rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_ADD,
                                                0x2000u + i, NULL, NULL);
            if (r == RIM_OK) ++ok; else if (r == RIM_ERR_POST) ++full;
        }
        printf("   overflow: enqueued_ok=%d RIM_ERR_POST=%d (queue depth=%d)\n",
               ok, full, RIM_FAULT_QUEUE_DEPTH);
        rim_datastore_dispatch();
        if (rim_accessor_read_snapshot(RIM_DOMAIN_FAULT, &snap) == RIM_OK && snap.has_fault)
            printf("   after dispatch: active_count=%u (expect %d)\n",
                   snap.fault.active_count, RIM_FAULT_QUEUE_DEPTH);
    }

    /* (2) 順序: ADD A, ADD B, REMOVE A を dispatch前に投入 → 反映後は B のみ残る */
    rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_CLEAR_ALL, 0u, NULL, NULL);
    rim_datastore_dispatch();
    rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_ADD,    0xAAAAu, NULL, NULL);
    rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_ADD,    0xBBBBu, NULL, NULL);
    rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_REMOVE, 0xAAAAu, NULL, NULL);
    rim_datastore_dispatch();
    if (rim_accessor_read_snapshot(RIM_DOMAIN_FAULT, &snap) == RIM_OK && snap.has_fault) {
        printf("   ordered add/add/remove: active_count=%u", snap.fault.active_count);
        if (snap.fault.active_count == 1u)
            printf(" code=0x%04X (expect 0xBBBB)", snap.fault.active_codes[0]);
        printf("\n");
    }

    /* (3) 更新: 既存キーの UPDATE は成功、不在キーの REMOVE は変化なし */
    {
        rim_result_t ru = rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_UPDATE, 0xBBBBu, NULL, NULL);
        rim_result_t rr = rim_adapter_submit(RIM_ID_FAULT_CODE, RIM_OP_REMOVE, 0x9999u, NULL, NULL);
        rim_datastore_dispatch();
        printf("   update existing=%d, remove-absent=%d (both enqueued OK=%d)\n",
               (int)ru, (int)rr, (int)RIM_OK);
    }

    /* --- 可変長Listの仕組み（要件4-a・C++ FixedVectorをC ABIで） --- */
    printf("== variable-length list (C ABI over C++ FixedVector) ==\n");
    rim_dei_list_t* list = rim_dei_list_acquire();
    if (list) {
        rim_data_entry_item_t it;
        it.id = RIM_ID_HUMIDITY;
        it.value.type = RIM_VT_PERCENT; it.value.u.percent = 40;
        it.context.has_fault_state = RIM_FALSE; it.context.has_scale = RIM_FALSE;
        rim_dei_list_push(list, &it);
        it.value.u.percent = 55; rim_dei_list_push(list, &it);
        printf("   list size=%lu cap=%lu\n",
               (unsigned long)rim_dei_list_size(list),
               (unsigned long)rim_dei_list_capacity(list));
        rim_dei_list_release(list);
    }

    /* --- 隣接 Accessor Layer（Pull参照） --- */
    printf("== accessor pull ==\n");
    rim_capability_set_t cap;
    if (rim_accessor_read_capability(&cap))
        printf("   [PULL] printable=%d temp_alert=%d\n", cap.printable, cap.temp_alert);

    rim_shutdown();
    printf("done.\n");
    return 0;
}
