// GoogleTest skeleton
#include <gtest/gtest.h>

#include "datastore/RIMDataDefinitionRegistry.hpp"

TEST(RIMDataDefinitionRegistryTest, TemperatureFound)
{
    auto* def =
        rim::RIMDataDefinitionRegistry::Find(
            rim::RIMDataId::kTemperature);

    ASSERT_NE(def, nullptr);
}
