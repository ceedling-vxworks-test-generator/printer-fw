#include <gtest/gtest.h>

#include "PrintReadyRule.hpp"
#include "ErrorRepository.hpp"
#include "RIMValueFactory.hpp"

namespace
{

void AddBoolItem(
    rim::RIMSnapshot& snapshot,
    rim::RIMDataId id,
    bool value)
{
    rim::RIMDataItem item{};

    item.id = id;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            value);

    snapshot.items.push_back(
        item);
}

}

TEST(
    PrintReadyRuleTest,
    ReadyWhenNoError)
{
    rim::RIMSnapshot snapshot{};

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kUpperDoorOpen,
        false);

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kRightDoorOpen,
        false);

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kLeftDoorOpen,
        false);

    rim::ErrorRepository repository;

    rim::PrintReadyRule rule;

    const auto cap =
        rule.Evaluate(
            snapshot,
            repository);

    EXPECT_TRUE(
        cap.ready);
}

TEST(
    PrintReadyRuleTest,
    NotReadyWhenDoorOpen)
{
    rim::RIMSnapshot snapshot{};

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kUpperDoorOpen,
        true);

    rim::ErrorRepository repository;

    rim::PrintReadyRule rule;

    const auto cap =
        rule.Evaluate(
            snapshot,
            repository);

    EXPECT_FALSE(
        cap.ready);
}

TEST(
    PrintReadyRuleTest,
    NotReadyWhenErrorExists)
{
    rim::RIMSnapshot snapshot{};

    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    rim::PrintReadyRule rule;

    const auto cap =
        rule.Evaluate(
            snapshot,
            repository);

    EXPECT_FALSE(
        cap.ready);
}

