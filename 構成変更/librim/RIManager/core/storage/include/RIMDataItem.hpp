#pragma once

#include "RIMDataId.hpp"
#include "RIMValue.hpp"

namespace rim {
struct RIMDataItem {
    RIMDataId id;
    ValueType valueType;
    RIMValue value;
};
} // namespace rim
