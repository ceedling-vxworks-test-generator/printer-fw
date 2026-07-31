#include <gtest/gtest.h>

#include "PrinterAProductDefinition.hpp"
#include "ProductDefinition.hpp"

TEST(ProductDefinitionTest,
     PrinterAProductDefinitionIsValid)
{
    EXPECT_TRUE(
        rim::ValidateProductDefinition(
            rim::kPrinterAProductDefinition));
}