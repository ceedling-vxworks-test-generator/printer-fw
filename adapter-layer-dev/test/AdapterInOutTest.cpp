//
// AdapterInOutTest.cpp - Adapterレイヤ単体のin/outテスト。
//
// 対象は 構成変更/0808/Rim_new の実装(AdapterDispatcher::Dispatch /
// PrinterAdapter::Poll)を無改変で持ち込んだもの。
// gtestはネットワーク制限下で取得できないため、標準ライブラリのみに
// 依存する単独の実行ファイルとして実装し、実際に実行して確認する。
//
// IN  : DeviceEvent{ RIDataId, int }
// OUT : StoreInputQueue に積まれる RIMDataItem{ id, valueType, value }
//

#include <cstdio>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"
#include "MockAdapter.hpp"
#include "StoreInputQueue.hpp"

#include "TestProductCatalog.hpp"

namespace
{

int g_failures = 0;

} // namespace

#define CHECK(cond)                                             \
    do                                                           \
    {                                                             \
        if (!(cond))                                               \
        {                                                           \
            std::fprintf(                                           \
                stderr,                                               \
                "NG %s:%d: %s\n",                                      \
                __FILE__,                                               \
                __LINE__,                                                \
                #cond);                                                   \
            ++g_failures;                                                  \
        }                                                                   \
    } while (0)

namespace
{

//
// IN: 既知idの温度イベント -> OUT: 正規化なしでdoubleのまま積まれる。
//
void TestKnownTemperatureIsForwarded()
{
    std::printf("[TEST] KnownTemperatureIsForwarded\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        test::kTestProduct,
        queue);

    const rim::DeviceEvent in{
        RI_DATA_TEMPERATURE_SENSOR_A,
        25};

    CHECK(dispatcher.Dispatch(in) == true);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);

    CHECK(out.id == RI_DATA_TEMPERATURE_SENSOR_A);
    CHECK(out.valueType == rim::ValueType::kDouble);
    CHECK(out.value.value.d == 25.0);

    // 1件しか積んでいない。
    CHECK(queue.TryPop(out) == false);
}

//
// IN: 既知idのステープルレベル(範囲外) -> OUT: 0〜100へクランプ・int32化。
//
void TestStapleLevelIsClampedAndConverted()
{
    std::printf("[TEST] StapleLevelIsClampedAndConverted\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        test::kTestProduct,
        queue);

    CHECK(dispatcher.Dispatch({RI_DATA_STAPLE_LEVEL, 150}) == true);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);
    CHECK(out.valueType == rim::ValueType::kInt32);
    CHECK(out.value.value.i32 == 100); // 上限クランプ

    CHECK(dispatcher.Dispatch({RI_DATA_STAPLE_LEVEL, -10}) == true);

    CHECK(queue.TryPop(out) == true);
    CHECK(out.value.value.i32 == 0);   // 下限クランプ
}

//
// IN: カタログに無いid -> OUT: 何も積まれず、Dispatchはfalseを返す。
//
void TestUnknownIdIsRejectedWithoutForwarding()
{
    std::printf("[TEST] UnknownIdIsRejectedWithoutForwarding\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        test::kTestProduct,
        queue);

    // カタログに存在しないid(JOB_ID)を投入する。
    CHECK(dispatcher.Dispatch({RI_DATA_JOB_ID, 1}) == false);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == false);
}

//
// PrinterAdapter::Poll() 経由でも同じ経路を通ること
// (IHardwareAdapterインターフェース経由の呼び出し)。
//
void TestPrinterAdapterPollDispatchesThroughSameQueue()
{
    std::printf("[TEST] PrinterAdapterPollDispatchesThroughSameQueue\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        test::kTestProduct,
        queue);

    rim::IHardwareAdapter& adapter =
        *(new rim::PrinterAdapter(dispatcher));

    CHECK(adapter.Initialize() == true);
    CHECK(adapter.Poll() == true); // 内部でRI_DATA_TEMPERATURE_SENSOR_Aを固定値30で投入

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);
    CHECK(out.id == RI_DATA_TEMPERATURE_SENSOR_A);
    CHECK(out.value.value.d == 30.0);

    adapter.Shutdown();

    delete &adapter;
}

//
// MockAdapterがIHardwareAdapterを満たし、常に成功を返すだけであること。
//
void TestMockAdapterSatisfiesInterface()
{
    std::printf("[TEST] MockAdapterSatisfiesInterface\n");

    rim::MockAdapter mock;

    CHECK(mock.Initialize() == true);
    CHECK(mock.Poll() == true);

    mock.Shutdown(); // 例外なく完了すること
}

} // namespace

int main()
{
    TestKnownTemperatureIsForwarded();
    TestStapleLevelIsClampedAndConverted();
    TestUnknownIdIsRejectedWithoutForwarding();
    TestPrinterAdapterPollDispatchesThroughSameQueue();
    TestMockAdapterSatisfiesInterface();

    if (g_failures == 0)
    {
        std::printf("[RESULT] 全項目 OK\n");
        return 0;
    }

    std::printf("[RESULT] %d 件 NG\n", g_failures);
    return 1;
}
