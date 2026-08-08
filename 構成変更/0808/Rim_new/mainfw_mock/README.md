# フォルダ構成
mainfw-mock
├── CMakeLists.txt
├── README.md
├── include
├── lib
└── src　
  　　└── main.c

# ライブラリ追加手順

## ①includeに公開ヘッダファイル(XXXX.h)を置く

## ②libにライブラリ(XXXX.so)を置く

## ③CMakeListsのライブラリ修正

以下を追加
target_link_libraries(mainfw_mock PRIVATE 〇〇〇〇.so)