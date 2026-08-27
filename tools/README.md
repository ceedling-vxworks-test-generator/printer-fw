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
