# tools/

このリポジトリのC++コードを解析するための、ビルド不要のPythonスクリプト集。
どちらも正規表現＋波括弧対応カウントによる簡易パーサで実装しており、
libclang等の外部依存もビルドシステムとの連携も不要（`python3`単体で動く）。

## coverage_audit.py — 単体テストカバレッジの機械的調査

**用途**: `core/`・`products/printer_a/`配下のクラスのpublicメソッド、および
`include/rim_api.h`・`products/printer_a/printer_a.h`のC-API関数プロトタイプを
洗い出し、`test/unit/`配下の`TEST()`/`TEST_F()`からその名前が参照されているかを
突き合わせて、正常系・異常系それぞれのテスト実施状況を一覧化する。

参照元テストケース名に`Null`/`Invalid`/`Unknown`/`Reject`/`Missing`/`Duplicate`等の
キーワードが含まれていれば異常系、含まれなければ正常系と判定する（命名規則ベースの
ヒューリスティック。テストのassert内容までは読んでいない）。

**使い方**:

```bash
python3 tools/coverage_audit.py                  # サマリーを標準出力
python3 tools/coverage_audit.py --csv out.csv     # 全件CSV
python3 tools/coverage_audit.py --xlsx out.xlsx   # 整形済みExcel（openpyxl要）
```

**向いている場面**:
- 「どのメソッドがまだ異常系テストを持っていないか」を機械的に洗い出したいとき
- リファクタ後・機能追加後に、テストカバレッジがどう変化したかを継続的に確認したいとき
  （CIに組み込んで定点観測することも想定）

**既知の限界**（詳細はスクリプト冒頭のdocstring参照）:
- 間接呼び出しは追えない（例: `RIM_SetBinary`は`PrinterA_SetErrorList`経由でしか
  呼ばれていないテストがあっても、テストコード中に`RIM_SetBinary`という文字列が
  直接出てこなければ「未テスト」と判定される）
- 同名オーバーロードは区別できない（CSVの`Note`列に注記される）
- 正常系/異常系の判定はテストケース名のキーワード頼みで、assertの中身までは見ない
- 「想定される異常系シナリオの提案」のような、コードの意味を読んで判断する部分は
  対象外（これは別途LLMによる手動レビューが必要な領域として意図的に切り離している）

## library_feature_rom_audit.py — C++標準ライブラリ機能の使用状況とROM目安調査

**用途**: 指定ディレクトリ配下の`.hpp`/`.cpp`を再帰的に解析し、クラス（および
ファイル直下の自由関数群）ごとにC++標準ライブラリのどの機能
（`std::vector`、`std::string`、例外、RTTI、iostream、`std::thread`等）を
使用しているかを検出し、各機能の一般的なROM(コードサイズ)影響の目安と合わせて
Excelに出力する。

```bash
python3 tools/library_feature_rom_audit.py                     # サマリーを標準出力
python3 tools/library_feature_rom_audit.py --xlsx out.xlsx      # 詳細Excel
```

**重要な注意**: ROM影響の数値は実測ではなく、一般的な組み込みC++開発における
経験則に基づく目安（Small/Medium/Large/VeryLarge区分と概算KB）。実際の値は
コンパイラ・最適化オプション・リンカのdead code除去設定・ターゲットアーキテクチャに
強く依存する。正確な値が必要な場合は、実ビルド後のmapファイルや`nm`/`size`コマンドの
出力で実測すること。Excel内の目安値は編集可能なので、実測値が判明次第上書きして
使う運用を想定している。

## guard_gap_audit.py — 異常系ガード欠落調査

**用途**: `Rev○○_FW/`（省略時は最新のRev番号のものを自動検出）配下の`.hpp`/`.cpp`を
再帰的に解析し、クラス内メソッドの実装本体（ヘッダのインライン定義・`.cpp`側の
`Class::Method(...)`定義の両方）を抽出したうえで、以下13種類の異常系ガードパターンごとに
「該当しそうなのにガードが見当たらないメソッド」を洗い出す。

- NULLポインタチェック / 未初期化状態チェック(`g_context`) / ID未登録チェック(Find系) /
  型不一致チェック(Get/TryGet系) / サイズ・範囲外チェック / 重複登録・多重初期化チェック /
  未設定データへの参照チェック / キューShutdown後操作チェック / 二重start/stopガード /
  リソース所有権(二重解放/未解放) / コールバック関数ポインタのnullチェック /
  戻り値の握りつぶし / 不正なenum値・範囲外キャスト

各カテゴリは「トリガー条件（そのガードが必要そうか）」と「ガード条件（それらしいガードが
本体のどこかにあるか）」の正規表現ペアで判定する、命名/構文パターンベースのヒューリスティック。

```bash
python3 tools/guard_gap_audit.py                       # 最新のRevNNN_FWを自動検出してサマリー出力
python3 tools/guard_gap_audit.py --root Rev755_FW       # 対象フォルダを明示
python3 tools/guard_gap_audit.py --xlsx out.xlsx        # 凡例/メソッド別一覧/カテゴリ別サマリーの3シート構成Excel
```

**重要な注意**: データフロー解析ではない。「該当あり」は引数の型や呼んでいる関数名など
表面的な特徴からの推定であり、実際にそのガードが必要かは個別に確認が必要。「ガードなし」も
メソッド本体全体でのパターン有無の相関に過ぎず、リスクのある処理の直後を狙い撃ちした
チェックではない。カテゴリごとに検出信頼度（高/中/低）をExcelの凡例シートに明記している。
`g_context`カテゴリは`!g_context->Foo()`（呼び出し結果の否定であってg_context自体のnullチェック
ではない）を誤って「ガードあり」と判定しないよう調整済み（実際に`RIM_GetNotification`の
既知のガード漏れを検出できることで確認済み）。
