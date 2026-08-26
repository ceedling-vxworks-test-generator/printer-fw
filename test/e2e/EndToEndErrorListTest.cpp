#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <cstring>

#include "printer_a.h"

#include "DataDomainMap.hpp"

#include "RIMValueFactory.hpp"
#include "PartitionStorageRegistry.hpp"
#include "SnapshotBuilder.hpp"
#include "PrinterAProductDefinition.hpp"
#include "ProductContext.hpp"

TEST(
    EndToEndErrorListTest,
    FaultInfoListRoundTrip)
{
    struct FaultInfo
    {
        std::uint32_t code;
        std::uint32_t state;
    };

    struct FaultInfoList
    {
        FaultInfo entries[20];
        std::uint32_t count;
    };

    rim::PartitionStorageRegistry store;

    FaultInfoList input{};

    input.count = 1U;

    input.entries[0].code = 1234U;
    input.entries[0].state = 1U;

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    // item.value.type =
    //     rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            reinterpret_cast<
                const std::uint8_t*>(
                    &input),
            sizeof(input));

    const rim::DataDomainMap domainMap(
        rim::kPrinterAProductDefinition);

    const rim::DomainId domainId =
        domainMap.Find(
            RI_DATA_ERROR_LIST);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    store
        .GetOrCreate(
            domainId)
        .Store(
            item);

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

    rim::SnapshotBuilder builder(
        store);

    auto snapshot =
        builder.Build(
            {
                domainId
            });

    const std::uint8_t* found{};
    std::size_t size{};

    ASSERT_TRUE(
        snapshot.TryGetBinary(
            RI_DATA_ERROR_LIST,
            found,
            size));

    ASSERT_NE(
        found,
        nullptr);

    FaultInfoList output{};

    std::memcpy(
        &output,
        found,
        sizeof(output));

    EXPECT_EQ(
        output.count,
        1U);

    EXPECT_EQ(
        output.entries[0].code,
        1234U);

    EXPECT_EQ(
        output.entries[0].state,
        1U);
}