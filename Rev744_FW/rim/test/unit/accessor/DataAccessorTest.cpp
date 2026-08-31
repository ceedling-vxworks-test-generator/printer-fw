#include <gtest/gtest.h>

#include "DataAccessor.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "ValueStore.hpp"

TEST(
    DataAccessorTest,
    GetData)
{
    rim::ValueStore store;

    rim::RIMDataItem item
    {
        rim::RIMDataId::kTemperatureSensorA,
        rim::ValueType::kDouble,
        rim::RIMValueFactory::CreateDouble(
            25.0)
    };

    store.Store(
        item);

    rim::DataAccessor accessor(
        store,
        rim::kPrinterAProductDefinition);

    rim::RIMDataItem out{};

    EXPECT_TRUE(
        accessor.TryGetData(
            rim::RIMDataId::kTemperatureSensorA,
            out));

    EXPECT_EQ(
        out.id,
        rim::RIMDataId::kTemperatureSensorA);
}

TEST(
    DataAccessorTest,
    GetDomain)
{
    rim::ValueStore store;

    rim::DataAccessor accessor(
        store,
        rim::kPrinterAProductDefinition);

    std::string_view domain;

    EXPECT_TRUE(
        accessor.TryGetDomain(
            rim::RIMDataId::kTemperatureSensorA,
            domain));

    EXPECT_FALSE(
        domain.empty());
}

TEST(
    DataAccessorTest,
    EnumerateDomains)
{
    rim::ValueStore store;

    rim::DataAccessor accessor(
        store,
        rim::kPrinterAProductDefinition);

    const auto domains =
        accessor.GetDomains();

    EXPECT_FALSE(
        domains.empty());
}