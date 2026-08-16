#pragma once
#include "IHardwareAdapter.hpp"
#include "AdapterLog.hpp"
namespace rim {
class MockAdapter : public IHardwareAdapter {
public:
  bool Initialize() override { RIM_ADAPTER_LOG_TRACE("enter/exit result=true"); return true; }
  bool Poll() override { RIM_ADAPTER_LOG_TRACE("enter/exit result=true"); return true; }
  void Shutdown() override { RIM_ADAPTER_LOG_TRACE("enter/exit"); }
};
} // namespace rim
