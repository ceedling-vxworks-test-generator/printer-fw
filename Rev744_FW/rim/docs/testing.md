# テスト構造

この文書は RIManager 実装リポジトリのテスト構成を、実際のビルド定義と CI 設定に基づいて整理したもの。

## 1. テスト配置

テストソースは [test](../test) 配下で、主に次の層に分かれる。

- [test/unit](../test/unit): 単体ロジック検証
- [test/integration](../test/integration): コンポーネント間の結合検証
- [test/e2e](../test/e2e): API と通知経路を含むエンドツーエンド検証
- [test/thread](../test/thread): 並行処理・ワーカライフサイクル検証
- [test/performance](../test/performance): 性能計測

現時点の .cpp ファイル数は次の通り。

- unit: 47
- integration: 11
- e2e: 14
- thread: 5
- performance: 7

補助ディレクトリとして [test/mock](../test/mock) と [test/support](../test/support) がある。

## 2. ビルド上の集約単位

トップレベル CMake はテストを2つの実行バイナリへ集約する。

- `rimanager_functional_test`
- `rimanager_performance_test`

定義上の対応は次の通り。

- `rimanager_functional_test`: unit + integration + e2e + thread
- `rimanager_performance_test`: performance

根拠:

- functional source glob: [CMakeLists.txt](../CMakeLists.txt#L55)
- performance source glob: [CMakeLists.txt](../CMakeLists.txt#L71)
- executable 定義: [CMakeLists.txt](../CMakeLists.txt#L78)

## 3. 実行コマンド

Configure 後に `build` ディレクトリ（生成物）で実行する。

```bash
# ビルド
make -j"$(nproc)"

# 直接実行
./rimanager_functional_test
./rimanager_performance_test

# CTest 経由
ctest --output-on-failure
```

## 4. CI での扱い

PR トリガーの CI は次の2ジョブ。

- `build-check`: configure/build/functional test
- `coverage`: カバレッジ生成

`rimanager_performance_test` は PR の必須ゲートでは実行していない。

根拠:

- PR トリガー: [.github/workflows/ci.yml](../.github/workflows/ci.yml#L3)
- functional 実行: [.github/workflows/ci.yml](../.github/workflows/ci.yml#L46)
- coverage ジョブ: [.github/workflows/ci.yml](../.github/workflows/ci.yml#L61)

## 5. 運用の目安

日常開発では次を基本とする。

- コミット前: `rimanager_functional_test` を実行
- PR 前: CI と同等に configure/build/functional を通す
- 変更が性能へ影響する場合: `rimanager_performance_test` を追加実行

性能系は実行時間や環境依存のぶれがあるため、常時必須化よりも変更内容に応じて実行対象を選ぶ運用が適している。
