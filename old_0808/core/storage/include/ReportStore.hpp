#pragma once
#include "RIMDataItem.hpp"
#include <mutex>
#include <vector>
namespace rim {
class ReportStore {
  mutable std::mutex m_;
  std::vector<RIMDataItem> v_;

public:
  void Store(const RIMDataItem &i) {
    std::lock_guard<std::mutex> l(m_);
    v_.push_back(i);
  }
};
} // namespace rim