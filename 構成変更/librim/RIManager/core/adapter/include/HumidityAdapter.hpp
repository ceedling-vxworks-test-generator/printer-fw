#pragma once

#include "RIMDataId.hpp"
#include "SensorData.hpp"
#include "ValueStore.hpp"

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
