#include <gtest/gtest.h>

#include "adapter/rule/IdentityRule.hpp"

#include "datastore/RIMValueAccessor.hpp"
#include "datastore/RIMValueFactory.hpp"

TEST(
    IdentityRuleTest,
    ReturnInputValue)
{
    rim::IdentityRule rule;

    auto input =
        rim::RIMValueFactory::CreateInt32(
            123);

    auto output =
        rule.Execute(
            input);

    std::int32_t value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            output,
            value));

    EXPECT_EQ(
        value,
        123);
}