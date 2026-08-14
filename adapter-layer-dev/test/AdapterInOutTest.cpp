//
// AdapterInOutTest.cpp - Adapterレイヤ単体のin/outテスト。
//
// 対象は 構成変更/0808/Rim_new の実装(AdapterDispatcher::Dispatch /
// PrinterAdapter::Poll)をベースに、IN引数へcontext(第3引数)を追加した版。
// gtestはネットワーク制限下で取得できないため、標準ライブラリのみに
// 依存する単独の実行ファイルとして実装し、実際に実行して確認する。
//
// IN  : id(RIDataId) / value(int) / context(RIMContext*、省略可・nullptr可)
// OUT : StoreInputQueue に積まれる RIMDataItem{ id, valueType, value }
//

#include <cstdio>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"
#include "MockAdapter.hpp"
#include "StoreInputQueue.hpp"

#include "PrinterAContextKeys.hpp"
#include "PrinterADataItems.hpp"

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
// IN: 既知idの温度、context無し -> OUT: 既に正規化済み扱いでdoubleのまま積まれる。
//
void TestKnownTemperatureWithoutContextIsTreatedAsNormalized()
{
    std::printf("[TEST] KnownTemperatureWithoutContextIsTreatedAsNormalized\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProduct,
        queue);

    CHECK(dispatcher.Dispatch(RI_DATA_TEMPERATURE_SENSOR_A, 25) == true);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);

    CHECK(out.id == RI_DATA_TEMPERATURE_SENSOR_A);
    CHECK(out.valueType == rim::ValueType::kDouble);
    CHECK(out.value.value.d == 25.0); // context無し = 換算しない

    // 1件しか積んでいない。
    CHECK(queue.TryPop(out) == false);
}

//
// IN: context で単位(摂氏/華氏)を指定 -> OUT: どちらもケルビンへ正規化される。
// (今回追加した「id+contextから変換ルールへ辿り着く」振る舞いの確認)
//
void TestTemperatureContextSelectsUnitConversion()
{
    std::printf("[TEST] TemperatureContextSelectsUnitConversion\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProduct,
        queue);

    // 摂氏0度 -> 273.15K
    rim::RIMContext celsiusCtx{};
    celsiusCtx[0] = {rim::kContextKeyUnit, rim::kUnitCelsius};

    CHECK(dispatcher.Dispatch(
              RI_DATA_TEMPERATURE_SENSOR_A,
              0,
              &celsiusCtx) == true);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);
    CHECK(out.value.value.d == 273.15);

    // 華氏32度 -> 273.15K(摂氏0度と同じ)
    rim::RIMContext fahrenheitCtx{};
    fahrenheitCtx[0] = {rim::kContextKeyUnit, rim::kUnitFahrenheit};

    CHECK(dispatcher.Dispatch(
              RI_DATA_TEMPERATURE_SENSOR_A,
              32,
              &fahrenheitCtx) == true);

    CHECK(queue.TryPop(out) == true);
    CHECK(out.value.value.d == 273.15);
}

//
// IN: 既知idのステープルレベル(範囲外) -> OUT: 0〜100へクランプ・int32化。
// (単位を持たないデータ種別。contextは省略できることの確認も兼ねる)
//
void TestStapleLevelIsClampedAndConverted()
{
    std::printf("[TEST] StapleLevelIsClampedAndConverted\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProduct,
        queue);

    CHECK(dispatcher.Dispatch(RI_DATA_STAPLE_LEVEL, 150) == true);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == true);
    CHECK(out.valueType == rim::ValueType::kInt32);
    CHECK(out.value.value.i32 == 100); // 上限クランプ

    CHECK(dispatcher.Dispatch(RI_DATA_STAPLE_LEVEL, -10) == true);

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
        rim::kPrinterAProduct,
        queue);

    // カタログに存在しないid(JOB_ID)を投入する。
    CHECK(dispatcher.Dispatch(RI_DATA_JOB_ID, 1) == false);

    rim::RIMDataItem out{};

    CHECK(queue.TryPop(out) == false);
}

//
// PrinterAdapter::Poll() 経由でも同じ経路を通ること
// (IHardwareAdapterインターフェース経由の呼び出し。contextは持たない)。
//
void TestPrinterAdapterPollDispatchesThroughSameQueue()
{
    std::printf("[TEST] PrinterAdapterPollDispatchesThroughSameQueue\n");

    rim::StoreInputQueue queue;

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProduct,
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
    TestKnownTemperatureWithoutContextIsTreatedAsNormalized();
    TestTemperatureContextSelectsUnitConversion();
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
