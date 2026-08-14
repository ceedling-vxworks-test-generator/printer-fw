#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <cstring>

#include "rim_api.h"
#include "rim_types.h"

#include "ValueStore.hpp"
#include "BinaryStoreValue.hpp"

#include "RIMSnapshotManager.hpp"
#include "RIMValueFactory.hpp"

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

    rim::ValueStore store;

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

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    store.Store(
        item);

    rim::RIMSnapshotManager reader(
        store);

    auto snapshot =
        reader.Read();

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
