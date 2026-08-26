#include <gtest/gtest.h>

#include "ProductContext.hpp"
#include "PrinterAProductDefinition.hpp"
#include "printer_a.h"

TEST(
    ProductContextTest,
    FindDataItemById)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    EXPECT_NE(
        context.FindDataItem(
            RI_DATA_ERROR_LIST),
        nullptr);
}

TEST(
    ProductContextTest,
    FindCapabilityById)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    EXPECT_NE(
        context.FindCapability(
            RI_CAPABILITY_ENVIRONMENT),
        nullptr);
}

TEST(
    ProductContextTest,
    UnknownDataReturnsNull)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    EXPECT_EQ(
        context.FindDataItem(
            static_cast<RIDataId>(
                0xFFFFFFFF)),
        nullptr);
}

TEST(
    ProductContextTest,
    RepeatedLookupReturnsSamePointer)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    auto* first =
        context.FindDataItem(
            RI_DATA_ERROR_LIST);

    auto* second =
        context.FindDataItem(
            RI_DATA_ERROR_LIST);

    EXPECT_EQ(
        first,
        second);
}