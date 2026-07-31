#pragma once
#include "RIMDataItem.hpp"
#include <deque>
#include <mutex>
namespace rim {
class ErrorStore {
  mutable std::mutex m_;
  std::deque<RIMDataItem> q_;

public:
  void Store(const RIMDataItem &i) {
    std::lock_guard<std::mutex> l(m_);
    q_.push_back(i);
  }
};
} // namespace rim