#include "FacadeManager.hpp"

#include "FacadeItemDefinition.hpp"
#include "RIMDataItem.hpp"
#include "DomainId.hpp"

namespace rim
{

bool FacadeManager::Evaluate(
    const RIMSnapshot& snapshot,
    const FacadeItemDefinition* facade)
{
    const RIMValue newValue =
        facade->build(
            snapshot);

    const DomainId domainId =
        context_.FindFacadeDomainId(
            facade->id);

    if (domainId ==
        kInvalidDomainId)
    {
        return false;
    }

    auto& storage =
        store_.GetOrCreate(
            domainId);

    RIMDataItem storedItem{};

    const bool exists =
        storage.Find(
            static_cast<RIDataId>(
                facade->id),
            storedItem);

    const RIMValue* oldValue =
        exists
            ? &storedItem.value
            : nullptr;

    bool changed = true;

    if (oldValue != nullptr)
    {
        changed =
            facade->diff(
                *oldValue,
                newValue);
    }

    if (changed)
    {
        RIMDataItem item{};

        item.id =
            static_cast<RIDataId>(
                facade->id);

        item.value =
            newValue;

        storage.Store(
            item);
    }

    return changed;
}

} // namespace rim