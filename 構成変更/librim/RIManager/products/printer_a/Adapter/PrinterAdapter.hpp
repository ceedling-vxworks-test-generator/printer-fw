#pragma once

//
// PrinterAdapter - PrinterA のハードウェアを叩く実アダプタ。
//
// 読み出す先(RIMDataId::kTemperatureSensorA など)を知っているのは機種だけなので、
// core ではなく products に置く。core は IHardwareAdapter という抽象しか知らない。
//

#include "AdapterDispatcher.hpp"
#include "IHardwareAdapter.hpp"
namespace rim {
class PrinterAdapter : public IHardwareAdapter {
public:
  explicit PrinterAdapter(AdapterDispatcher &dispatcher);
  bool Initialize() override;
  bool Poll() override;
  void Shutdown() override;

private:
  AdapterDispatcher &dispatcher_;
};
} // namespace rim
