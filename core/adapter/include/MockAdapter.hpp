#pragma once
#include "IHardwareAdapter.hpp"
namespace rim {

// IHardwareAdapterの何もしない実装(テストダブル)。
// 実ハードウェア/PrinterAdapter相当の実装が無い環境でも、
// アダプタが存在する前提のコードパス(初期化/ポーリング/終了処理)を
// 通しておきたい場合に使う。DataStoreへは一切値を投入しない。
class MockAdapter : public IHardwareAdapter {
public:
  bool Initialize() override { return true; }
  bool Poll() override { return true; }
  void Shutdown() override {}
};
} // namespace rim
