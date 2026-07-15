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
    rim_adapter_push_u32(RIM_ID_FAULT_CODE, 0x1001u, &fctx); /* 異常→印刷不可 */
    rim_datastore_dispatch();

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
