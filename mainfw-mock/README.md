# mainfw-mock

本物のmainFWとまだ結合できていない段階で、printer-fw(`librimanager.so`)を
**共有ライブラリとして正しくロードできるか**・**ステップ実行でデバッグできるか**を
先に確認するための、最小限のモックCプロジェクト。

## 運用方針: ライブラリは自動生成せず、人が手で配置する

`mainfw-mock`自身のconfigure/buildは、printer-fwを一切ビルドしない。
`lib/librimanager.so`・`include/`以下のヘッダは**人が手で配置**する運用にしている
(mainFW側リポジトリがベンダーライブラリを`lib/`・`include/`で同梱する、
一般的な構成を模している)。

- `lib/librimanager.so` が**無ければ**、`cmake --build`は**リンクエラー**で失敗する
  (`cannot find -lrimanager`)
- `include/rim_api.h` 等が**無ければ**、**コンパイルエラー**で失敗する
- 両方**置いてあれば**、そのままリンクして動く

configure時点では存在チェックのみ行い、無くても警告を出すだけでconfigure自体は
通る(実際に失敗するのはビルド時)。

## セットアップ

### 1. printer-fw をビルドし、lib/・include/ へ手で配置する

```sh
cd ..
cmake -S . -B build-shared -DCMAKE_BUILD_TYPE=Debug -DRIMANAGER_BUILD_TESTS=OFF
cmake --build build-shared --target rimanager
cmake --install build-shared --prefix "$(pwd)/mainfw-mock"
```

`RIMANAGER_BUILD_TESTS=OFF`は、gtestをFetchContentで取得しようとしてネットワーク制限に
引っかかるのを避けるため(`rimanager`ターゲット自体のビルドには不要)。

`--prefix`をmainfw-mock自身にすることで、`mainfw-mock/lib/librimanager.so`と
`mainfw-mock/include/*`(CMakeのGNUInstallDirs標準レイアウト)が生成される。
**この手順はmainfw-mock自身のビルドからは呼ばれない**。VSCodeでも
`.vscode/tasks.json`に「printer-fw: build & vendor into lib/ include/(手動・任意)」
という**独立したタスク**として用意してあり、ライブラリを更新したいときだけ
コマンドパレットから明示的に実行する(F5やデフォルトビルドでは自動実行されない)。

### 2. mainfw-mock をビルドする

```sh
cmake -S . -B build
cmake --build build
./build/mainfw_mock
```

`lib/`・`include/`が置かれていれば、`mainfw-mock/lib`がビルド木のRPATHに
埋め込まれるため、`LD_LIBRARY_PATH`を手で通さなくても`librimanager.so`を
見つけて実行できる。置かれていなければ、上記の通りビルド自体が失敗する。

### VSCodeで開く場合

`mainfw-mock/`をワークスペースルートとして開き、デフォルトビルドタスク
(F5前に自動実行)は`cmake --build`のみを行う。ライブラリの配置は
別タスクとして手動で呼び出す(上記参照)。

## デバッグ

`.vscode/launch.json`の「Debug mainfw_mock」構成でF5すれば、`src/main.c`は
もちろん、`librimanager.so`側(`core/`配下の実装)にもステップイン・
ブレークポイント設置ができる(printer-fw側もDebugビルドのため)。

実際に動作確認済み: `main.c`側のブレークポイントと、ライブラリ内
`RIManager_Push`(`core/common/src/rim_api.cpp`)のブレークポイントの両方が
正しく停止し、バックトレース・ローカル変数の参照(`print current.temperature`)
も機能する。

## 何を確認しているか(`src/main.c`)

1. `RIManager_Create()` / `RIManager_Start()` — 初期化とワーカスレッド起動
2. `RIManager_SubscribeCapability()` — 変化通知のコールバック購読
3. `RIManager_Push()` — 単位付き(摂氏25度)センサ値の投入 → ケルビンへ正規化されることを確認
4. `RIManager_GetCurrentCapability()` / `RIManager_GetCapability()` — 現在値の読み出しと通知の読み出し
5. `RIManager_Stop()` / `RIManager_Destroy()` — 後始末

## 注意

- `build-shared/`(printer-fw側)・`mainfw-mock/lib/`・`mainfw-mock/include/`・
  `mainfw-mock/build/`はいずれもビルド/install生成物のため`.gitignore`対象
  (コミットしない)。クローン後は毎回§1の手順で手動配置し直すこと。
- `find_package(rimanager)`は使っていない(CMakeパッケージ設定一式が無くても
  `lib/`・`include/`さえあれば動く、より素朴な「ベンダーライブラリ同梱」形式)。
- ここでの結合確認はあくまで**ローカル開発機(host, Linux/x86-64)向け**。
  実機(VxWorks等)へのクロスビルド・実配布時のRPATH/リンク方式は別途検討が必要。
