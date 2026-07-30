# printer-fw — ReactiveInfoManager (C++ OO / 旧ex仕様③準拠)

`old/ex/` の各レイヤ仕様③(2026-07-07 正)を **C++ OO ＋ framework/devices 構造**で実装したもの。
本ディレクトリ構成が printer-fw の正式なコード。旧`pf_*`フレームワーク一式は `old/` を参照。
機器のセンサ値を受理・正規化し、性質別にステージングして Snapshot を作り、8種Capabilityを
生成して購読者へ配信する。

## レイヤ構成(仕様③)

```
RIM_AdapterLayer(L1) → RIM_DatastoreLayer(L2) → RIM_CapabilityLayer(L3) → RIM_PublisherLayer(L4)
                              │
                              └→ Accessor Layer(隣接・Pull参照)
```

### L1 RIM_AdapterLayer
`RawDataInput`(型自由 PushI32/PushF/PushU32) → `IRuleResolver.SelectRule` → `IRule.Convert`(正規化)
→ `IInputClassifier.Classify`(性質判別) → CentralInputPort の3種post。

### L2 RIM_DatastoreLayer
`CentralInputPort`(3種post・検証) → 3レーン(`FaultInputQueue`/`OperationReportQueue`/`CurrentValueBuffer`)
→ 3Dispatcher(`FaultDispatcher`/`OperationDispatcher`/`CurrentValueDispatcher`)
→ 3Registry(`FaultRegistry`/`OperationRegistry`/`CurrentValueRegistry`) ← `MachineRegistry`(集約)
→ `MachineSnapshotReader.Capture`。`Registry.Apply`→変更ドメイン、`notifyUpdated(RegistryDomainSet)`。
ドメイン単位 mutex。RegistryDomain は Fault/Operation/Environment/Consumable/Safety/Maintenance/Health。

### L3 RIM_CapabilityLayer
`CapabilityManager`(IRegistryUpdateNotifier実装) が更新通知→Snapshot取得→`ICapabilityBuilder.Build`
→`CapabilityDiffChecker`(意味的差分)→`CapabilityPriorityChecker`(優先度)→`IPublisher.Notify`。
8種Capability(Error/Job/Env/Maint/Health/Safety/Consumable/Print)。ErrorCap/JobCap変化は Event 通知。

### L4 RIM_PublisherLayer
`PublishEngine`(IPublisher実装) が トリガ評価・Rate Limit → `SubscriptionBroker`(関心Cap一致)
→ `StateRepository`(購読者ごと配信的差分) → Push配信。5トリガ(OnChange/Periodic/Threshold/Event/Initial)。
Event / 優先度Critical・High は Rate Limit 除外。

### Accessor Layer
`PrinterStatusReader.GetPrinterStatus` → `PrinterStatus{data, capability}`。参照専用・Pull・状態非保持。

## framework / devices

- **framework/**(機種共通): 全レイヤ機構・共通型・抽象IF(IRule/IRuleResolver/IInputClassifier/
  IRegistryUpdateNotifier/IMachineSnapshotReader/ICapabilityBuilder/IPublisher/ISubscriber)・
  固定容量コンテナ・`RIMSystem`(結線)。
- **devices/printer_a/**(機種可変): `RIMDataId`(id一覧)、`PrinterADataProfile`(SelectRule/Classify)、
  Rule群、`PrinterACapabilityBuilder`(8種判定)。`RIMDataId` は devices 側の定義で、framework は
  ヘッダ名にのみ依存する(機種展開してもframework無改修。詳細は`docs/adapter/02_詳細設計.md`§1.1)。
- **devices/_skeleton/**: 新機種テンプレ(ビルド対象外・README参照)。

## 設計判断(仕様TBDの確定)
- 更新通知後の取得範囲: PrintCap 等の横断依存のため全ドメインを capture。
- 優先度ポリシー: Error/非Safety=Critical、Maint/消耗品少=High、Job/Env変化=Normal、他=Low。
- 閾値: 温度>60℃、湿度20-80%、消耗品<10%、メンテカウンタ>10000。
- 喪失検知: Queue満杯を kErrPost で検知。フル再同期プロトコルは今後(hook)。
- Rate Limit: 最小間隔既定0(無効)。トレーリングエッジ配信は周期タスク導入時に実装(hook)。
- C API(extern"C")は持たない(旧`old/rim/` の役割)。

## ビルド・テスト
```sh
cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
```
テスト: Registry(Fault/Operation/CurrentValue) / Capability(Builder/Diff/Priority) / 全系フロー(SystemFlow)。

## mainFW への組み込み(ライブラリ化)

`rimanager` は `install`/`find_package` に対応した通常のCMakeライブラリとして配布できる。

```sh
cmake -S . -B build -DRIMANAGER_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/path/to/prefix
cmake --build build
cmake --install build
```

mainFW側は次のように参照する(`find_package` 経由、または `add_subdirectory`)。

```cmake
find_package(rimanager REQUIRED)
target_link_libraries(mainFW PRIVATE rimanager::rimanager)
```

`RIMANAGER_BUILD_TESTS` は既定でトップレベルビルド時のみON(`add_subdirectory()`で組み込まれた
場合は自動的にOFF)。framework(機種共通)と devices/printer_a(機種固有。`RIMDataId`を含む)の
公開ヘッダは同じ `include/` 以下へマージしてインストールされるため、mainFW側の
`#include "adapter/PrinterADataProfile.hpp"` 等はビルド前後で変わらない。

## C言語から呼ぶ場合(`capi/`)

mainFWがC言語の場合、C++実装(上記の`framework`/`devices`)を直接呼ぶことはできない
(クラス・参照・テンプレート・`std::optional`・名前空間はいずれもC非互換)。`capi/`が
その上に被せる薄い`extern "C"`シムで、C言語からはこの1ヘッダだけを使う。

```c
#include "rim_capi.h"

rim_init();
rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(25.0), NULL);
rim_dispatch();

rim_subscription_t sub = rim_subscribe(on_capability_changed, NULL, 0 /* 全Capabilityに関心 */);
rim_printer_status_t st = rim_get_status(RIM_DOMAIN_CURRENT_ALL, true);
```

- `rim_id_t`(id一覧)は機種固有のため`devices/printer_a/inc/capi/rim_ids.h`が提供する。
  C++側の`RIMDataId`と整数値が一致していることは`capi/src/rim_capi.cpp`の`static_assert`群が
  ビルド時に検証する(手書きの2重管理のズレを機械チェック)。
- `RawValue`(構造体・配列を受け付ける型自由な生値)は本物のC `union`として`rim_raw_value_t`に
  対応する。`std::optional`を持つ型(`DataContext`/`CapabilitySet`/`MachineSnapshot`等)は
  `has_xxx`という`bool`フィールド付きのプレーン構造体に平坦化してある。
### capi のテスト

`capi/test/*.c` は**Cコンパイラ(gcc)でビルド**し、C++実装とリンクして実行する。
gtestに依存しないため`RIMANAGER_BUILD_TESTS`の値に関わらず、トップレベルビルド時は
既定でビルド・`ctest`登録される(ネットワーク不要)。

```sh
cmake -S . -B build && cmake --build build --target rim_capi_smoke
ctest --test-dir build -R rim_capi_smoke --output-on-failure
./build/rim_capi_smoke     # 直接実行すると性能値も見える
```

| ファイル | 内容 |
|---|---|
| `rim_capi_test.h` | 最小ハーネス(`RIM_EXPECT_*`)。失敗しても続行し件数を集計する |
| `rim_capi_smoke.c` | 呼び出し可否・購読上限・不正引数の扱い |
| `rim_capi_value_test.c` | **値の保証**(double経路の精度・切り捨て・clamp) |
| `rim_capi_perf_test.c` | **性能測定** |

#### 値の保証: double 経路の精度(実測で確認済み)

受理点が`double`1本(`rim_raw_scalar`)なので整数の値落ちが懸念されるが、**`double`自体は
原因にならない**ことを回帰テストで固定している。

- `double`の仮数部は53bitあるため、**int32/uint32の全域が完全に正確**。`float`(仮数部24bit)
  では壊れる`2^24+1`のような値も保たれる(`ValueUInt32IsExactThroughDouble`、
  および全32ビット位置の往復テスト)。
- **値落ちの実体は`double`ではなく Rule 内の`static_cast`による切り捨て**。小数は四捨五入
  されず**0方向へ切り捨て**られる(`ValueTruncatesTowardZeroBeyondResolution`)。
  例: 温度`25.678`→`2567`(2568ではない)、`-0.005`→`0`、インク残量`50.9`→`50`。
  x100固定小数の分解能(0.01)より細かい桁は保持されない。
- ⚠️ **既知の穴**: 型の範囲外入力(負値→uint32、巨大値、NaN/Inf)は C/C++ 規格上 **UB** で、
  現状どの Rule も範囲チェックをしていない。実測挙動を
  `ValueOutOfRangeIsUncheckedKnownGap`に記録して可視化しているが、この挙動に依存しては
  ならない(恒久対策は Rule 側での範囲チェック追加。**未対応**)。

#### 性能(参考値・ホスト実測)

実機の性能保証はターゲット上での計測で行うこと。以下はホスト(x86_64/gcc)での相対比較用。
テストのアサートは「桁が変わる破滅的退行」だけを捕まえる緩い上限にしてある(CI環境の負荷で
揺れる厳しい閾値は偽陽性になるため)。Debugビルドでは約9倍遅くなる。

| 経路 | Release | 備考 |
|---|---|---|
| `rim_push` のみ | ~45 ns/op | 変換＋分類＋バッファ格納 |
| `rim_push` + `rim_dispatch` | ~170 ns/op | Registry反映・差分判定まで |
| 同上 + 購読者あり(Capability不変) | ~170 ns/op | **配信は起きない** |
| 同上 + 毎サイクルCapability変化 | ~260 ns/op | 最悪ケース(配信＋コールバック) |
| `rim_get_status`(全域+Capability) | ~85 ns/op | 戻り値は336バイトの値返し |

> 「Capability不変なら配信されない」ことをテストで確認している
> (`PerfDispatchWithSubscriberNoCapabilityChange`)。生値が動いても意味づけ結果が
> 変わらなければイベントゼロ、という設計契約が実際に効いている。
> 高頻度で`rim_get_status`をポーリングする用途では、336バイトのコピーが毎回発生する点に注意。

## VSCodeでのデバッグ

`.vscode/tasks.json`・`.vscode/launch.json`に`rim_capi_smoke`用のビルド・デバッグ設定がある。
`build-debug/`という別ディレクトリを使い、`-DRIMANAGER_BUILD_TESTS=OFF`でgtestのFetchContent
(ネットワーク要)を避けているため、ネットワーク制限下でもビルド・ブレークポイントでの
停止が可能(gdb使用、`.c`/`.cpp`どちらのファイルにもブレークポイントを張れる)。

1. VSCodeでこのリポジトリを開く(拡張機能`C/C++`があること)
2. 実行とデバッグ(Ctrl+Shift+D)から「Debug rim_capi_smoke」を選択して開始
   (`preLaunchTask`が自動でconfigure・ビルドする)
3. `capi/src/rim_capi.cpp`や`capi/test/rim_capi_smoke.c`にブレークポイントを置いて確認する

`miDebuggerPath`はLinux(`/usr/bin/gdb`)を既定にしている。macOSでは`MIMode`を`lldb`に変更するか、
`gdb`のインストール先に合わせて`miDebuggerPath`を書き換えること。

### うまく止まらないとき

- **`warning: gdb failed to set controlling terminal: Operation not permitted` が出て、
  プログラムが起動しない**
  gdbがシェル経由でプログラムを起動する際に制御端末を設定できないために起きる
  (WSL・Dockerコンテナ・devcontainer等の制限された環境で発生する)。`launch.json`の
  `setupCommands`に`set startup-with-shell off`を入れてあるので通常は回避されるが、
  それでも出る場合は`"externalConsole": true`も試すこと。
- **ブレークポイントの丸が灰色の中抜きになる**
  デバッグ情報とソースの対応が取れていない状態。`build-debug/`を一度削除してから
  「cmake: configure (Debug)」をやり直す(`CMAKE_BUILD_TYPE`がDebug以外だと`-g`が付かない)。
- **プログラムの標準出力(`rim_capi_smoke: OK`)が見当たらない**
  「デバッグコンソール」ではなく、統合ターミナルの`cppdbg: rim_capi_smoke`タブに出力される。
