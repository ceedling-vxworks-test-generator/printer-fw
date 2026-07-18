#pragma once
#include "datastore/RIMDataDefinition.hpp"

namespace rim {
class RIMDataDefinitionRegistry {
public:
    static const RIMDataDefinition* Find(RIMDataId id);
};
}
