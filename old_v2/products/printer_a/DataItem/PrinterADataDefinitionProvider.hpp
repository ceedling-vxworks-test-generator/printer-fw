#pragma once

#include <vector>

#include "IDataDefinitionProvider.hpp"
#include "RIMDataDefinition.hpp"
#include "SensorDefinition.hpp"

namespace rim
{

class PrinterADataDefinitionProvider
    : public IDataDefinitionProvider
{
public:

    const std::vector<RIMDataDefinition>&
    GetDefinitions() const override;

    const std::vector<SensorDefinition>&
    GetSensorDefinitions() const;
};

} // namespace rim