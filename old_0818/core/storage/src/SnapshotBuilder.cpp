#include "SnapshotBuilder.hpp"

namespace rim
{

SnapshotBuilder::SnapshotBuilder(
    const DomainStorageRegistry& store,
    const ProductDefinition& product)
    :
    store_(store),
    product_(product),
    domainMap_(product)
{
}

void
SnapshotBuilder::UpdateSnapshot(
    RIMSnapshot& snapshot) const
{
    snapshot.items.clear();

    const auto items =
        store_.GetAll();

    for (const auto& item : items)
    {
        const DomainId itemDomainId =
            domainMap_.Find(
                item.id);

        if (itemDomainId ==
            kInvalidDomainId)
        {
            continue;
        }

        for (const auto domain :
             snapshot.domains)
        {
            if (itemDomainId ==
                domain)
            {
                snapshot.items.push_back(
                    item);

                break;
            }
        }
    }
}

} // namespace rim