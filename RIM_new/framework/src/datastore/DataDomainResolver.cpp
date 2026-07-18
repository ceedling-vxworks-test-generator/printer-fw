#include "datastore/DataDomainResolver.hpp"
#include "datastore/RIMDataDefinitionRegistry.hpp"

namespace rim {

DataDomain DataDomainResolver::Resolve(RIMDataId id)
{
    const auto* def = RIMDataDefinitionRegistry::Find(id);

    if (def == nullptr)
    {
        return DataDomain::kDevice;
    }

    return def->domain;
}

}
