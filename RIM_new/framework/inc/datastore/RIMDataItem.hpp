#pragma once

#include "datastore/RIMDataDefinition.hpp"
#include "datastore/RIMValue.hpp"

namespace rim {
struct RIMDataItem {
    RIMDataId id;
    ValueType valueType;
    RIMValue value;
};
} // namespace rim
