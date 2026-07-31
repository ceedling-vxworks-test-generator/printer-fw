#pragma once
namespace rim {
class MaintenanceRule {
public:
  bool Evaluate(int counter) const { return counter > 1000; }
};
} // namespace rim