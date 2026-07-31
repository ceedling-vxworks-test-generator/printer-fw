#pragma once
#include <array>
namespace rim {
struct ContextEntry {
  const char *key;
  int value;
};
using RIMContext = std::array<ContextEntry, 8>;
} // namespace rim