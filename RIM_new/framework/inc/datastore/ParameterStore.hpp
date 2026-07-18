#pragma once
#include "datastore/RIMDataItem.hpp"
#include <mutex>
#include <unordered_map>
namespace rim {
class ParameterStore {
  mutable std::mutex m_;
  std::unordered_map<int, RIMDataItem> p_;

public:
  void Store(const RIMDataItem &i) {
    std::lock_guard<std::mutex> l(m_);
    p_[i.id] = i;
  }
};
} // namespace rim