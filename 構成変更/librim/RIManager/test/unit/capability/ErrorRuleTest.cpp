#include <gtest/gtest.h>

#include "ErrorRule.hpp"
#include "ErrorRepository.hpp"

TEST(
    ErrorRuleTest,
    EmptyRepository)
{
    rim::ErrorRepository repository;

    rim::ErrorRule rule;

    const auto capability =
        rule.Evaluate(
            repository);

    EXPECT_TRUE(
        capability.errors.empty());
}

TEST(
    ErrorRuleTest,
    SingleError)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    rim::ErrorRule rule;

    const auto capability =
        rule.Evaluate(
            repository);

    ASSERT_EQ(
        capability.errors.size(),
        1U);

    EXPECT_EQ(
        capability.errors.front().errorCode,
        1001U);

    EXPECT_EQ(
        capability.errors.front().state,
        rim::ErrorState::kActive);

    EXPECT_EQ(
        capability.errors.front().severity,
        rim::ErrorSeverity::kError);

}

TEST(
    ErrorRuleTest,
    MultipleErrors)
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
            rim::ErrorState::kRecovered
        });

    rim::ErrorRule rule;

    const auto capability =
        rule.Evaluate(
            repository);

    ASSERT_EQ(
        capability.errors.size(),
        2U);

    EXPECT_EQ(
        capability.errors[0].errorCode,
        1001U);

    EXPECT_EQ(
        capability.errors[1].errorCode,
        1002U);
}

TEST(
    ErrorRuleTest,
    DuplicateErrorRejected)
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
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    rim::ErrorRule rule;

    const auto capability =
        rule.Evaluate(
            repository);

    EXPECT_EQ(
        capability.errors.size(),
        1U);
}

