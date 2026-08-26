#include "CapabilitySnapshotResolver.hpp"

#include <algorithm>

namespace rim
{

std::vector<DomainId>
CapabilitySnapshotResolver::
ResolveSnapshotDomains(
    RIDataId changedDataId) const
{
    std::vector<DomainId>
        result;

    const auto& capabilities =
        dependencyMap_.Find(
            changedDataId);

    for (const auto* capability
         : capabilities)
    {
        const auto& domains =
            domainMap_.Find(
                capability->id);

        for (const auto domain
             : domains)
        {
            if (std::find(
                    result.begin(),
                    result.end(),
                    domain)
                == result.end())
            {
                result.push_back(
                    domain);
            }
        }
    }

    return result;
}

} // namespace rim