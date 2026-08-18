#pragma once
#include "IHardwareAdapter.hpp"
namespace rim {
class MockAdapter : public IHardwareAdapter {
public:
  bool Initialize() override { return true; }
  bool Poll() override { return true; }
  void Shutdown() override {}
};
} // namespace rim
