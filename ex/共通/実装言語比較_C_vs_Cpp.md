# RIM（ReactiveInfoManager）実装言語比較：C vs C++（実装者目線）

本書は、RIM（ReactiveInfoManager＝L1 RIM_AdapterLayer → L2 RIM_DatastoreLayer →
L3 RIM_CapabilityLayer → L4 RIM_PublisherLayer、および隣接する Accessor Layer）を
**C（C99/C11）で実装する場合**と**C++（C++17）で実装する場合**の得失を、
本設計の具体的な構成要素に即して比較する。一般論ではなく「この設計を書くとき何が起きるか」を扱う。

---

## 0. 採用決定（2026-07-14・チーム事情による方針確定）

> 本節は§1〜§7の**分析より上位の「確定した採用方針」**である。分析上の最有力は
> §1・§6のとおり **C++-firstハイブリッド**だが、**実装担当がC言語専門**という
> チーム事情を最優先し、以下の **C-firstハイブリッド** を採用する。以降の各節は
> この決定の根拠・留意点として読むこと（§1・§6の「推奨」は分析上の結論であり、
> 本§0が実運用上の決定で上書きする）。

**採用方針：C-firstハイブリッド（IFはextern "C"、内部は基本C・C++は2箇所に限定）**

1. **公開IFはすべて `extern "C"`**：RIMの層間IF・公開APIはC ABIで提供し、
   **C言語の解釈でそのまま使用できる**ようにする（C専門の開発者が主読者・主実装者）。
   ヘッダは純Cとして解釈可能な形（`pf_*_t` 様式、関数ポインタテーブル＋ctx）で書く。
2. **内部実装は基本C**：タグ付きunion、`bool + out引数`（`std::optional` 不使用）、
   `has_xxx` フラグ（optionalメンバ不使用）、関数ポインタテーブル（抽象クラス不使用）、
   DataId連番enum＋固定長配列（`unordered_map` 不使用）、短時間mutex＋構造体コピー
   （atomic `shared_ptr` 不使用＝H-7緩和）で書く。→ §4 の設計差分6点を正式採用。
3. **C++を用いる箇所は次の2つに限定**（ここだけC++の仕組みを持ち込む）：
   - **(a) 可変長Listの仕組み**：容量固定・静的確保の可変長コンテナ（`takeUpdatedValues()`
     の戻り、購読者リスト、SnapshotのドメインListなど）は C++ テンプレートの
     固定容量コンテナ（`rim::FixedVector<T, N>` 相当）で実装し、要素型ごとの
     ボイラープレートを排除する。**動的確保はしない**（内部は固定長配列＋サイズ）。
   - **(b) RIM_AdapterLayer でデータを受け取る箇所の型の自由さ**：`RawValue` は
     入力元ごとに型が異なるため、受理点のみ C++ の型機構（テンプレート／`std::variant`
     相当の型安全union）で「型の自由さ」を確保する。境界を越えて後続層へ渡す時点で
     正規化済みの `DataValue`（タグ付きunion・C表現）へ落とす。
4. **それ以外でのC++機能（例外・RTTI・STLコンテナ・atomic shared_ptr・
   テンプレートメタプロ）は持ち込まない**（`-fno-exceptions -fno-rtti` 継続）。
5. **ビルドは C++ でリンク**（(a)(b)がC++のため）。公開IFがC ABIなので、
   利用側のCコードからは `-lstdc++`（またはリンカにg++）でリンクする。

この方針の実体スケルトンは `rim/`（`rim/include/rim/*.h`＝extern "C"公開ヘッダ、
`rim/src/*.cpp`＝C++実装）に置く。§4の設計差分6点は本方針で「必須」として適用済みとする。

---

**前提（本リポジトリの現状）**
- 既存コードは「C ABIヘッダ（`extern "C"` / `pf_*_t`）＋ C++17実装」のハイブリッド構成
- 組込みフラグは `-fno-exceptions -fno-rtti`（例外・RTTI不使用）で運用中
- port層は baremetal / FreeRTOS / Linux を想定。テストはCeedling系（C）資産あり
- 設計書（詳細設計）の疑似コードはC++語彙（`std::optional` / `shared_ptr` / `condition_variable` 等）で記述されている

---

## 1. 結論サマリ

| 観点 | C | C++ (C++17, 例外/RTTI無効) |
|------|---|---------------------------|
| 設計書との距離 | **遠い**（optional/variant/RAIIを手作りで置換） | **近い**（疑似コードがほぼそのまま実装になる） |
| H-7（prev Capabilityのatomic共有） | **難所**（参照カウント/ダブルバッファを自作） | `shared_ptr` atomic操作で定石どおり |
| 型安全性（DataValue/Context/Snapshot） | タグ付きunion＋規約頼み（コンパイラは守ってくれない） | variant/optional/enum classでコンパイル時に守られる |
| メモリ戦略 | **静的確保が自然**（FW向きの規律を言語が強制） | 動的確保が混入しやすい（規律とレビューで縛る必要） |
| コード量（本設計の見積り） | 多い（ジェネリクスなし・ボイラープレート大） | 少ない（テンプレート/クラスで圧縮） |
| RTOS移植性・ツールチェーン | **最強**（どの環境でもCはある） | ほぼ問題ないが古いBSPでC++17が無い場合がある |
| テスト | Ceedling/CMock資産を直接活用 | GoogleTest/gMock（リンク差し替えモックは不可、IF注入で代替） |
| チーム採用性 | C技術者は多いが並行処理の安全担保が人依存 | モダンC++既修者が必要 |
| デバッグ/障害解析 | シンプル（シンボルが素直、逆アセンブルが読める） | テンプレート/インライン展開でスタックが読みにくい場合あり |

**分析上の推奨**：純粋に技術得失だけで見れば **「C ABI公開ヘッダ＋C++17内部実装（例外/RTTI無効・
動的確保は初期化時のみ）」**（C++-firstハイブリッド）が最有力。理由は§6。
**ただし実運用の採用方針は §0 の C-firstハイブリッドで確定**（実装者がC専門というチーム事情を優先）。
本表の「C」列は、その C-first 内部実装で実際に何が起きるかの予習として読むこと。

---

## 2. 設計要素別の比較（本設計で実際に書くもの）

### 2.1 DataEntryItem（DataValue variant + DataContext）

設計：`DataValue` は種別ごとの型のバリアント、`DataContext` は key-value 集合。

**C++**
```cpp
using DataValue = std::variant<Celsius, Percent, JobProgress, FaultCode /*...*/>;
// 誤った型の取り出しはコンパイルエラー or 明示的な失敗として現れる
```

**C**
```c
typedef struct {
    pf_data_type_t tag;          /* 種別タグ（手動管理） */
    union {
        int32_t  celsius_x100;   /* 固定小数で表現 */
        uint8_t  percent;
        pf_job_progress_t job;
        pf_fault_code_t   fault;
    } u;
} pf_data_value_t;
```
- Cは**タグとunionの整合をコンパイラが検査しない**。CentralInputPortの
  「Value型とIdの定義一致検証」（DataStore詳細§3.4）が実行時の最後の砦になり、
  タグ間違いは**実行時までバグとして潜伏**する。
- 実装工数そのものは大差ないが、**種別追加のたびに switch 文が全域に増殖**するのがC。
  C++はvisit/テンプレートで追加点が局所化する。

### 2.2 `std::optional<RegistryDomain> apply()`（変化なし=nullopt 契約）

**C**
```c
/* 戻り値bool＋出力引数が定石 */
bool pf_fault_registry_apply(pf_fault_registry_t* self,
                             const pf_data_entry_item_t* item,
                             pf_registry_domain_t* out_changed);
```
- 書けるが、**「変化なし」と「エラー」の区別**を戻り値設計で毎回考えることになる
  （設計のoptional契約はこの曖昧さを型で消している）。
  Cでは全Registry/Dispatcherで規約を統一し、レビューで守るしかない。

### 2.3 MachineSnapshot（optionalメンバの部分Snapshot）

**C**
```c
typedef struct {
    bool has_fault;        pf_fault_snapshot_t fault;
    bool has_operation;    pf_operation_snapshot_t operation;
    bool has_current;      pf_current_value_snapshot_t current;
} pf_machine_snapshot_t;
```
- `has_xxx` フラグ方式で素直に書ける。**Cでも大きな困難はない**。
- ただし「フラグを見ずにメンバを読む」事故はCでは防げない（C++のoptionalは参照時に意図が明示される）。

### 2.4 層間インターフェース（IRegistryUpdateNotifier / IPublisher / IMachineSnapshotReader）

**C++**：抽象クラス。モック注入も自然。

**C**：関数ポインタテーブル＋コンテキストポインタの定石。
```c
typedef struct {
    void (*notify_updated)(void* ctx, pf_registry_domain_set_t domains);
    void* ctx;
} pf_registry_update_notifier_t;
```
- Cでも十分実用的で、**既存 `pf_port_t` と同じパターン**なので違和感はない。
- 欠点は「`ctx` の型消失（void*）」。渡し間違いはコンパイラが検出しない。
- IF数が少ない本設計（層間IFは実質4本）では**Cのハンデは小さい**。

### 2.5 H-7：前回Capabilityのatomic共有（最大の分岐点）

設計契約：Capabilityスレッドが更新し、Accessor等の**別スレッドがロックレスで読む**。

**C++**：定石どおり一行級。
```cpp
std::atomic_store(&prev_, std::make_shared<const CapabilitySet>(merged));
auto snap = std::atomic_load(&prev_);   // 読側：寿命はshared_ptrが保証
```

**C**：以下のいずれかを**自作**する。
1. **参照カウント自作**（atomicカウンタ＋解放競合の正しい処理）
   → ABA/解放タイミングの罠があり、**正しく書くのは高難度**。レビューも困難。
2. **ダブルバッファ＋世代カウンタ（seqlock風）**
   → 読側リトライループが必要。CapabilitySetサイズが小さいため現実解だが、
     「読んでいる間に2回更新されたら」等の検証が必要。
3. **単純mutex**
   → 最も安全だが「Accessorの参照がCapability更新を阻害しない」という
     設計意図（Accessor詳細§10.2）が弱まる。実害は小さい可能性が高く、**Cならこれを推奨**。

**実装者への正直な忠告**：Cで1.を選ぶと本設計最悪の沼になる。Cなら3.（短時間mutex＋構造体コピー）に
契約を緩める設計変更をセットで行うべき。C++なら設計書のまま書けば終わる。

### 2.6 pendingドメイン集合＋wake（H-1機構）

- 中身は「ビットマスクOR＋mutex＋条件変数（またはRTOSイベントフラグ）」。
- **C/C++で難易度差はほぼない**。FreeRTOSなら `xEventGroupSetBits` が
  そのままpending集合になるため、**Cのほうが素直**なくらいである。
- `FlagSet<RegistryDomain>` も実体は `uint32_t` ビット演算。Cで全く問題ない。

### 2.7 Queue / Buffer（IQueue\<DataEntryItem\>、CurrentValueBufferのslot map）

**C++**：テンプレート1つで3レーン共用。`unordered_map` は組込みでは静的配列に置換推奨。

**C**：
- ジェネリックQueueは「void*＋要素サイズ」or「マクロ生成」or「レーン別に3回書く」。
  本設計は要素型が `DataEntryItem` 1種なので**単一型Queueでよく、実害は小さい**。
- CurrentValueBufferの `slots_` は、**DataIdを連番enumにして固定長配列**にすれば
  mapより単純・高速・静的確保。**これはC/C++どちらでも採るべき組込み定石**であり、
  設計書の `unordered_map` はホスト向け表記と割り切る。

### 2.8 RAIIとロック（全層）

**C++**：`lock_guard` / `unique_lock`。早期returnでも解放漏れなし。
  H-1のwait_for述語つき待機も標準ライブラリで書ける。

**C**：
```c
pf_mutex_lock(&reg->mutex);
/* ... 途中でreturnしてはならない ... */
pf_mutex_unlock(&reg->mutex);
```
- 本設計はロック区間が多い（Registry×3、Buffer、pending集合、StateRepository…）。
  **解放漏れはCで最も出やすいバグ**であり、`goto cleanup` 規約か
  「ロック区間を関数に切り出す」規約の徹底が必須。
- Dispatcherの `while(!stop)` ＋停止token解除（ライフサイクル設計）もCで書けるが、
  条件変数の spurious wakeup 処理などの様式美を全員が守る必要がある。

### 2.9 Evaluator 8種（Capability判定式）

- C++：`ICapabilityEvaluator` 実装8クラス。
- C：関数ポインタ配列 `pf_evaluator_fn evaluators[PF_CAP_KIND_COUNT]`。
- **どちらでも綺麗に書ける**。差はほぼない（状態を持たない純関数群のため）。

### 2.10 CapabilitySetマージ規則（H-3）

- 実体は「Kindごとの has_value 判定＋コピー」。**C/C++で難易度差なし**。
- ただしCは `optional` がないため「有効フラグ配列＋値配列」で表現することになり、
  §2.3と同じく**フラグ見落とし事故はレビュー頼み**。

---

## 3. テスト戦略の違い（実装者の日常に直結）

| 項目 | C（Ceedling/CMock/Unity） | C++（GoogleTest/gMock） |
|------|---------------------------|-------------------------|
| モック生成 | **CMockがヘッダから自動生成**。リンク差し替えで下位層を丸ごと差し替え可能 | gMockは仮想IF前提。設計が既にIF注入型なので相性は良い |
| 層単体テスト | 例：FaultDispatcherテストでnotifier/registryをCMock自動モック | 例：CapabilityManagerテストにReader/Publisherのモック注入 |
| 並行テスト | スレッド絡みはCeedlingの守備範囲外（自前） | gtestでスレッド起こして検証（sanitizer併用が容易） |
| 資産 | **社内/本リポジトリ周辺にCeedling資産あり**（VxWorks系実績） | 本リポジトリのtests/はC++ |
| 静的解析 | MISRA-C系ツールが充実 | clang-tidy/sanitizerが充実（TSanはH-1/H-7検証に非常に有効） |

**実装者目線の要点**：本設計の肝は並行契約（H-1/H-2/H-7）であり、
**ThreadSanitizer / AddressSanitizer が回せるC++（ホストビルド）の検証力は大きな武器**。
Cでも回せるが、参照カウント自作（§2.5）のような検証対象を増やすのはC側である。

---

## 4. C実装を選ぶ場合の設計書からの差分（必要な設計変更）

Cで作るなら、以下を**設計書側も修正**して整合させること（暗黙変更は禁止）。

1. **H-7の緩和**：atomic shared_ptr → 「短時間mutex＋構造体コピー」契約へ変更（§2.5）
2. `std::optional<RegistryDomain>` → `bool + out引数` の統一規約を用語集に追加
3. `MachineSnapshot`/`CapabilitySet` のoptional → `has_xxx` フラグ表現を正式定義
4. 層間IF → 関数ポインタテーブル＋ctx の正式シグネチャを定義
5. `unordered_map` 系 → DataId連番enum＋固定長配列に置換（これはC++でも推奨）
6. エラー戻り値 → 既存 `pf_result_t` 体系に統一

## 5. C++実装を選ぶ場合の規律（既存方針の継承＋追加）

1. **例外・RTTI無効**（既存 `-fno-exceptions -fno-rtti` を継続）。エラーは戻り値
2. **動的確保は初期化フェーズのみ**（ライフサイクル設計 Phase 1）。
   定常運転中の new/malloc 禁止。`shared_ptr<const CapabilitySet>` の差し替えは
   例外的に許すか、固定2面バッファ＋atomicポインタで代替するかを最初に決める
3. STLコンテナは容量固定（reserve済みvector / 固定長array）。定常時の再確保禁止
4. 公開ヘッダは**C ABI（extern "C"）を維持**（既存 `pf_*.h` 様式。上位アプリ/他言語から使える）
5. テンプレートは「Queue等の器」まで。メタプログラミングは持ち込まない（デバッグ性維持）

---

## 6. 総合評価と推奨

### 工数感（本設計・相対値）

| フェーズ | C | C++ |
|---------|---|-----|
| 骨格実装（層・IF・型） | 1.3〜1.5x | 1.0x |
| 並行機構（H-1/H-7） | 1.5〜2.0x（H-7自作分＋検証） | 1.0x |
| テスト整備 | 1.0x（CMock資産が効く） | 1.0〜1.2x |
| 保守（種別追加・Cap追加） | switch増殖・規約維持コスト | 局所変更で済む |

### 分析上の推奨：**C++-firstハイブリッド** ／ 実運用の採用：**C-firstハイブリッド（§0）**

> 以下は技術得失のみで見た分析上の結論。**実運用では §0 の C-firstハイブリッドを採用**し、
> C++ は「可変長List」と「RIM_AdapterLayer受理点の型の自由さ」の2箇所に限定する。
> §0採用時は下記の6点（§4の設計差分）を必須で適用する。

- **（分析上の最有力）公開ヘッダ＝C ABI、内部実装＝C++17（例外/RTTI無効）**
- 根拠：
  1. 設計書の契約（optional/variant/atomic shared_ptr）がそのまま実装になり、**設計と実装の乖離が最小**
  2. H-7をはじめ並行契約の実装・検証コストが**C比で明確に低い**（TSan活用含む）
  3. 既存printer-fwと同一方針であり、port層（baremetal/FreeRTOS/Linux）の構造をそのまま流用できる
  4. C ABI公開により、利用側・既存Cテスト資産との接続は失わない
- **純Cに倒す条件**：ターゲットのツールチェーンにC++17が無い/使用禁止規約（MISRA-C強制等）がある場合。
  その際は§4の設計変更6点を先に実施すること。

---

## 7. 判断チェックリスト（着手前に埋める）

- [ ] ターゲットMCU/RTOSのツールチェーンはC++17対応か（VxWorks版数・GCCバージョン）
- [ ] コーディング規約（MISRA-C / AUTOSAR C++等）の指定はあるか
- [ ] 定常時ヒープ使用の可否（shared_ptr戦略の決定に直結）
- [ ] テスト環境はCeedling継続か、gtestホストテスト併用か
- [ ] チームのC++習熟度（variant/atomic/所有権の設計レビューができる人数）
