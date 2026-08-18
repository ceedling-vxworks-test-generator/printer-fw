#include <algorithm>

#include "SnapshotAccessor.hpp"
#include "DomainId.hpp"
#include "DataItemDefinition.hpp"

namespace
{

void
AppendUniqueDomains(
    std::vector<rim::DomainId>& destination,
    const std::vector<rim::DomainId>& source)
{
    for (const auto& domain : source)
    {
        if (std::find(
                destination.begin(),
                destination.end(),
                domain)
            == destination.end())
        {
            destination.push_back(
                domain);
        }
    }
}

}

namespace rim
{

SnapshotResult
SnapshotAccessor::CreateSnapshot(
    RIDataId changedDataId)
{
    return CreateSnapshot(
        std::vector<RIDataId>
        {
            changedDataId
        });
}

SnapshotResult
SnapshotAccessor::CreateSnapshot(
    const std::vector<RIDataId>& changedDataIds)
{
    SnapshotResult result;

    RIMSnapshot snapshot;

    for (const auto dataId :
         changedDataIds)
    {
        const auto& capabilities =
            dependencyMap_.Find(
                dataId);

        for (const auto* capability :
             capabilities)
        {
            const auto domains =
                domainResolver_.
                    ResolveDomains(
                        *capability);

            AppendUniqueDomains(
                snapshot.domains,
                domains);
        }
    }

    if (snapshot.domains.empty())
    {
        return result;
    }

    snapshotBuilder_.
        UpdateSnapshot(
            snapshot);

    result.accessId =
        storage_.Store(
            snapshot);

    result.snapshot =
        snapshot;

    return result;
}

bool
SnapshotAccessor::TryGetSnapshot(
    AccessId id,
    RIMSnapshot& snapshot) const
{
    return storage_.TryGet(
        id,
        snapshot);
}

} // namespace rim