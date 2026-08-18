#include <gtest/gtest.h>

#include <algorithm>

#include "DataAccessor.hpp"

#include "test/support/AccessorTestConstants.hpp"
#include "test/support/AccessorTestFixture.hpp"

namespace
{

class DataAccessorTest :
    public ::testing::Test,
    protected rim::test::AccessorTestFixture
{
protected:

    DataAccessorTest()
        :
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition)
    {
    }

    rim::DataAccessor accessor;
};

}

TEST_F(
    DataAccessorTest,
    GetData)
{
    StoreTemperatureA();

    rim::RIMDataItem out{};

    EXPECT_TRUE(
        accessor.TryGetData(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));

    EXPECT_EQ(
        out.id,
        RI_DATA_TEMPERATURE_SENSOR_A);
}

TEST_F(
    DataAccessorTest,
    ReturnFalseWhenDataNotFound)
{
    rim::RIMDataItem out{};

    EXPECT_FALSE(
        accessor.TryGetData(
            rim::test::kUnknownDataId,
            out));
}

TEST_F(
    DataAccessorTest,
    GetDomain)
{
    std::string_view domain;

    EXPECT_TRUE(
        accessor.TryGetDomain(
            RI_DATA_TEMPERATURE_SENSOR_A,
            domain));

    EXPECT_FALSE(
        domain.empty());
}

TEST_F(
    DataAccessorTest,
    ReturnFalseWhenDomainNotFound)
{
    std::string_view domain;

    EXPECT_FALSE(
        accessor.TryGetDomain(
            rim::test::kUnknownDataId,
            domain));
}

TEST_F(
    DataAccessorTest,
    EnumerateDomains)
{
    const auto domains =
        accessor.GetDomains();

    EXPECT_FALSE(
        domains.empty());
}

TEST_F(
    DataAccessorTest,
    EnumeratedDomainsContainTemperatureDomain)
{
    const auto domains =
        accessor.GetDomains();

    EXPECT_FALSE(
        domains.empty());

    std::string_view domain;

    ASSERT_TRUE(
        accessor.TryGetDomain(
            RI_DATA_TEMPERATURE_SENSOR_A,
            domain));

    EXPECT_NE(
        std::find(
            domains.begin(),
            domains.end(),
            domain),
        domains.end());
}