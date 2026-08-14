#pragma once
namespace rim {
struct SnapshotRequest {
  bool includeDevice{true};
  bool includeError{true};
};
} // namespace rim