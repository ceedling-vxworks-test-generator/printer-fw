#pragma once
#include "SnapshotVersion.hpp"
namespace rim {
class VersionedSnapshotPolicy {
public:
  bool IsConsistent(SnapshotVersion a, SnapshotVersion b) const {
    return a.value == b.value;
  }
};
} // namespace rim