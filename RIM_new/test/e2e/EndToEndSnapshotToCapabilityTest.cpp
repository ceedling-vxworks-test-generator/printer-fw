#include <gtest/gtest.h>

#if 1
#include <gtest/gtest.h>

#include "datastore/RIMSnapshot.hpp"
#include "capability/EnvironmentRule.hpp"
#include "capability/PrintReadyRule.hpp"

TEST(
    EndToEndSnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    snapshot.temperature = 30;
    snapshot.humidity = 40;
    snapshot.upperDoorOpen = false;
    snapshot.rightDoorOpen = false;
    snapshot.leftDoorOpen = false;

    rim::EnvironmentRule envRule;
    rim::PrintReadyRule readyRule;

    auto env = envRule.Evaluate(snapshot);
    auto ready = readyRule.Evaluate(snapshot);

    EXPECT_DOUBLE_EQ(
        env.temperature,
        30.0);

    EXPECT_DOUBLE_EQ(
        env.humidity,
        40.0);

    EXPECT_TRUE(
        ready.ready);
}
#else
namespace rim {
struct RIMSnapshot {
    int temperature{0};
    int humidify{0};
    bool hasError{false};
};
struct EnvironmentCapability {
    double temperature;
    double humidify;
};
struct PrintReadyCapability {
    bool ready;
};
class EnvironmentRule {
public:
    EnvironmentCapability Evaluate(const RIMSnapshot& s) const {
        return {static_cast<double>(s.temperature), static_cast<double>(s.humidify)};
    }
};
class PrintReadyRule {
public:
    PrintReadyCapability Evaluate(const RIMSnapshot& s) const {
        return {!s.hasError};
    }
};
}

TEST(EndToEndSnapshotToCapabilityTest, GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};
    snapshot.temperature = 30;
    snapshot.humidity = 40;
    snapshot.upperDoorOpen = false;
    snapshot.rightDoorOpen = false;
    snapshot.leftDoorOpen = false;

    rim::EnvironmentRule envRule;
    rim::PrintReadyRule readyRule;

    const auto env = envRule.Evaluate(snapshot);
    const auto ready = readyRule.Evaluate(snapshot);

    EXPECT_DOUBLE_EQ(env.temperature, 30.0);
    EXPECT_DOUBLE_EQ(env.humidity, 40.0);
    EXPECT_TRUE(ready.ready);
}
#endif