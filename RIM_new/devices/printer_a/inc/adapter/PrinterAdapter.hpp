#pragma once
#include "adapter/AdapterDispatcher.hpp"
#include "adapter/IHardwareAdapter.hpp"
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
