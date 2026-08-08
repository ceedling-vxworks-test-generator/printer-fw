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
        RI_DATA_TEMPERATURE_SENSOR_A,
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
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));

    EXPECT_EQ(
        out.id,
        RI_DATA_TEMPERATURE_SENSOR_A);
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
            RI_DATA_TEMPERATURE_SENSOR_A,
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