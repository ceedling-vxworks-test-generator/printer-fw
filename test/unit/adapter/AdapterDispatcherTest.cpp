#include <gtest/gtest.h>

#include <cstdint>

#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"
#include "RouteDefinition.hpp"
#include "RouteProvider.hpp"
#include "RIMDataItem.hpp"
#include "RIMValue.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "ProductContext.hpp"


namespace
{

constexpr rim::DomainDefinition kTestDomain
{
    "TestDomain"
};

constexpr rim::RouteDefinition kTestRoute
{
    1,
    "TestRoute",
    rim::QueuePolicy::Buffered,
    rim::QueuePolicy::Buffered,
    rim::CompressionPolicy::None,
    0
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
        kTestRoute.id,

        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,

        {
            rim::ValueType::kInt32,
            { .i32 = 0 },
            0
        },

        ScaleByTen,
        nullptr,
        nullptr
    },
    {
        kTypeChangeDataId,
        "TypeChangeItem",
        &kTestDomain,
        kTestRoute.id,

        rim::ValueType::kInt32,
        rim::ValueType::kUInt32,
        rim::ValueType::kUInt32,
        rim::ValueType::kUInt32,

        {
            rim::ValueType::kInt32,
            { .i32 = 0 },
            0
        },

        NarrowToUInt32,
        nullptr,
        nullptr
    }
};

const rim::ProductDefinition kTestProduct
{
    nullptr,
    0,

    &kTestRoute,
    1,

    kTestDataItems,
    2,

    nullptr,
    0
};

// RouteProvider::Initialize()済みのProviderと、それを使うDispatcherを
// まとめて用意するテストフィクスチャ。RouteProviderが各Routeのキューを
// 所有するため、テストごとに新しいインスタンスを使う。
struct AdapterDispatcherFixture
{
    rim::RouteProvider routeProvider;
    rim::ProductContext productContext{kTestProduct};

    rim::AdapterDispatcher dispatcher{
        productContext,
        routeProvider};

    AdapterDispatcherFixture()
    {
        routeProvider.Initialize(
            productContext);
    }
};

} // namespace

TEST(
    AdapterDispatcherTest,
    DispatchNormalizesAndPushesToRouteQueue)
{
    AdapterDispatcherFixture fixture;

    rim::RIMDataItem item{};

    item.id = kNormalizedDataId;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            5);

    EXPECT_TRUE(
        fixture.dispatcher.Dispatch(
            item));

    auto* queues =
        fixture.routeProvider.Find(
            kNormalizedDataId);

    ASSERT_NE(
        queues,
        nullptr);

    rim::RIMDataItem pushed{};

    ASSERT_TRUE(
        queues->storeQueue->TryPop(
            pushed));

    EXPECT_EQ(
        pushed.id,
        kNormalizedDataId);

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
        queues->storeQueue->TryPop(
            extra));
}

TEST(
    AdapterDispatcherTest,
    DispatchStoresNormalizedValueWithDefinitionSetValueType)
{
    AdapterDispatcherFixture fixture;

    rim::RIMDataItem item{};

    // kTypeChangeDataId の rawValueType は kInt32
    item.id = kTypeChangeDataId;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            7);

    EXPECT_TRUE(
        fixture.dispatcher.Dispatch(
            item));

    auto* queues =
        fixture.routeProvider.Find(
            kTypeChangeDataId);

    ASSERT_NE(
        queues,
        nullptr);

    rim::RIMDataItem pushed{};

    ASSERT_TRUE(
        queues->storeQueue->TryPop(
            pushed));

    EXPECT_EQ(
        pushed.id,
        kTypeChangeDataId);

    // normalize(NarrowToUInt32)がsetValueType(kUInt32)の値を返すため、
    // pushed.value.type もkUInt32になる。
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
    AdapterDispatcherFixture fixture;

    rim::RIMDataItem item{};

    item.id = kUnregisteredDataId;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            1);

    EXPECT_FALSE(
        fixture.dispatcher.Dispatch(
            item));

    EXPECT_EQ(
        fixture.routeProvider.Find(
            kUnregisteredDataId),
        nullptr);
}

TEST(
    AdapterDispatcherTest,
    DispatchRejectsRawValueTypeMismatch)
{
    AdapterDispatcherFixture fixture;

    rim::RIMDataItem item{};

    // kNormalizedDataId の rawValueType は kInt32
    item.id = kNormalizedDataId;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            1.5);

    EXPECT_FALSE(
        fixture.dispatcher.Dispatch(
            item));

    auto* queues =
        fixture.routeProvider.Find(
            kNormalizedDataId);

    ASSERT_NE(
        queues,
        nullptr);

    rim::RIMDataItem pushed{};

    EXPECT_FALSE(
        queues->storeQueue->TryPop(
            pushed));
}
