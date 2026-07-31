#pragma once

#include <vector>

#include "IDataDefinitionProvider.hpp"
#include "RIMDataDefinition.hpp"

namespace rim
{

class SkeletonDataDefinitionProvider
    : public IDataDefinitionProvider
{
public:

    const std::vector<RIMDataDefinition>&
    GetDefinitions() const override;
};

} // namespace rim
