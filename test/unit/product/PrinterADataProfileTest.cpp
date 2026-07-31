#include <gtest/gtest.h>

#include "DataItem/PrinterADataProfile.hpp"
#include "RIMDataId.hpp"

//
// id -> Rule の対応表。
//
// 行数のずれは PrinterADataProfile.cpp の static_assert がビルド時に落とすが、
// **行の並び崩れ(取り違え)は検出できない**。ここで実際に引いて確認する。
//

TEST(
    PrinterADataProfileTest,
    EveryIdResolvesToARule)
{
    const rim::PrinterADataProfile profile;

    for (int i = 0; i < rim::kRIMDataIdCount; ++i)
    {
        const auto id =
            static_cast<rim::RIMDataId>(i);

        EXPECT_NE(
            profile.SelectRule(id),
            nullptr)
            << "id " << i << " に規則が対応していない";
    }
}

TEST(
    PrinterADataProfileTest,
    OutOfRangeIdReturnsNull)
{
    const rim::PrinterADataProfile profile;

    EXPECT_EQ(
        profile.SelectRule(
            static_cast<rim::RIMDataId>(
                rim::kRIMDataIdCount)),
        nullptr);

    EXPECT_EQ(
        profile.SelectRule(
            static_cast<rim::RIMDataId>(-1)),
        nullptr);
}

TEST(
    PrinterADataProfileTest,
    TemperatureIdsShareTheSameRule)
{
    // 並び崩れの検出。同じ種類の id には同じ規則が付いているはず。
    const rim::PrinterADataProfile profile;

    EXPECT_EQ(
        profile.SelectRule(
            rim::RIMDataId::kTemperatureSensorA),
        profile.SelectRule(
            rim::RIMDataId::kTemperatureSensorB));
}

TEST(
    PrinterADataProfileTest,
    DoorIdsShareTheSameRule)
{
    const rim::PrinterADataProfile profile;

    const auto* upper =
        profile.SelectRule(
            rim::RIMDataId::kUpperDoorOpen);

    EXPECT_EQ(
        upper,
        profile.SelectRule(
            rim::RIMDataId::kRightDoorOpen));

    EXPECT_EQ(
        upper,
        profile.SelectRule(
            rim::RIMDataId::kLeftDoorOpen));
}

TEST(
    PrinterADataProfileTest,
    DifferentKindsUseDifferentRules)
{
    // 温度と扉が同じ規則を指していたら、表がずれている。
    const rim::PrinterADataProfile profile;

    EXPECT_NE(
        profile.SelectRule(
            rim::RIMDataId::kTemperatureSensorA),
        profile.SelectRule(
            rim::RIMDataId::kUpperDoorOpen));
}
