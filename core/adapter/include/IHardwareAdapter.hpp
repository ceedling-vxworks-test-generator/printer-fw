#pragma once
namespace rim {
class IHardwareAdapter {
public:
  virtual ~IHardwareAdapter() = default;
  virtual bool Initialize() = 0;
  virtual bool Poll() = 0;
  virtual void Shutdown() = 0;
};
} // namespace rim
