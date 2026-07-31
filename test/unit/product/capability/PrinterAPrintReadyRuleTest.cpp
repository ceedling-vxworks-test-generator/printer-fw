#include <gtest/gtest.h>

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRules.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "ErrorRepository.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// 印字可否 = 扉が全て閉じている かつ 異常が無い。
//

namespace
{

rim::RIMSnapshot MakeDoorSnapshot(
    bool upperOpen,
    bool rightOpen,
    bool leftOpen)
{
    rim::RIMSnapshot snapshot{};

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kUpperDoorOpen,
        upperOpen);

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kRightDoorOpen,
        rightOpen);

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kLeftDoorOpen,
        leftOpen);

    return snapshot;
}

bool EvaluateReady(
    const rim::RIMSnapshot& snapshot,
    const rim::ErrorRepository& repository)
{
    rim::PrintReadyCapabilityRule rule(
        repository);

    rim::CapabilityPayload payload;

    if (!rule.Evaluate(
            snapshot,
            payload))
    {
        return false;
    }

    const auto* cap =
        payload.As<
            rim::PrintReadyCapability>();

    return cap != nullptr
        && cap->ready;
}

}

TEST(
    PrinterAPrintReadyRuleTest,
    ReadyWhenAllDoorsClosedAndNoError)
{
    rim::ErrorRepository repository;

    EXPECT_TRUE(
        EvaluateReady(
            MakeDoorSnapshot(
                false,
                false,
                false),
            repository));
}

TEST(
    PrinterAPrintReadyRuleTest,
    NotReadyWhenUpperDoorOpen)
{
    rim::ErrorRepository repository;

    EXPECT_FALSE(
        EvaluateReady(
            MakeDoorSnapshot(
                true,
                false,
                false),
            repository));
}

TEST(
    PrinterAPrintReadyRuleTest,
    NotReadyWhenRightDoorOpen)
{
    rim::ErrorRepository repository;

    EXPECT_FALSE(
        EvaluateReady(
            MakeDoorSnapshot(
                false,
                true,
                false),
            repository));
}

TEST(
    PrinterAPrintReadyRuleTest,
    NotReadyWhenLeftDoorOpen)
{
    rim::ErrorRepository repository;

    EXPECT_FALSE(
        EvaluateReady(
            MakeDoorSnapshot(
                false,
                false,
                true),
            repository));
}

TEST(
    PrinterAPrintReadyRuleTest,
    NotReadyWhenErrorExists)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    EXPECT_FALSE(
        EvaluateReady(
            MakeDoorSnapshot(
                false,
                false,
                false),
            repository));
}

TEST(
    PrinterAPrintReadyRuleTest,
    ReportsItsOwnCapabilityId)
{
    rim::ErrorRepository repository;

    rim::PrintReadyCapabilityRule rule(
        repository);

    EXPECT_EQ(
        rule.Id(),
        rim::kCapPrintReady);
}
