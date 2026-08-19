// #include <iostream>

#include "ProductDefinition.hpp"

#include "DomainDefinition.hpp"
#include "DataItemDefinition.hpp"
#include "CapabilityItem/CapabilityItemDefinition.hpp"
#include "Pipeline/PipelineDefinition.hpp"

namespace
{

template<typename T, typename Getter>
bool HasDuplicate(
    const T* items,
    std::size_t count,
    Getter getter)
{
    if (items == nullptr)
    {
        return false;
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        for (std::size_t j = i + 1;
             j < count;
             ++j)
        {
            if (getter(items[i]) ==
                getter(items[j]))
            {
                return true;
            }
        }
    }

    return false;
}

} // namespace

namespace rim
{

const DomainDefinition*
FindDomain(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.domainCount; ++i)
    {
        const auto& domain = product.domains[i];

        if (domain.name == name)
        {
            return &domain;
        }
    }

    return nullptr;
}

const DataItemDefinition*
FindDataItem(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.dataItemCount; ++i)
    {
        const auto& item = product.dataItems[i];

        if (item.name == name)
        {
            return &item;
        }
    }

    return nullptr;
}

const DataItemDefinition*
FindDataItem(
    const ProductDefinition& product,
    RIMDataId id)
{
    for (std::size_t i{}; i < product.dataItemCount; ++i)
    {
        if (product.dataItems[i].id == id)
        {
            return &product.dataItems[i];
        }
    }

    return nullptr;
}

const CapabilityItemDefinition*
FindCapability(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0; i < product.capabilityCount; ++i)
    {
        const auto& capability = product.capabilities[i];

        if (capability.name == name)
        {
            return &capability;
        }
    }

    return nullptr;
}

const CapabilityItemDefinition*
FindCapability(
    const ProductDefinition& product,
    RICapabilityId id)
{
    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        if (product.capabilities[i].id == id)
        {
            return &product.capabilities[i];
        }
    }

    return nullptr;
}

const PipelineDefinition*
FindPipeline(
    const ProductDefinition& product,
    std::string_view name)
{
    for (std::size_t i = 0;
         i < product.pipelineCount;
         ++i)
    {
        const auto& pipeline =
            product.pipelines[i];

        if (pipeline.name == name)
        {
            return &pipeline;
        }
    }

    return nullptr;
}

bool ValidateProductDefinition(
    const ProductDefinition& product)
{
    //
    // Domain uniqueness
    //

    if (HasDuplicate(
            product.domains,
            product.domainCount,
            [&](const DomainDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // DataItem name uniqueness
    //

    if (HasDuplicate(
            product.dataItems,
            product.dataItemCount,
            [&](const DataItemDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // DataItem id uniqueness
    //

    if (HasDuplicate(
            product.dataItems,
            product.dataItemCount,
            [&](const DataItemDefinition& item)
            {
                return item.id;
            }))
    {
        return false;
    }

    //
    // Capability uniqueness
    //

    if (HasDuplicate(
            product.capabilities,
            product.capabilityCount,
            [&](const CapabilityItemDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // Capability id uniqueness
    //

    if (HasDuplicate(
            product.capabilities,
            product.capabilityCount,
            [&](const CapabilityItemDefinition& item)
            {
                return item.id;
            }))
    {
        return false;
    }

    //
    // Pipeline uniqueness
    //

    if (HasDuplicate(
            product.pipelines,
            product.pipelineCount,
            [&](const PipelineDefinition& item)
            {
                return item.name;
            }))
    {
        return false;
    }

    //
    // Capability → DataItem
    //

    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        const auto& capability =
            product.capabilities[i];

        for (std::size_t j = 0;
             j < capability.requiredDataItemCount;
             ++j)
        {
            const auto* dataItem =
                FindDataItem(
                    product,
                    capability.requiredDataItems[j]);

            if (dataItem == nullptr)
            {
                return false;
            }

        }
    }

    //
    // Pipeline → Domain
    //

    for (std::size_t i = 0;
         i < product.pipelineCount;
         ++i)
    {
        const auto& pipeline =
            product.pipelines[i];

        for (std::size_t j = 0;
             j < pipeline.triggerDomainCount;
             ++j)
        {
            if (!FindDomain(
                    product,
                    pipeline.triggerDomains[j]))
            {
                // std::cout
                //     << "Missing Domain: "
                //     << pipeline.triggerDomains[j]
                //     << std::endl;

                return false;
            }
        }

        if (!FindCapability(
                product,
                pipeline.outputCapability))
        {

            
            return false;
        }
    }

    return true;
}

const CapabilityItemDefinition*
GetCapabilities(
    const ProductDefinition& product)
{
    return product.capabilities;
}

std::size_t
GetCapabilityCount(
    const ProductDefinition& product)
{
    return product.capabilityCount;
}

const PipelineDefinition*
GetPipelines(
    const ProductDefinition& product)
{
    return product.pipelines;
}

std::size_t
GetPipelineCount(
    const ProductDefinition& product)
{
    return product.pipelineCount;
}

const DataItemDefinition*
GetDataItems(
    const ProductDefinition& product)
{
    return product.dataItems;
}

std::size_t
GetDataItemCount(
    const ProductDefinition& product)
{
    return product.dataItemCount;
}

const DomainDefinition*
GetDomains(
    const ProductDefinition& product)
{
    return product.domains;
}

std::size_t
GetDomainCount(
    const ProductDefinition& product)
{
    return product.domainCount;
}


} // namespace rim