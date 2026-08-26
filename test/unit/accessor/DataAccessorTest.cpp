#include <gtest/gtest.h>

#include <algorithm>

#include "DataAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "BinaryInfo.hpp"

#include "test/support/AccessorTestConstants.hpp"
#include "test/support/AccessorTestFixture.hpp"
#include "ProductContext.hpp"


namespace
{

class DataAccessorTest :
    public rim::test::AccessorTestFixture
{
protected:

    DataAccessorTest()
        :
        accessor(
            domainStore,
            productContext)
    {
    }

    rim::DataAccessor accessor;
};

} // namespace

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

TEST_F(
    DataAccessorTest,
    GetBinaryInfo)
{
    const auto domainId =
        rim::DataDomainMap(
            rim::kPrinterAProductDefinition)
            .Find(
                RI_DATA_ERROR_LIST);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    auto& storage =
        domainStore.GetOrCreate(
            domainId);

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            data,
            sizeof(data));

    storage.Store(
        item);

    rim::BinaryInfo info{};

    ASSERT_TRUE(
        accessor.GetBinaryInfo(
            RI_DATA_ERROR_LIST,
            info));

    ASSERT_NE(
        info.bytes,
        nullptr);

    EXPECT_EQ(
        info.size,
        sizeof(data));

    EXPECT_NE(
        info.hash,
        0ULL);

    EXPECT_EQ(
        info.bytes[0],
        0x11);

    EXPECT_EQ(
        info.bytes[1],
        0x22);

    EXPECT_EQ(
        info.bytes[2],
        0x33);
}

TEST_F(
    DataAccessorTest,
    GetBinaryInfoUnknownReturnsFalse)
{
    rim::BinaryInfo info{};

    EXPECT_FALSE(
        accessor.GetBinaryInfo(
            RI_DATA_ERROR_LIST,
            info));
}
