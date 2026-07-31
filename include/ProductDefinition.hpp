#pragma once

#include <cstddef>
#include <string_view>

namespace rim
{

struct DomainDefinition;
struct DataItemDefinition;
struct CapabilityItemDefinition;
struct PipelineDefinition;

struct ProductDefinition
{
    const DomainDefinition* domains;
    std::size_t domainCount;

    const DataItemDefinition* dataItems;
    std::size_t dataItemCount;

    const CapabilityItemDefinition* capabilities;
    std::size_t capabilityCount;

    const PipelineDefinition* pipelines;
    std::size_t pipelineCount;
};

const DomainDefinition*
FindDomain(
    const ProductDefinition& product,
    std::string_view name);

const DataItemDefinition*
FindDataItem(
    const ProductDefinition& product,
    std::string_view name);

const CapabilityItemDefinition*
FindCapability(
    const ProductDefinition& product,
    std::string_view name);

const PipelineDefinition*
FindPipeline(
    const ProductDefinition& product,
    std::string_view name);    

const CapabilityItemDefinition*
GetCapabilities(
    const ProductDefinition& product);

std::size_t
GetCapabilityCount(
    const ProductDefinition& product);

const PipelineDefinition*
GetPipelines(
    const ProductDefinition& product);

std::size_t
GetPipelineCount(
    const ProductDefinition& product);

const DataItemDefinition*
GetDataItems(
    const ProductDefinition& product);

std::size_t
GetDataItemCount(
    const ProductDefinition& product);

const DomainDefinition*
GetDomains(
    const ProductDefinition& product);

std::size_t
GetDomainCount(
    const ProductDefinition& product);

bool ValidateProductDefinition(
    const ProductDefinition& product);  
    
    

} // namespace rim