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
├── port/                 プラットフォーム依存サンプル（bare-metal / FreeRTOS）
├── models/               機種依存サンプル（model_sample）
├── examples/             最小デモ（app_demo）
├── tests/                Unity/ceedling 想定のテスト雛形
├── docs/                 設計書・図
└── CMakeLists.txt        ビルド構成
```

## ビルド（スケルトン）

```sh
cmake -S . -B build
cmake --build build
./build/app_demo        # 購読 → raw更新 → 状態変化 → 変化時のみ通知 を確認
```

> 現状はスケルトン（API骨組み＋最小stub）。各機種への展開は `docs/detailed-design.md` の手順に従う。
