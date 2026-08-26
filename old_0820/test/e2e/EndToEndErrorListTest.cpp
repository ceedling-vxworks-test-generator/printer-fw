#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <cstring>

#include "printer_a.h"

#include "BinaryStoreValue.hpp"
#include "DataDomainMap.hpp"

#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"
#include "SnapshotBuilder.hpp"
#include "PrinterAProductDefinition.hpp"

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

    rim::DomainStorageRegistry store;

    FaultInfoList input{};

    input.count = 1U;

    input.entries[0].code = 1234U;
    input.entries[0].state = 1U;

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data.resize(
        sizeof(input));

    std::memcpy(
        binary->data.data(),
        &input,
        sizeof(input));

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    // item.value.type =
    //     rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

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

    rim::SnapshotBuilder builder(
        store,
        rim::kPrinterAProductDefinition);

    auto snapshot =
        builder.Build(
            {
                domainId
            });

    const rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        snapshot.TryGetBinary(
            RI_DATA_ERROR_LIST,
            found));

    ASSERT_NE(
        found,
        nullptr);

    FaultInfoList output{};

    std::memcpy(
        &output,
        found->data.data(),
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