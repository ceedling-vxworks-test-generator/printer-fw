#pragma once

#include <string_view>

#include "IDataDefinitionProvider.hpp"

namespace rim {

class ProfileAwareDataDefinitionRegistry {
    const IDataDefinitionProvider& p_;

public:
    explicit ProfileAwareDataDefinitionRegistry(
        const IDataDefinitionProvider& p)
        : p_(p)
    {
    }

    const RIMDataDefinition*
    Find(
        RIMDataId id) const;

    const RIMDataDefinition*
    FindByName(
        std::string_view name) const;
};

}