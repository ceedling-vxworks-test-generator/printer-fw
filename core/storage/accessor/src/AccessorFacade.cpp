#include <algorithm>

#include "AccessorFacade.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

AccessId
AccessorFacade::CreateSnapshotByDomains(
    const std::vector<std::string_view>& domains)
{
    // return snapshotAccessor_.CreateSnapshot(
    //     domains);
    return {};
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

    // return snapshotAccessor_.CreateSnapshot(
    //     {
    //         domain
    //     });
    return snapshotAccessor_
        .CreateSnapshot(id)
        .accessId;
}

AccessId
AccessorFacade::CreateSnapshotByDataIds(
    const std::vector<RIDataId>& ids)
{
    if (ids.empty())
    {
        return {};
    }

    return snapshotAccessor_
        .CreateSnapshot(
            ids)
        .accessId;
}

} // namespace rim