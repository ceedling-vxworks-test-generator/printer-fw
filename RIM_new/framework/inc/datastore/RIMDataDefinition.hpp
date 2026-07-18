#pragma once

#include "datastore/ValueType.hpp"

namespace rim {

enum class DataDomain {
  kDevice,
  kJob,
  kError,
  kConsumable,
  kMaintenance,
  kNetwork
};

enum class RIMDataId {
  kTemperature,
  kHumidify,
  // kDoorOpen,
  kDoorUpper,
  kDoorRight,
  kDoorLeft,

  kCurrentError,
  kStapleLevel
};

struct RIMDataDefinition
{
    RIMDataId id;
    DataDomain domain;
    ValueType valueType;
    const char* name;
};
} // namespace rim
