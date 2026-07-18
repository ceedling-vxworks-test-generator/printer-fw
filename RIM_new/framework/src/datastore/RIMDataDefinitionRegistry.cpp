#include "datastore/RIMDataDefinitionRegistry.hpp"

namespace rim
{

namespace
{

const RIMDataDefinition kDefinitions[] =
{
    {
        RIMDataId::kTemperature,
        DataDomain::kDevice,
        ValueType::kDouble,
        "Temperature"
    },
    {
        RIMDataId::kHumidify,
        DataDomain::kDevice,
        ValueType::kDouble,
        "Humidify"
    },
    {
        RIMDataId::kDoorUpper,
        DataDomain::kDevice,
        ValueType::kBool,
        "DoorUpper"
    },

    {
        RIMDataId::kDoorRight,
        DataDomain::kDevice,
        ValueType::kBool,
        "DoorRight"
    },

    {
        RIMDataId::kDoorLeft,
        DataDomain::kDevice,
        ValueType::kBool,
        "DoorLeft"
    },
    {
        RIMDataId::kCurrentError,
        DataDomain::kError,
        ValueType::kStruct,
        "CurrentError"
    },
    {
        RIMDataId::kStapleLevel,
        DataDomain::kConsumable,
        ValueType::kUInt32,
        "StapleLevel"
    }
};

} // namespace

const RIMDataDefinition*
RIMDataDefinitionRegistry::Find(
    RIMDataId id)
{
    for (const auto& def : kDefinitions)
    {
        if (def.id == id)
        {
            return &def;
        }
    }

    return nullptr;
}

} // namespace rim
