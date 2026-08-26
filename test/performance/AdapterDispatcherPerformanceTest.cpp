#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <vector>

#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"
#include "RIMDataItem.hpp"
#include "RIMValue.hpp"
#include "RIMValueFactory.hpp"
#include "RouteDefinition.hpp"
#include "RouteProvider.hpp"

#include "PerformanceStatistics.hpp"

// AdapterDispatcher::Dispatch() (FindDataItem検索 -> rawValueType一致確認
// -> normalize -> RouteキューへPush) 単体の性能特性を計測する。
//
// DataStoreWorker等の下流ワーカは介在させず、Adapterレイヤの処理コストのみを
// 切り出すため、RouteProviderとAdapterDispatcherだけを直接構築する
// (test/unit/adapter/AdapterDispatcherTestと同じ構成)。
//
// 目標レイテンシ/スループットの数値目標は未策定のため、閾値によるPASS/FAIL
// 判定は行わず、計測結果を出力するのみとする。将来目標値が定まった場合は
// ASSERT/EXPECTによる閾値チェックを追加する。

namespace
{

constexpr rim::DomainDefinition kTestDomain
{
    "PerfTestDomain"
};

constexpr rim::RouteDefinition kTestRoute
{
    1,
    "PerfTestRoute",
    rim::QueuePolicy::Buffered,
    rim::QueuePolicy::Buffered,
    rim::CompressionPolicy::None,
    0
};

constexpr RIDataId kNormalizedDataId = 1;
constexpr RIDataId kUnregisteredDataId = 9999;

// item.value を 10 倍する正規化(単位変換等を模したダミー)
rim::RIMValue ScaleByTen(
    const rim::RIMValue& value,
    const void*)
{
    return rim::RIMValueFactory::CreateInt32(
        value.value.i32 * 10);
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
    }
};

const rim::ProductDefinition kTestProduct
{
    nullptr,
    0,

    &kTestRoute,
    1,

    kTestDataItems,
    1,

    nullptr,
    0
};

// RouteProvider::Initialize()済みのProviderと、それを使うDispatcherを
// まとめて用意するフィクスチャ。RouteProviderが各Routeのキューを所有する
// ため、計測ケースごとに新しいインスタンスを使う。
struct AdapterDispatcherPerfFixture
{
    rim::RouteProvider routeProvider;
    rim::ProductContext productContext{kTestProduct};

    rim::AdapterDispatcher dispatcher{
        productContext,
        routeProvider};

    AdapterDispatcherPerfFixture()
    {
        rim::ProductContext productContext(
            kTestProduct);

        routeProvider.Initialize(
            productContext);
    }
};

// Push済みのキューを空にする(計測対象外の後片付け)。
void DrainQueue(
    rim::RouteProvider& routeProvider,
    RIDataId id)
{
    auto* queues =
        routeProvider.Find(
            id);

    if (queues == nullptr)
    {
        return;
    }

    rim::RIMDataItem item{};

    while (queues->storeQueue->TryPop(
        item))
    {
    }
}

} // namespace

TEST(
    AdapterDispatcherPerformanceTest,
    DispatchThroughputScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        50000
    };

    for (const auto count : counts)
    {
        AdapterDispatcherPerfFixture fixture;

        rim::RIMDataItem item{};

        item.id = kNormalizedDataId;

        const auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            item.value =
                rim::RIMValueFactory::CreateInt32(
                    static_cast<std::int32_t>(
                        i));

            ASSERT_TRUE(
                fixture.dispatcher.Dispatch(
                    item));
        }

        const auto end =
            std::chrono::steady_clock::now();

        const auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                .count();

        const auto throughput =
            elapsedUs > 0
            ? static_cast<double>(count)
              * 1000000.0
              / static_cast<double>(elapsedUs)
            : 0.0;

        std::cout
            << "[AdapterDispatcher Throughput]"
            << " count=" << count
            << " elapsed=" << elapsedUs << " us"
            << " dispatch/sec="
            << static_cast<std::uint64_t>(
                   throughput)
            << std::endl;

        DrainQueue(
            fixture.routeProvider,
            kNormalizedDataId);
    }
}

TEST(
    AdapterDispatcherPerformanceTest,
    DispatchLatencyDistribution)
{
    constexpr std::size_t kIterations =
        10000;

    AdapterDispatcherPerfFixture fixture;

    std::vector<std::int64_t>
        latenciesUs;

    latenciesUs.reserve(
        kIterations);

    rim::RIMDataItem item{};

    item.id = kNormalizedDataId;

    for (std::size_t i = 0;
         i < kIterations;
         ++i)
    {
        item.value =
            rim::RIMValueFactory::CreateInt32(
                static_cast<std::int32_t>(
                    i));

        const auto start =
            std::chrono::steady_clock::now();

        ASSERT_TRUE(
            fixture.dispatcher.Dispatch(
                item));

        const auto end =
            std::chrono::steady_clock::now();

        latenciesUs.push_back(
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                .count());

        // キューを溜めすぎないよう都度払い出す
        // (下流ワーカが定常的に消費する状況を模す)
        rim::RIMDataItem popped{};

        fixture.routeProvider
            .Find(kNormalizedDataId)
            ->storeQueue->TryPop(
                popped);
    }

    const auto stats =
        perf::CalculateStatistics(
            latenciesUs);

    std::cout
        << "\n[AdapterDispatcher Latency]\n"
        << "  Samples : " << latenciesUs.size() << '\n'
        << "  Min     : " << stats.min << " us\n"
        << "  Avg     : " << stats.avg << " us\n"
        << "  P95     : " << stats.p95 << " us\n"
        << "  P99     : " << stats.p99 << " us\n"
        << "  Max     : " << stats.max << " us\n";
}

TEST(
    AdapterDispatcherPerformanceTest,
    RejectionPathOverhead)
{
    constexpr std::size_t kIterations =
        10000;

    // 成功経路: FindDataItem成功 -> 型一致 -> normalize -> Push
    {
        AdapterDispatcherPerfFixture fixture;

        rim::RIMDataItem item{};

        item.id = kNormalizedDataId;

        item.value =
            rim::RIMValueFactory::CreateInt32(
                1);

        const auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < kIterations;
             ++i)
        {
            ASSERT_TRUE(
                fixture.dispatcher.Dispatch(
                    item));
        }

        const auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    std::chrono::steady_clock::now()
                    - start)
                .count();

        std::cout
            << "[RejectionPathOverhead] Success       : "
            << elapsedUs << " us / " << kIterations
            << " calls ("
            << (static_cast<double>(elapsedUs)
                / static_cast<double>(kIterations))
            << " us/call)"
            << std::endl;

        DrainQueue(
            fixture.routeProvider,
            kNormalizedDataId);
    }

    // 異常系1: 未登録DataId(FindDataItemがnullptrを返し即return)
    {
        AdapterDispatcherPerfFixture fixture;

        rim::RIMDataItem item{};

        item.id = kUnregisteredDataId;

        item.value =
            rim::RIMValueFactory::CreateInt32(
                1);

        const auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < kIterations;
             ++i)
        {
            ASSERT_FALSE(
                fixture.dispatcher.Dispatch(
                    item));
        }

        const auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    std::chrono::steady_clock::now()
                    - start)
                .count();

        std::cout
            << "[RejectionPathOverhead] UnregisteredId : "
            << elapsedUs << " us / " << kIterations
            << " calls ("
            << (static_cast<double>(elapsedUs)
                / static_cast<double>(kIterations))
            << " us/call)"
            << std::endl;
    }

    // 異常系2: rawValueType不一致(FindDataItemは成功するが型チェックでreturn)
    {
        AdapterDispatcherPerfFixture fixture;

        rim::RIMDataItem item{};

        item.id = kNormalizedDataId;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                1.5);

        const auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < kIterations;
             ++i)
        {
            ASSERT_FALSE(
                fixture.dispatcher.Dispatch(
                    item));
        }

        const auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    std::chrono::steady_clock::now()
                    - start)
                .count();

        std::cout
            << "[RejectionPathOverhead] TypeMismatch   : "
            << elapsedUs << " us / " << kIterations
            << " calls ("
            << (static_cast<double>(elapsedUs)
                / static_cast<double>(kIterations))
            << " us/call)"
            << std::endl;
    }
}
