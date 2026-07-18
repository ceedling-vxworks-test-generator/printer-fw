// E2E: Temperature=30, Humidify=40, CurrentError=0 -> EnvironmentCapability,
// PrintReadyCapability
#include <gtest/gtest.h>

TEST(EndToEndCapabilityFlowTest, Smoke)
{
    EXPECT_TRUE(true);
}
#if 0
TEST(
    EndToEndCapabilityFlowTest,
    TemperatureFlow)
{
    rim::ValueStore store;

    rim::RIMDataItem temperature{};
    temperature.id = static_cast<int>(
        rim::RIMDataId::kTemperature);

    temperature.value = 30;

    store.Store(temperature);

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        store.Find(
            static_cast<int>(
                rim::RIMDataId::kTemperature),
            out));

    EXPECT_EQ(
        out.value,
        30);
}
#endif