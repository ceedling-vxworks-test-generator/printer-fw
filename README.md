# printer-fw

> 種類: Library（組込みC共通フレームワーク） / 作成日: 2026-07-01

印刷機の組込みソフトウェア向け **共通フレームワーク（C言語ライブラリ）**。
複数機種へ流用できる、長期運用可能なスケルトン。機種依存部と共通部を明確に分離する。

## アーキテクチャ（5レイヤ + Observer）

```
Driver(機種依存)
  → Raw Data Layer      … データ辞書（型非依存・ID＋タグ付き値・全静的確保・O(1)）
  → Abstract State Layer … 評価器レジストリ（生データ→意味のある状態へ変換）
  → State Monitor Layer  … 変化検知（変化時のみイベント）＋主系状態は FSM
  → Observer (中央)      … Subscribe / Unsubscribe / Notify（購読対象は Abstract State）
  → Application Modules  … UI / Log / 印刷制御 / 通信 / 保守
```

3層に責務分離：
- **core**（共通ライブラリ・機種非依存）: `include/printer_fw/` + `src/`
- **port**（プラットフォーム依存：mutex・時間・assert/log）: `port/`
- **models**（機種依存：データID・記述子表・状態評価器・FSM遷移表・ドライバグルー）: `models/`

## ドキュメント（設計が最優先の成果物）

- [docs/basic-design.md](docs/basic-design.md) — ① 基本設計書（アーキテクチャ・各方針・代替案比較）
- [docs/detailed-design.md](docs/detailed-design.md) — ③ 詳細設計書（IF仕様。これだけで別機種展開可能を目標）
- [docs/self-review.md](docs/self-review.md) — 設計の自己レビューと反映ログ
- [docs/diagrams/printer-fw.drawio](docs/diagrams/printer-fw.drawio) — ② draw.io 図（編集可能・複数ページ）

## ディレクトリ構成

```
printer-fw/
├── include/printer_fw/   公開ヘッダ（ライブラリAPI）
├── src/                  コア実装（共通・機種非依存）
├── port/                 プラットフォーム依存サンプル（bare-metal / FreeRTOS / linux）
├── models/               機種依存サンプル（model_sample）
├── examples/             最小デモ（app_demo）
├── tests/                Unity/ceedling 想定のテスト雛形
├── docs/                 設計書・図
├── cmake/                パッケージ設定（Config / pkg-config テンプレート）
├── scripts/run_demo.sh   クローン後にビルド＆実行しログを表示
├── Makefile              cmake 無し環境向けの簡易ビルド
└── CMakeLists.txt        ビルド構成（install / export / pkg-config 対応）
```

---

## Linux で使う（ビルド・インストール・組込み）

対象は Linux 上で動作するシステムで、**共有/静的ライブラリ `libprinter_fw` として組み込む**ことを想定。

### 1) クローンしてビルド・動作確認（ログを見る）

```sh
git clone https://github.com/ceedling-vxworks-test-generator/printer-fw.git
cd printer-fw
sh scripts/run_demo.sh          # cmake があれば cmake、無ければ Makefile でビルド→テスト→デモ実行
```

`scripts/run_demo.sh` を使わず手動でも可：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/pf_tests                # ユニットテスト
./build/app_demo                # デモ（ログ表示）
```

cmake が無い環境では Makefile：

```sh
make            # build/libprinter_fw.a / .so / app_demo / pf_tests
make run        # デモ実行
make test       # テスト実行
```

### 2) ライブラリとしてインストール

```sh
cmake -S . -B build -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build       # 既定 /usr/local に lib・ヘッダ・cmake・pkg-config を配置
# 例: DESTDIR/PREFIX 指定 → cmake --install build --prefix /opt/printerfw
```

インストールされるもの:
- `lib/libprinter_fw.{a,so}`（`BUILD_SHARED_LIBS=ON` で共有ライブラリ）
- `include/printer_fw/*.h`（公開ヘッダ）
- `lib/cmake/printer_fw/`（`find_package` 用）／ `lib/pkgconfig/printer_fw.pc`

### 3) 別プロジェクトから組み込む

**CMake（推奨）**
```cmake
find_package(printer_fw REQUIRED)
target_link_libraries(my_app PRIVATE printer_fw::printer_fw)
```

**pkg-config**
```sh
gcc my_app.c $(pkg-config --cflags --libs printer_fw) -o my_app
```

**素の gcc**
```sh
gcc my_app.c -I/usr/local/include -L/usr/local/lib -lprinter_fw -o my_app
```

> 組み込む側は、自機種の `port`（`pf_port_t`）と `model`（`pf_model_t`）を実装して `pf_core_init()` に渡す。
> サンプルは `port/pf_port_linux.c`（Linux）・`models/model_sample/` を参照。

---

## ログの見かた（Ubuntu / TeraTerm）

ログはすべて **`pf_port_t.log` コールバック経由**で出力される（出力先を差し替え可能）。

- **Linux port（`pf_port_linux`）**: `stdout` に即時 flush 出力 → **Ubuntu の端末や、SSH/シリアル接続した TeraTerm でそのまま閲覧**できる。
- **実機（別MCU）**: `port` の `log` を UART 送信に実装すれば、**TeraTerm のシリアルコンソール**にログが流れる。

出力例（`app_demo`）:
```
[I] core: init ok model=model_sample data=3 state=3 fsm=1
[I] monitor: state=1 changed 1->0 (t=3)      # PRINTABLE: 印刷不可→印刷可
[I] monitor: state=2 changed -1->0 (t=4)     # TEMP_ALERT: 未知→正常
[W] monitor: reject invalid transition state=0 2->1   # ERROR→PRINTING を拒否
```

ログの制御（`pf_log.h`）:
- `PF_LOG_LEVEL`（既定 `PF_LOG_INFO`）で出力レベルを調整（`ERROR/WARN/INFO/DEBUG`）。
- `PF_LOG_ENABLE=0` でログを完全に無効化（stdio を持たない極小構成向け）。
- 例: `cmake -S . -B build -DCMAKE_C_FLAGS="-DPF_LOG_LEVEL=PF_LOG_DEBUG"`

> 現状はスケルトン（API骨組み＋動作する最小コア）。各機種への展開は [docs/detailed-design.md](docs/detailed-design.md) の「拡張手順」に従う。
