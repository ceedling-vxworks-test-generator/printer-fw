#pragma once

#include <vector>

#include "datastore/RIMDataDefinition.hpp"
#include "datastore/IDataDefinitionProvider.hpp"
#include "datastore/SensorDefinition.hpp"

namespace rim
{

class PrinterAProvider
    : public IDataDefinitionProvider
{
public:

    const std::vector<RIMDataDefinition>&
    GetDefinitions() const override;

    const std::vector<SensorDefinition>&
    GetSensorDefinitions() const;
};

}