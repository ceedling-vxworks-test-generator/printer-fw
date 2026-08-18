#include "SnapshotAccessor.hpp"

#include "DataItemDefinition.hpp"

namespace rim
{

AccessId
SnapshotAccessor::CreateSnapshot()
{
    return storage_.Store(
        CreateFilteredSnapshot(
            nullptr));
}

AccessId
SnapshotAccessor::CreateSnapshot(
    const std::vector<std::string_view>& domains)
{
    return storage_.Store(
        CreateFilteredSnapshot(
            &domains));
}

RIMSnapshot
SnapshotAccessor::CreateFilteredSnapshot(
    const std::vector<std::string_view>* domains) const
{
    const auto source =
        reader_.Read();

    if (domains == nullptr)
    {
        return source;
    }

    RIMSnapshot filtered;

    for (const auto& item : source.items)
    {
        const auto* definition =
            rim::FindDataItem(
                product_,
                item.id);

        if (definition == nullptr)
        {
            continue;
        }

        bool matched = false;

        for (const auto& domain : *domains)
        {
            if (definition->domain ==
                domain)
            {
                matched = true;
                break;
            }
        }

        if (matched)
        {
            filtered.items.push_back(
                item);
        }
    }

    return filtered;
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