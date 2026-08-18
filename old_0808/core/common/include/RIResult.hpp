#pragma once
#include "RIStatus.hpp"
namespace rim {
template <class T> struct RIResult {
  RIStatus status;
  T value;
};
} // namespace rim