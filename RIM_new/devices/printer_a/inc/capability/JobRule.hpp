#pragma once
namespace rim {
class JobRule {
public:
  bool EvaluateJobActive(int id) const { return id > 0; }
};
} // namespace rim