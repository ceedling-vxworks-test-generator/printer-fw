#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "DomainId.hpp"
#include "DomainDefinition.hpp"   // 追加

namespace rim
{
    
class DomainRegistry
{
public:

    DomainId
    GetOrCreate(
        const DomainDefinition& definition);

    DomainId
    Find(
        const DomainDefinition& definition) const;

    std::string
    GetName(
        DomainId id) const;

    std::size_t
    Size() const;

private:

    mutable std::mutex
        m_;

    std::unordered_map<
        const DomainDefinition*,
        DomainId>
        definitionToId_;

    std::vector<
        std::string>
        idToName_;
};

} // namespace rim