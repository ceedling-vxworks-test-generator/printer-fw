# mainfw-mock

本物のmainFWとまだ結合できていない段階で、printer-fw(`librimanager.so`)を
**共有ライブラリとして正しくロードできるか**・**ステップ実行でデバッグできるか**を
先に確認するための、最小限のモックCプロジェクト。

本物のmainFWとは独立したCMakeプロジェクトとして、printer-fwを
`find_package(rimanager)` 経由で見つけてリンクする(本番の結合方法と同じ経路)。

## セットアップ

### 1. printer-fw を共有ライブラリとしてビルド・install する

```sh
cd ..
cmake -S . -B build-shared -DCMAKE_BUILD_TYPE=Debug -DRIMANAGER_BUILD_TESTS=OFF
cmake --build build-shared --target rimanager
cmake --install build-shared --prefix "$(pwd)/_install"
```

`RIMANAGER_BUILD_TESTS=OFF`は、gtestをFetchContentで取得しようとしてネットワーク制限に
引っかかるのを避けるため(`rimanager`ターゲット自体のビルドには不要)。

### 2. mainfw-mock をビルドする

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(pwd)/../_install"
cmake --build build
./build/mainfw_mock
```

`_install/lib`がビルド木のRPATHに埋め込まれるため、`LD_LIBRARY_PATH`を手で
通さなくても`librimanager.so`を見つけて実行できる。

### VSCodeで開く場合

`mainfw-mock/`をワークスペースルートとして開き、`.vscode/tasks.json`の
デフォルトビルドタスク(F5前に自動実行)が上記1・2をまとめて行う。

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

- `build-shared/`・`_install/`・`mainfw-mock/build/`はいずれもビルド生成物のため
  `.gitignore`対象(コミットしない)。クローン後は毎回上記手順でビルドし直すこと。
- ここでの結合確認はあくまで**ローカル開発機(host, Linux/x86-64)向け**。
  実機(VxWorks等)へのクロスビルド・実配布時のRPATH/リンク方式は別途検討が必要。
