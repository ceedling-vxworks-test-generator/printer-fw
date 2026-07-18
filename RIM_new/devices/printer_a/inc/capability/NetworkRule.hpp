#pragma once
namespace rim {
class NetworkRule {
public:
  bool Evaluate(bool link) const { return link; }
};
} // namespace rim