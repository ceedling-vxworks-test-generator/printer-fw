#include <gtest/gtest.h>

#include "FixedVector.hpp"
#include "RIMConfig.hpp"
#include "RIMValueFactory.hpp"
#include "ValueStore.hpp"

//
// ValueStore は unordered_map から id 添字の固定長配列になった。
// 「全件取り出し」も返り値ではなく呼出側の入れ物へ詰める形になっている
// (返り値だと呼び出しごとに動的確保が起きるため)。
//

namespace
{

rim::RIMDataItem MakeDouble(
    rim::RIMDataId id,
    double value)
{
    rim::RIMDataItem item{};

    item.id        = id;
    item.valueType = rim::ValueType::kDouble;
    item.value     = rim::RIMValueFactory::CreateDouble(value);

    return item;
}

}

TEST(
    ValueStoreFixedTest,
    StoreAndFind)
{
    rim::ValueStore store;

    ASSERT_TRUE(
        store.Store(
            MakeDouble(
                rim::RIMDataId::kTemperatureSensorA,
                300.15)));

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kTemperatureSensorA,
            out));

    EXPECT_EQ(
        out.id,
        rim::RIMDataId::kTemperatureSensorA);
}

TEST(
    ValueStoreFixedTest,
    FindUnstoredReturnsFalse)
{
    rim::ValueStore store;

    rim::RIMDataItem out{};

    EXPECT_FALSE(
        store.Find(
            rim::RIMDataId::kTemperatureSensorA,
            out));
}

TEST(
    ValueStoreFixedTest,
    StoreOverwritesSameId)
{
    rim::ValueStore store;

    store.Store(
        MakeDouble(
            rim::RIMDataId::kTemperatureSensorA,
            1.0));

    store.Store(
        MakeDouble(
            rim::RIMDataId::kTemperatureSensorA,
            2.0));

    // 同じ id は上書きなので件数は増えない
    EXPECT_EQ(
        store.Count(),
        1U);
}

TEST(
    ValueStoreFixedTest,
    CopyAllToCollectsEveryStoredItem)
{
    rim::ValueStore store;

    store.Store(
        MakeDouble(
            rim::RIMDataId::kTemperatureSensorA,
            1.0));

    store.Store(
        MakeDouble(
            rim::RIMDataId::kHumiditySensor,
            2.0));

    rim::FixedVector<rim::RIMDataItem, rim::kMaxDataItems> items;

    store.CopyAllTo(
        items);

    EXPECT_EQ(
        items.Size(),
        2U);
}

TEST(
    ValueStoreFixedTest,
    CopyAllToClearsPreviousContent)
{
    // 呼出側の入れ物を使い回しても前回分が混ざらないこと
    rim::ValueStore store;

    store.Store(
        MakeDouble(
            rim::RIMDataId::kTemperatureSensorA,
            1.0));

    rim::FixedVector<rim::RIMDataItem, rim::kMaxDataItems> items;

    store.CopyAllTo(items);
    store.CopyAllTo(items);

    EXPECT_EQ(
        items.Size(),
        1U);
}

TEST(
    ValueStoreFixedTest,
    Clear)
{
    rim::ValueStore store;

    store.Store(
        MakeDouble(
            rim::RIMDataId::kTemperatureSensorA,
            1.0));

    store.Clear();

    EXPECT_EQ(
        store.Count(),
        0U);
}
