#pragma once
namespace rim {

// ハードウェア起点(実機/エミュレータ等)のAdapter実装が満たすべきインターフェース。
// 「生の数値しか持たない」入力源(AdapterDispatcher::Dispatchに渡す前に
// RIDataIdごとの型が確定していない値)を想定した、ポーリング型アダプタの契約。
class IHardwareAdapter {
public:
  virtual ~IHardwareAdapter() = default;

  // ハードウェアの初期化。Poll()より前に一度だけ呼ばれる想定。
  // 失敗時はfalseを返す。
  virtual bool Initialize() = 0;

  // 1回分のハードウェア読み取り〜AdapterDispatcher::Dispatchへの引き渡しまでを行う。
  // 呼び出し周期は実装側(呼び出し元のスケジューラ)に委ねる。
  // 読み取り失敗時、またはDispatchが拒否した場合はfalseを返す。
  virtual bool Poll() = 0;

  // ハードウェアの終了処理。
  virtual void Shutdown() = 0;
};
} // namespace rim
