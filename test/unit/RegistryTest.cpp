#include <gtest/gtest.h>

#include "datastore/FaultRegistry.hpp"
#include "datastore/OperationRegistry.hpp"
#include "datastore/CurrentValueRegistry.hpp"

using namespace rim;

namespace
{
RIMDataItem Fault(std::uint32_t code, FaultState st)
{
    RIMDataItem it;
    it.id = RIMDataId::kFaultCode;
    it.value = RIMValue::FaultCode(code);
    it.context.faultState = st;
    return it;
}
} // namespace

TEST(FaultRegistryTest, RaiseClearAndSnapshot)
{
    FaultRegistry reg;

    EXPECT_EQ(reg.Apply(Fault(0x10, FaultState::kRaised)), kDomainFault);
    EXPECT_EQ(reg.MakeSnapshot().activeCount, 1u);

    // 同一コードの再Raiseは変化なし。
    EXPECT_FALSE(reg.Apply(Fault(0x10, FaultState::kRaised)).has_value());

    // Heal → active 0 / healed 1
    EXPECT_EQ(reg.Apply(Fault(0x10, FaultState::kUpdatedHeal)), kDomainFault);
    EXPECT_EQ(reg.MakeSnapshot().activeCount, 0u);
    EXPECT_EQ(reg.MakeSnapshot().healedCount, 1u);

    // Clear
    EXPECT_EQ(reg.Apply(Fault(0x10, FaultState::kCleared)), kDomainFault);
    EXPECT_EQ(reg.MakeSnapshot().activeCount, 0u);
    EXPECT_EQ(reg.MakeSnapshot().healedCount, 0u);
}

TEST(OperationRegistryTest, JobProgress)
{
    OperationRegistry reg;
    RIMDataItem j; j.id = RIMDataId::kJobProgress; j.value = RIMValue::JobProgress(50);

    EXPECT_EQ(reg.Apply(j), kDomainOperation);
    EXPECT_FALSE(reg.Apply(j).has_value());  // 同値は変化なし

    auto snap = reg.MakeSnapshot();
    EXPECT_TRUE(snap.jobPresent);
    EXPECT_EQ(snap.jobProgress, 50);
    EXPECT_TRUE(snap.jobActive);
}

TEST(CurrentValueRegistryTest, ApplyAndDomainSnapshot)
{
    CurrentValueRegistry reg;

    RIMDataItem t; t.id = RIMDataId::kTemperature; t.value = RIMValue::CelsiusX100(2500);
    EXPECT_EQ(reg.Apply(t), kDomainEnvironment);
    EXPECT_FALSE(reg.Apply(t).has_value());  // 同値は変化なし

    RIMDataItem ink; ink.id = RIMDataId::kInkLevel; ink.value = RIMValue::Percent(5);
    EXPECT_EQ(reg.Apply(ink), kDomainConsumable);

    auto envSnap = reg.MakeSnapshot(kDomainEnvironment);
    ASSERT_TRUE(envSnap.environment.has_value());
    ASSERT_TRUE(envSnap.environment->temperatureX100.has_value());
    EXPECT_EQ(*envSnap.environment->temperatureX100, 2500);
    // Environment のみ要求 → consumable は未生成。
    EXPECT_FALSE(envSnap.consumable.has_value());

    auto conSnap = reg.MakeSnapshot(kDomainConsumable);
    ASSERT_TRUE(conSnap.consumable.has_value());
    ASSERT_TRUE(conSnap.consumable->inkLevel.has_value());
    EXPECT_EQ(*conSnap.consumable->inkLevel, 5);
}
