#pragma once

#include "datastore/RIMDataId.hpp"
#include "datastore/SensorData.hpp"
#include "datastore/ValueStore.hpp"

namespace rim
{

class HumidityAdapter
{
public:

    bool Store(
        double humidityPercent,
        ValueStore& store)
    {
        SensorData data{};

        data.id =
            RIMDataId::HumidityRelative;

        data.value =
            humidityPercent;

        store.Store(data);

        return true;
    }
};

} // namespace rim
