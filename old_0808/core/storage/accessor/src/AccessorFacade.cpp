#include <algorithm>

#include "AccessorFacade.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

AccessId
AccessorFacade::CreateSnapshotByDomains(
    const std::vector<std::string_view>& domains)
{
    return snapshotAccessor_.CreateSnapshot(
        domains);
}

AccessId
AccessorFacade::CreateSnapshotByDataId(
    RIDataId id)
{
    std::string_view domain;

    if (!dataAccessor_.TryGetDomain(
            id,
            domain))
    {
        return {};
    }

    return snapshotAccessor_.CreateSnapshot(
        {
            domain
        });
}

AccessId
AccessorFacade::CreateSnapshotByDataIds(
    const std::vector<RIDataId>& ids)
{
    std::vector<std::string_view>
        domains;

    for (const auto id : ids)
    {
        std::string_view domain;

        if (!dataAccessor_.TryGetDomain(
                id,
                domain))
        {
            continue;
        }

        if (std::find(
                domains.begin(),
                domains.end(),
                domain)
            == domains.end())
        {
            domains.push_back(
                domain);
        }
    }

    if (domains.empty())
    {
        return {};
    }

    return snapshotAccessor_.CreateSnapshot(
        domains);
}

} // namespace rim