#pragma once
#include "datastore/SnapshotVersion.hpp"
#include <atomic>
namespace rim {
class VersionedStoreState {
  std::atomic<unsigned long long> v_{0};

public:
  void Increment() { ++v_; }
  SnapshotVersion Get() const { return {v_.load()}; }
};
} // namespace rim