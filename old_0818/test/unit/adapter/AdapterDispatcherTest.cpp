#include <gtest/gtest.h>

#include <cstdint>

#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"
#include "ProductDefinition.hpp"
#include "RIMDataItem.hpp"
#include "RIMValue.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "StoreInputQueue.hpp"

namespace
{

constexpr rim::DomainDefinition kTestDomain
{
    "TestDomain"
};

constexpr RIDataId kNormalizedDataId = 1001;
constexpr RIDataId kTypeChangeDataId = 1002;
constexpr RIDataId kUnregisteredDataId = 9999;

// item.value を 10 倍する正規化(単位変換等を模したダミー)
rim::RIMValue ScaleByTen(
    const rim::RIMValue& value,
    const void*)
{
    std::int32_t raw{};

    if (!rim::RIMValueAccessor::GetInt32(
            value,
            raw))
    {
        return {};
    }

    return rim::RIMValueFactory::CreateInt32(
        raw * 10);
}

// rawValueType(kInt32) から setValueType(kUInt32) への変換を模したダミー
rim::RIMValue NarrowToUInt32(
    const rim::RIMValue& value,
    const void*)
{
    std::int32_t raw{};

    if (!rim::RIMValueAccessor::GetInt32(
            value,
            raw))
    {
        return {};
    }

    return rim::RIMValueFactory::CreateUInt32(
        static_cast<std::uint32_t>(raw));
}

const rim::DataItemDefinition kTestDataItems[]
{
    {
        kNormalizedDataId,
        "NormalizedItem",
        &kTestDomain,
        "TestRoute",

        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,

        ScaleByTen,
        nullptr,
        nullptr
    },
    {
        kTypeChangeDataId,
        "TypeChangeItem",
        &kTestDomain,
        "TestRoute",

        rim::ValueType::kInt32,
        rim::ValueType::kUInt32,
        rim::ValueType::kUInt32,
        rim::ValueType::kUInt32,

        NarrowToUInt32,
        nullptr,
        nullptr
    }
};

const rim::ProductDefinition kTestProduct
{
    nullptr,
    0,

    nullptr,
    0,

    kTestDataItems,
    2,

    nullptr,
    0,

    nullptr,
    0
};

} // namespace

TEST(
    AdapterDispatcherTest,
    DispatchNormalizesAndPushesToQueue)
{
    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        kTestProduct,
        queue);

    rim::RIMDataItem item{};

    item.id = kNormalizedDataId;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            5);

    EXPECT_TRUE(
        dispatcher.Dispatch(
            item));

    rim::RIMDataItem pushed{};

    ASSERT_TRUE(
        queue.TryPop(
            pushed));

    EXPECT_EQ(
        pushed.id,
        kNormalizedDataId);

    EXPECT_EQ(
        pushed.valueType,
        rim::ValueType::kInt32);

    std::int32_t value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            pushed.value,
            value));

    EXPECT_EQ(
        value,
        50);

    rim::RIMDataItem extra{};

    EXPECT_FALSE(
        queue.TryPop(
            extra));
}

TEST(
    AdapterDispatcherTest,
    DispatchUsesDefinitionSetValueTypeRegardlessOfCallerValueType)
{
    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        kTestProduct,
        queue);

    rim::RIMDataItem item{};

    item.id = kTypeChangeDataId;

    // Dispatch は item.valueType を見ないため、わざと不一致な値を入れておく
    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            7);

    EXPECT_TRUE(
        dispatcher.Dispatch(
            item));

    rim::RIMDataItem pushed{};

    ASSERT_TRUE(
        queue.TryPop(
            pushed));

    EXPECT_EQ(
        pushed.id,
        kTypeChangeDataId);

    EXPECT_EQ(
        pushed.valueType,
        rim::ValueType::kUInt32);

    std::uint32_t value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetUInt32(
            pushed.value,
            value));

    EXPECT_EQ(
        value,
        7U);
}

TEST(
    AdapterDispatcherTest,
    DispatchRejectsUnregisteredDataId)
{
    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        kTestProduct,
        queue);

    rim::RIMDataItem item{};

    item.id = kUnregisteredDataId;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            1);

    EXPECT_FALSE(
        dispatcher.Dispatch(
            item));

    rim::RIMDataItem pushed{};

    EXPECT_FALSE(
        queue.TryPop(
            pushed));
}

TEST(
    AdapterDispatcherTest,
    DispatchRejectsRawValueTypeMismatch)
{
    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        kTestProduct,
        queue);

    rim::RIMDataItem item{};

    // kNormalizedDataId の rawValueType は kInt32
    item.id = kNormalizedDataId;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            1.5);

    EXPECT_FALSE(
        dispatcher.Dispatch(
            item));

    rim::RIMDataItem pushed{};

    EXPECT_FALSE(
        queue.TryPop(
            pushed));
}
