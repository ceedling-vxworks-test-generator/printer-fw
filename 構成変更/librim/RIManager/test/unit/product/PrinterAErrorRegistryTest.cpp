#include <gtest/gtest.h>

#include "products/printer_a/CapabilityItem/PrinterAErrorRegistry.hpp"

TEST(
    PrinterAErrorRegistryTest,
    WarningError)
{
    rim::PrinterAErrorRegistry registry;

    rim::ErrorSeverity severity{};

    ASSERT_TRUE(
        registry.TryGetSeverity(
            1001,
            severity));

    EXPECT_EQ(
        severity,
        rim::ErrorSeverity::kWarning);
}

TEST(
    PrinterAErrorRegistryTest,
    FatalError)
{
    rim::PrinterAErrorRegistry registry;

    rim::ErrorSeverity severity{};

    ASSERT_TRUE(
        registry.TryGetSeverity(
            1003,
            severity));

    EXPECT_EQ(
        severity,
        rim::ErrorSeverity::kFatal);
}