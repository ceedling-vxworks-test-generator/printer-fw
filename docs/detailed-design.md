# printer-fw 詳細設計書（インターフェース仕様書）

| 項目 | 内容 |
|------|------|
| 版 | v1.0（2026-07-01） |
| 対象読者 | 本ライブラリの**利用者**（各機種への展開担当）／コア保守者 |
| 目標 | 「**この仕様書だけで別機種へ展開できる**」こと |
| 前提 | 上位設計は [basic-design.md](basic-design.md) を参照 |

> 本書の API はスケルトン（`include/printer_fw/*.h`）と一致する契約である。
> 型のサイズ・enum値はターゲットに合わせて `pf_config.h` で調整できる。

---

## 0. 全体像と命名

- 公開ヘッダは `include/printer_fw/` 配下。利用者は `#include "printer_fw/printer_fw.h"`（単一エントリ）でよい。
- 公開シンボルは `pf_` / `PF_` 接頭。内部実装シンボルは `static` または `pf_<mod>__`（二重アンダースコア）で非公開を表す。
- **全公開関数は `pf_result_t` を返す**（値の取り出しは out 引数）。`PF_OK == 0`。
- 公開ヘッダは `extern "C"` で囲み、C++ から利用可能。

### 0.1 モジュール対応表

| モジュール | ヘッダ | 実装 | レイヤ | 帰属 |
|-----------|--------|------|--------|------|
| 結果コード | `pf_result.h` | （ヘッダのみ＋`pf_result.c`） | 横断 | core |
| 基本型 | `pf_types.h` | （ヘッダのみ） | 横断 | core |
| 設定 | `pf_config.h` | （ヘッダのみ） | 横断 | core |
| Port抽象 | `pf_port.h` | `port/pf_port_*.c` | Port | port |
| Raw Data | `pf_data.h` | `pf_data.c` | ① | core |
| Abstract State | `pf_state.h` | `pf_state.c` | ② | core(+model評価器) |
| FSM | `pf_fsm.h` | `pf_fsm.c` | ③補助 | core |
| Monitor | `pf_monitor.h` | `pf_monitor.c` | ③ | core |
| Observer | `pf_observer.h` | `pf_observer.c` | ④ | core |
| Model記述子 | `pf_model.h` | （ヘッダのみ／実装は機種側） | 登録口 | model |
| Core統括 | `pf_core.h` | `pf_core.c` | 統括 | core |

---

## 1. 共通型・結果コード

### 1.1 `pf_result.h`（enum）

```c
typedef enum {
    PF_OK = 0,                  /* 成功 */
    PF_ERR_INVALID_ARG,         /* NULL/不正引数 */
    PF_ERR_NOT_INIT,            /* 未初期化で呼ばれた */
    PF_ERR_ALREADY_INIT,        /* 二重初期化 */
    PF_ERR_NOT_FOUND,           /* 該当ID/ハンドルなし */
    PF_ERR_NO_SPACE,            /* 静的テーブル/プール満杯 */
    PF_ERR_TYPE_MISMATCH,       /* 期待型と異なる */
    PF_ERR_OUT_OF_RANGE,        /* サイズ/範囲外 */
    PF_ERR_INVALID_TRANSITION,  /* FSMで不許可遷移 */
    PF_ERR_UNSUPPORTED,         /* 未対応操作 */
    PF_ERR_INTERNAL,            /* 内部不整合（通常assert対象） */
} pf_result_t;

const char* pf_result_str(pf_result_t r);   /* デバッグ用文字列 */
```

### 1.2 `pf_types.h`

```c
typedef uint16_t  pf_data_id_t;     /* データ辞書のID */
typedef uint16_t  pf_state_id_t;    /* 抽象状態のID */
typedef uint32_t  pf_time_ms_t;     /* ミリ秒タイムスタンプ */

typedef enum {                      /* 辞書セルの型タグ */
    PF_TYPE_BOOL = 0,
    PF_TYPE_U8, PF_TYPE_U16, PF_TYPE_U32,
    PF_TYPE_I32, PF_TYPE_F32, PF_TYPE_F64,
    PF_TYPE_ENUM,                   /* int32 として保持 */
    PF_TYPE_BLOB,                   /* 構造体・配列・可変長 */
} pf_type_t;

typedef struct {                    /* タグ付き値（型非依存の授受用） */
    pf_type_t type;
    union {
        bool     b;
        uint8_t  u8;
        uint16_t u16;
        uint32_t u32;
        int32_t  i32;
        float    f32;
        double   f64;
        int32_t  e;                 /* enum */
        struct { const void* ptr; uint16_t size; } blob;
    } as;
} pf_value_t;
```

> **型非依存の核**: 利用者は型別便宜API（速い・型安全）と汎用API（`pf_value_t`/blob）を選べる。
> 状態値は `int32_t` に正規化（bool/enum/小整数を1本化）し、Monitor の比較を単純・高速にする。

### 1.3 `pf_config.h`（コンパイル時上限・上書き可）

```c
#ifndef PF_DATA_MAX
#define PF_DATA_MAX        64    /* 辞書エントリ上限 */
#endif
#ifndef PF_STATE_MAX
#define PF_STATE_MAX       32    /* 抽象状態上限 */
#endif
#ifndef PF_OBSERVER_MAX
#define PF_OBSERVER_MAX    16    /* 購読者上限 */
#endif
#ifndef PF_FSM_MAX
#define PF_FSM_MAX          8    /* FSM管理状態の上限 */
#endif
#ifndef PF_RAW_POOL_BYTES
#define PF_RAW_POOL_BYTES 1024   /* BLOB用固定プール（バイト） */
#endif
```

---

## 2. Port 抽象（`pf_port.h`）

### 2.1 役割
HW/OS 依存を1点に隔離する。core はこの IF だけに依存し、特定 RTOS/MCU に縛られない。

### 2.2 データ構造（callback集）

```c
typedef struct {
    void         (*critical_enter)(void);   /* 排他区間の開始（割込み禁止 or mutex lock） */
    void         (*critical_exit)(void);    /* 排他区間の終了 */
    pf_time_ms_t (*time_now)(void);          /* 単調増加のms時刻 */
    void         (*log)(const char* msg);    /* 任意。NULL可 */
    void         (*assert_fail)(const char* expr, const char* file, int line); /* 任意。NULL可 */
} pf_port_t;
```

- `critical_enter/exit` は**短時間**で、ネスト可能であること（実装側の責務）。
- `log/assert_fail` が NULL の場合、core は黙って無視する（リリースビルド向け）。

---

## 3. ① Raw Data Layer（`pf_data.h`）

### 3.1 役割
ドライバが取得した生値を**型非依存**で保持。意味づけはしない（それは ② の責務）。

### 3.2 データ構造：記述子（モデルが供給）

```c
typedef struct {
    pf_data_id_t id;          /* 一意のID（モデルのenum） */
    pf_type_t    type;        /* セルの型 */
    uint16_t     elem_size;   /* BLOBの1要素バイト数（スカラは0でよい） */
    uint16_t     length;      /* 配列要素数（スカラ/単一は1） */
    uint32_t     change_eps;  /* 変化とみなす最小差（f32等向け, 0=厳密一致） */
    const char*  name;        /* デバッグ用（任意, NULL可） */
} pf_data_desc_t;
```

### 3.3 公開 API

```c
pf_result_t pf_data_init(const pf_data_desc_t* desc, size_t count);

/* 型別便宜API（ISR安全：内部で critical 保護） */
pf_result_t pf_data_set_bool(pf_data_id_t id, bool v);
pf_result_t pf_data_set_u8  (pf_data_id_t id, uint8_t v);
pf_result_t pf_data_set_u16 (pf_data_id_t id, uint16_t v);
pf_result_t pf_data_set_u32 (pf_data_id_t id, uint32_t v);
pf_result_t pf_data_set_i32 (pf_data_id_t id, int32_t v);
pf_result_t pf_data_set_f32 (pf_data_id_t id, float v);
pf_result_t pf_data_set_f64 (pf_data_id_t id, double v);
pf_result_t pf_data_set_enum(pf_data_id_t id, int32_t v);

pf_result_t pf_data_get_bool(pf_data_id_t id, bool* out);
pf_result_t pf_data_get_u32 (pf_data_id_t id, uint32_t* out);
pf_result_t pf_data_get_i32 (pf_data_id_t id, int32_t* out);
pf_result_t pf_data_get_f32 (pf_data_id_t id, float* out);
/* … 他型も同様 … */

/* 汎用API（構造体・配列・可変長） */
pf_result_t pf_data_set(pf_data_id_t id, const void* src, uint16_t size);
pf_result_t pf_data_get(pf_data_id_t id, void* dst, uint16_t size);

/* タグ付き値での取得（型を問わず） */
pf_result_t pf_data_get_value(pf_data_id_t id, pf_value_t* out);
```

- **戻り値**: 不正ID→`PF_ERR_NOT_FOUND`、型不一致→`PF_ERR_TYPE_MISMATCH`、サイズ超過→`PF_ERR_OUT_OF_RANGE`。
- **ISR安全**: `set_*` は ISR から呼べる。`get_*` はタスク文脈推奨（Monitorが使用）。

### 3.4 ID 規約と索引（O(1)保証）
- **稠密ID規約**: `id` は **0..count-1 の連番**を基本とし、`index == id` の**直接添字**でアクセスする（O(1)）。
- `pf_data_init` は「全 `id` が範囲内・重複なし」を検証し、違反は `PF_ERR_INVALID_ARG`。
- 疎なIDが必要な機種のみ、init時に索引表（id→slot）を構築する（既定は不要）。
- `0xFFFF` は予約（`PF_STATE_ANY`）。data/state の有効IDに使ってはならない。

非公開 API（`pf_data.c` 内 `static`）:
- `static int pf_data__index_of(pf_data_id_t)` … 既定は `id` を返す直接添字。疎ID時のみ索引表参照
- `static uint8_t* pf_data__cell_ptr(int slot)` … 静的バッファ内のセル先頭
- `static pf_result_t pf_data__check_type(int slot, pf_type_t)`

### 3.5 内部メモリと表現（内部 vs 外部）
- スカラ値: `static pf_value_t g_cells[PF_DATA_MAX];`（型タグ＋共用体）
- BLOB実体: `static uint8_t g_blob_pool[PF_RAW_POOL_BYTES];`（記述子順に詰める）。セルは内部的に `{offset,size}` を保持。
- **取得の安全性**:
  - `pf_data_get(id, dst, size)` … **コピー**で返す（呼出側バッファに複製、安全）。
  - `pf_data_get_value(id, &val)` … BLOB は `val.as.blob.ptr` が**プール内を指すゼロコピー参照**。
    並行更新があり得る環境では `critical` 内、または単一文脈でのみ参照すること（寿命・整合性は呼出側責務）。

---

## 4. ② Abstract State Layer（`pf_state.h`）

### 4.1 役割
複数の生データから**意味のある状態**を算出する。判定ロジックは**評価器（純関数）**としてモデルが供給。

### 4.2 callback 仕様（評価器）

```c
/* 生データを読み、状態値(int32に正規化)を出力する純関数。副作用を持たないこと。 */
typedef pf_result_t (*pf_state_eval_fn)(int32_t* out_value, void* ctx);
```

- 評価器の中では `pf_data_get_*()` で必要な生値を読む。
- `ctx` は記述子で与えた任意ポインタ（しきい値テーブル等）。グローバル不要＝テスト容易。
- **純粋であること**（同じ入力→同じ出力、I/Oや時間に依存しない）を強く推奨。時間依存は Monitor 側のデバウンスで扱う。

### 4.3 データ構造：記述子

```c
typedef struct {
    pf_state_id_t    id;
    pf_state_eval_fn eval;       /* 評価器 */
    void*            ctx;        /* 評価器へ渡す文脈（任意, NULL可） */
    int32_t          initial;    /* 初期状態値 */
    const char*      name;       /* デバッグ用（任意） */
} pf_state_desc_t;
```

### 4.4 公開 API

```c
pf_result_t pf_state_init(const pf_state_desc_t* desc, size_t count);
pf_result_t pf_state_eval(pf_state_id_t id, int32_t* out);  /* その場で評価器を実行 */
pf_result_t pf_state_get (pf_state_id_t id, int32_t* out);  /* 直近にMonitorが確定した値 */
```

### 4.5 enum（モデル定義の例 / 値はモデル裁量）

```c
/* 例: 印刷可否状態 */
enum { PRINT_READY = 0, PRINT_BLOCKED = 1 };
/* 例: 温度状態 */
enum { TEMP_NORMAL = 0, TEMP_WARN = 1, TEMP_ALARM = 2 };
```

---

## 5. FSM エンジン（`pf_fsm.h`）

### 5.1 役割
主系ライフサイクル状態（READY/PRINTING/ERROR/RECOVERY 等）の**許可された遷移のみ**を通す。

### 5.2 データ構造

```c
typedef struct { int32_t from; int32_t to; } pf_fsm_transition_t;

typedef struct {
    pf_state_id_t              state_id;          /* このFSMが管理する抽象状態ID */
    int32_t                    initial;
    const pf_fsm_transition_t* transitions;       /* 許可遷移の配列 */
    size_t                     transition_count;
    const char*                name;
} pf_fsm_desc_t;
```

### 5.3 公開 API

```c
bool        pf_fsm_is_allowed(const pf_fsm_desc_t* fsm, int32_t from, int32_t to);
pf_result_t pf_fsm_validate  (const pf_fsm_desc_t* fsm, int32_t from, int32_t to);
/* 不許可なら PF_ERR_INVALID_TRANSITION */
```

---

## 6. ③ State Monitor（`pf_monitor.h`）

### 6.1 役割
全（対象）抽象状態を再評価し、**前回値と比較して変化したものだけ**をイベント化。
FSM管理状態は遷移検証を通し、不正遷移は弾く。

### 6.2 データ構造

```c
typedef struct {
    pf_state_id_t state_id;
    int32_t       old_value;
    int32_t       new_value;
    pf_time_ms_t  timestamp;
} pf_event_t;

typedef struct {
    const pf_fsm_desc_t* fsm_list;    /* FSM管理する状態（任意, NULL可） */
    size_t               fsm_count;
    uint8_t              debounce_n;  /* N回連続一致で確定（0/1=即時） */
} pf_monitor_cfg_t;
```

### 6.3 公開 API

```c
pf_result_t pf_monitor_init(const pf_monitor_cfg_t* cfg);
pf_result_t pf_monitor_tick(void);   /* 1サイクル：再評価→変化検知→dispatch */
pf_result_t pf_monitor_force_emit(pf_state_id_t id);  /* 初期同期等で強制通知 */
```

### 6.4 動作仕様（tick）
1. 各状態の評価器を実行 → 新値を得る（評価器は必要な Raw を**短い critical** で読む＝局所整合。
   全 Raw の一括スナップショットは既定で行わない＝毎tickの全コピーを避ける）。
2. FSM対象なら `pf_fsm_validate(old, new)`。不許可なら**新値を採用せず** log/assert（旧値を維持）。
3. `debounce_n >= 2` の状態は、同一新値が `debounce_n` 回連続したら確定（チャタリング抑制）。
4. 確定した `new != old`（`f32/f64` は `change_eps` 超過）なら `pf_event_t` を作り `pf_observer_dispatch()`。
5. 変化なしの状態は何もしない（イベントゼロ）。

> **未使用時ゼロコスト**: `fsm_count=0` なら 2 をスキップ、`debounce_n<=1` なら 3 をスキップ（カウンタ配列も未使用）。

---

## 7. ④ Observer / Dispatcher（`pf_observer.h`）

### 7.1 役割
購読者管理と配信。Observerパターンの最小IF（Subscribe/Unsubscribe/Notify）。

### 7.2 callback 仕様

```c
typedef void (*pf_observer_cb)(const pf_event_t* ev, void* ctx);
typedef int  pf_subscription_t;                 /* >=0 有効ハンドル, <0 無効 */
#define PF_STATE_ANY ((pf_state_id_t)0xFFFFu)    /* 全状態を購読（予約ID） */
```

- コールバックは **Monitor の文脈で同期呼び出し**される。**短く・非ブロッキング**に保つこと。
- 重い処理（描画/通信/ファイル）はフラグ・キューに積み、購読側タスクで遅延実行する。
- `PF_STATE_ANY (0xFFFF)` は**予約**。有効な state/data ID として使ってはならない（init検証で弾く）。

**再入規約（dispatch 中の購読変更）**:
- 配信ループは「開始時点の登録スロット集合」に対して回す（途中の増減に影響されない）。
- **自分のコールバック内での `unsubscribe` は許可**（該当スロットは配信ループ完了後に解放）。
- **配信中の新規 `subscribe` は次回 tick から有効**（今回の配信対象には含めない）。
- 観測者テーブルの増減は内部で `critical_enter/exit` により保護する。

### 7.3 公開 API

```c
pf_result_t pf_observer_init(void);
pf_result_t pf_observer_subscribe(pf_state_id_t id, pf_observer_cb cb,
                                  void* ctx, pf_subscription_t* out_handle);
pf_result_t pf_observer_unsubscribe(pf_subscription_t handle);
pf_result_t pf_observer_dispatch(const pf_event_t* ev);  /* 主にMonitorが使用 */
```

### 7.4 Observer 登録方法（利用者手順）

```c
static void on_main_state(const pf_event_t* ev, void* ctx) {
    /* ev->old_value, ev->new_value を見て制御 */
}

pf_subscription_t h;
pf_observer_subscribe(SAMPLE_STATE_MAIN, on_main_state, NULL, &h);
/* … */
pf_observer_unsubscribe(h);   /* 不要になったら解除 */
```
- 全イベントを購読: `pf_observer_subscribe(PF_STATE_ANY, cb, ctx, &h);`（ログ部門などに有効）

---

## 8. Model 記述子（`pf_model.h`）

### 8.1 役割
機種依存のすべて（ID・記述子表・評価器・FSM表・ドライバグルー）を1構造体に集約し core に登録する。

```c
typedef struct {
    const char*            name;
    const pf_data_desc_t*  data_desc;   size_t data_count;
    const pf_state_desc_t* state_desc;  size_t state_count;
    const pf_fsm_desc_t*   fsm_desc;    size_t fsm_count;
    pf_result_t          (*driver_init)(void);  /* 任意：HW初期化 */
    pf_result_t          (*driver_poll)(void);  /* 任意：HW→辞書へ取り込み */
} pf_model_t;

/* 各機種が1つ実装する取得関数（規約） */
const pf_model_t* model_sample_get(void);
```

---

## 9. Core 統括（`pf_core.h`）

```c
pf_result_t      pf_core_init(const pf_model_t* model, const pf_port_t* port);
pf_result_t      pf_core_poll_and_tick(void);   /* driver_poll() → monitor_tick() */
const pf_port_t* pf_core_port(void);            /* port取得（内部・port実装が利用） */
void             pf_core_deinit(void);
```

- `pf_core_init` は順に `pf_data_init → pf_state_init → pf_monitor_init → pf_observer_init` を呼び、port を登録。
- 二重初期化は `PF_ERR_ALREADY_INIT`。未初期化で他APIを呼ぶと `PF_ERR_NOT_INIT`。

---

## 10. 拡張手順（本書の核 ― 既存コード無改修で増やす）

### 10.1 データを追加する（新センサー等）
1. `models/model_xxx/model_xxx_ids.h` の **データID enum に1つ追加**（例 `SAMPLE_RAW_HUMIDITY`）。
2. `model_xxx.c` の `pf_data_desc_t[]` に **1行追加**（id/type/length/eps）。
3. ドライバグルー（`driver_poll`）でその値を `pf_data_set_*()` する。
→ **core 無改修**。`PF_DATA_MAX` を超える場合のみ `pf_config.h` を引き上げ。

### 10.2 新しい状態を追加する
1. `model_xxx_ids.h` の **状態ID enum に1つ追加**（例 `SAMPLE_STATE_DOOR`）。
2. 評価器を1つ書く：`static pf_result_t eval_door(int32_t* out, void* ctx){ ... pf_data_get_*; *out = ...; return PF_OK; }`。
3. `pf_state_desc_t[]` に **1行追加**（id/eval/ctx/initial）。
→ **Abstract層・core 無改修**。

### 10.3 新しい Observer（購読者）を追加する
- 既存APIを呼ぶだけ：`pf_observer_subscribe(state_id, cb, ctx, &h);`。コア・モデルとも無改修。

### 10.4 新しい Driver を追加する
1. `driver_init` でHW初期化、`driver_poll` でHWを読んで `pf_data_set_*()`。
2. ISR から直接 `pf_data_set_*()` してもよい（ISR安全）。
→ core 無改修。`pf_model_t` に glue を結線するだけ。

### 10.5 新しいモデル（機種）を追加する ― 展開の本丸
1. `models/model_yyy/` を作成し、`model_yyy_ids.h`（ID enum）／`model_yyy.c`（記述子表・評価器・FSM表・glue）を実装。
2. `const pf_model_t* model_yyy_get(void)` を実装。
3. アプリ起動時に `pf_core_init(model_yyy_get(), &port_yyy);`。
→ **core・他モデル 無改修**。`model_sample/` を雛形にコピーして差分実装するのが最短。

---

## 11. エラーハンドリング規約（まとめ）

| 状況 | 返り値/動作 |
|------|------------|
| 引数NULL / 不正 | `PF_ERR_INVALID_ARG` |
| 未初期化で呼出 | `PF_ERR_NOT_INIT` |
| ID/ハンドル不在 | `PF_ERR_NOT_FOUND` |
| テーブル/プール満杯 | `PF_ERR_NO_SPACE` |
| 型不一致 | `PF_ERR_TYPE_MISMATCH` |
| サイズ/範囲外 | `PF_ERR_OUT_OF_RANGE` |
| FSM不許可遷移 | `PF_ERR_INVALID_TRANSITION`（値は更新しない） |
| 内部不整合 | `PF_ERR_INTERNAL` ＋ `assert_fail`（あれば） |

- 業務的な異常（温度異常・用紙切れ）は**戻り値ではなく Abstract State**で表し、Observer で配信する。

---

## 12. テスト容易性（テスト方法）

- 評価器は純関数 → 入力（辞書値）を設定して出力を assert するだけで単体テスト可能。
- core は port をスタブ（`critical_*` を no-op、`time_now` を固定）にしてホスト上でテスト可能。
- `tests/` に Unity/ceedling 想定の雛形を同梱（`engineering/ceedling/` のテスト自動生成と接続可能）。
- 推奨ユニット: `pf_data`（型別get/set・型不一致・範囲外）／`pf_fsm`（許可/不許可）／`pf_monitor`（変化時のみ発火）／`pf_observer`（subscribe/unsubscribe/ANY配信）。

---

## 13. 利用最小例（擬似）

```c
#include "printer_fw/printer_fw.h"
#include "model_sample/model_sample.h"

static void on_change(const pf_event_t* ev, void* ctx) {
    /* 状態変化時のみ呼ばれる */
}

int main(void) {
    pf_port_t port = pf_port_baremetal();          /* port実装を選ぶ */
    pf_core_init(model_sample_get(), &port);       /* モデル登録＋全レイヤ初期化 */

    pf_subscription_t h;
    pf_observer_subscribe(PF_STATE_ANY, on_change, NULL, &h);

    for (;;) {
        pf_core_poll_and_tick();                   /* driver_poll → monitor_tick */
        /* 変化があった状態だけ on_change が呼ばれる */
    }
}
```

---

## 14. 関連文書
- [basic-design.md](basic-design.md) — 上位設計・代替案比較
- [diagrams/printer-fw.drawio](diagrams/printer-fw.drawio) — 図（編集可能）
- [self-review.md](self-review.md) — 自己レビューと反映
