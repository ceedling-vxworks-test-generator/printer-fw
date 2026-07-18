#pragma once
#include "datastore/RIMDataDefinition.hpp"

namespace rim {
class DataDomainResolver {
public:
    static DataDomain Resolve(RIMDataId id);
};
}
