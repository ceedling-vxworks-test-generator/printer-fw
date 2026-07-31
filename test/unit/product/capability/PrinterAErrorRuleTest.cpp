#include <gtest/gtest.h>

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRules.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "ErrorRepository.hpp"
#include "RIMSnapshot.hpp"

//
// 異常一覧の Capability。
//
// 旧 ErrorCapability は std::vector<ErrorInfo> を持っていたが、CapabilityPayload に
// 入れるには trivially copyable でなければならないため、固定容量配列 + count +
// overflow に変えてある。上限超過時の振る舞いもここで確認する。
//

TEST(
    PrinterAErrorRuleTest,
    BuildErrorCapabilityFromRepository)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    repository.Add(
        {
            1002,
            rim::ErrorState::kRecovered,
            rim::ErrorSeverity::kWarning
        });

    rim::ErrorCapabilityRule rule(
        repository);

    rim::RIMSnapshot snapshot{};

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::ErrorCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    ASSERT_EQ(
        cap->count,
        2U);

    EXPECT_EQ(
        cap->errors[0].errorCode,
        1001U);

    EXPECT_EQ(
        cap->errors[1].errorCode,
        1002U);

    EXPECT_FALSE(
        cap->overflow);
}

TEST(
    PrinterAErrorRuleTest,
    EmptyRepositoryYieldsEmptyCapability)
{
    rim::ErrorRepository repository;

    rim::ErrorCapabilityRule rule(
        repository);

    rim::RIMSnapshot snapshot{};

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::ErrorCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_EQ(
        cap->count,
        0U);

    EXPECT_FALSE(
        cap->overflow);
}

TEST(
    PrinterAErrorRuleTest,
    OverflowIsTruncatedAndFlagged)
{
    rim::ErrorRepository repository;

    // 上限 +3 件を投入する
    for (std::uint32_t i = 0;
         i < rim::kPrinterAMaxErrors + 3;
         ++i)
    {
        repository.Add(
            {
                2000 + i,
                rim::ErrorState::kActive,
                rim::ErrorSeverity::kError
            });
    }

    rim::ErrorCapabilityRule rule(
        repository);

    rim::RIMSnapshot snapshot{};

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::ErrorCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    // 上限で打ち切られ、切り捨てが起きたことが観測できる
    EXPECT_EQ(
        static_cast<std::size_t>(
            cap->count),
        rim::kPrinterAMaxErrors);

    EXPECT_TRUE(
        cap->overflow);
}

TEST(
    PrinterAErrorRuleTest,
    ReportsItsOwnCapabilityId)
{
    rim::ErrorRepository repository;

    rim::ErrorCapabilityRule rule(
        repository);

    EXPECT_EQ(
        rule.Id(),
        rim::kCapError);
}
