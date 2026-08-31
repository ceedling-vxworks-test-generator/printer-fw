#include "CapabilityManager.hpp"

#include "CapabilityItemDefinition.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

bool CapabilityManager::Evaluate(
    const RIMSnapshot& snapshot,
    const CapabilityItemDefinition* capability)
{
    const RIMValue newValue =
        capability->build(
            snapshot);

    const DomainId domainId =
        context_.FindDomainId(
            RIMId
            {
                RIMIdType::Capability,
                static_cast<std::uint32_t>(
                    capability->id)
            });
                        
    if (domainId ==
        kInvalidDomainId)
    {
        return false;
    }

    // store_.RegisterDomain(
    //     domainId);

    auto* storage =
        store_.FindMutable(
            domainId);

    if (storage == nullptr)
    {
        return false;
    }

    RIMDataItem storedItem{};

    const bool exists =
        storage->Find(
            static_cast<RIDataId>(
                capability->id),
            storedItem);

    const RIMValue* oldValue =
        exists
            ? &storedItem.value
            : nullptr;

    bool changed = true;

    if (oldValue != nullptr)
    {
        changed =
            capability->diff(
                *oldValue,
                newValue);
    }

    if (changed)
    {
        RIMDataItem item{};

        item.id =
            static_cast<RIDataId>(
                capability->id);

        item.value =
            newValue;

        storage->Store(
            item);
    }

    return changed;
}

} // namespace rim