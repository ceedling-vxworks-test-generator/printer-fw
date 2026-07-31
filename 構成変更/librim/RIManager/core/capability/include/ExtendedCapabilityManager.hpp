#pragma once
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"
#include "MaintenanceCapability.hpp"
#include "NetworkCapability.hpp"
namespace rim {
class ExtendedCapabilityManager {
public:
  JobCapability BuildJob() { return {true, 1}; }
  ConsumableCapability BuildConsumable() { return {80, 50}; }
  MaintenanceCapability BuildMaintenance() { return {false}; }
  NetworkCapability BuildNetwork() { return {true}; }
};
} // namespace rim