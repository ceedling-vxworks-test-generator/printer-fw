#include <gtest/gtest.h>

#include "rim_api_common.h"

TEST(RIMApiTest, GetVersionInfoReturnsCurrentVersion)
{
    RIM_VERSION_INFO version{};

    ASSERT_EQ(
        RI_SUCCESS,
        RIM_GetVersionInfo(
            &version));

    EXPECT_EQ(
        1,
        version.core.major);

    EXPECT_EQ(
        0,
        version.core.minor);

    EXPECT_EQ(
        0,
        version.core.patch);

    EXPECT_EQ(
        2,
        version.product.major);

    EXPECT_EQ(
        3,
        version.product.minor);

    EXPECT_EQ(
        4,
        version.product.patch);
}

TEST(RIMApiTest, GetVersionInfoReturnsInvalidParameterWhenNullptr)
{
    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIM_GetVersionInfo(
            nullptr));
}

TEST(RIMApiTest, GetVersionStringReturnsExpectedString)
{
    EXPECT_STREQ(
        "Product/2.3.4 Core/1.0.0",
        RIM_GetVersionString());
}

TEST(RIMApiTest, GetProductNameReturnsExpectedName)
{
    EXPECT_STREQ(
        "Test Product",
        RIM_GetProductName());
}