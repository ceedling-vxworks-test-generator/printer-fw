#include <gtest/gtest.h>

#include <cstdint>

#include "ErrorRepository.hpp"
#include "FaultApplier.hpp"
#include "RIMValueFactory.hpp"

//
// 異常報告の反映。
//
// Adapter の受理点は Push 1本なので、「現在値」も「異常報告」も同じ経路で入る。
// どちらなのかを判別して振り分けるのは L2 の責務であり、その判別材料が
// DataContext::faultState である。
//

namespace
{

rim::RIMDataItem MakeFault(
    rim::FaultState state,
    std::uint32_t code)
{
    rim::RIMDataItem item{};

    item.id                = rim::RIMDataId::kTemperatureSensorA;
    item.context.faultState = state;
    item.context.key        = code;

    return item;
}

rim::RIMDataItem MakeValue(
    double value)
{
    rim::RIMDataItem item{};

    item.id        = rim::RIMDataId::kTemperatureSensorA;
    item.valueType = rim::ValueType::kDouble;
    item.value     = rim::RIMValueFactory::CreateDouble(value);

    return item;
}

}

TEST(
    FaultApplierTest,
    ValueItemIsNotAFault)
{
    // faultState が無いものは現在値。異常一覧には触れない。
    EXPECT_FALSE(
        rim::FaultApplier::IsFault(
            MakeValue(300.0)));
}

TEST(
    FaultApplierTest,
    FaultItemIsDetected)
{
    EXPECT_TRUE(
        rim::FaultApplier::IsFault(
            MakeFault(
                rim::FaultState::kRaised,
                1001)));
}

TEST(
    FaultApplierTest,
    RaisedAddsToRepository)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    EXPECT_TRUE(
        applier.Apply(
            MakeFault(
                rim::FaultState::kRaised,
                1001)));

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);

    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        1001U);

    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kActive);
}

TEST(
    FaultApplierTest,
    RaisingTheSameCodeTwiceChangesNothing)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(
        MakeFault(rim::FaultState::kRaised, 1001));

    // 2回目は変化なし(重複を持たない)
    EXPECT_FALSE(
        applier.Apply(
            MakeFault(rim::FaultState::kRaised, 1001)));

    EXPECT_EQ(
        repository.GetAll().Size(),
        1U);
}

TEST(
    FaultApplierTest,
    ClearedRemovesFromRepository)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(
        MakeFault(rim::FaultState::kRaised, 1001));

    EXPECT_TRUE(
        applier.Apply(
            MakeFault(rim::FaultState::kCleared, 1001)));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

TEST(
    FaultApplierTest,
    ClearingAnUnknownCodeChangesNothing)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    EXPECT_FALSE(
        applier.Apply(
            MakeFault(rim::FaultState::kCleared, 9999)));
}

TEST(
    FaultApplierTest,
    AllClearedEmptiesRepositoryWithoutCode)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(MakeFault(rim::FaultState::kRaised, 1001));
    applier.Apply(MakeFault(rim::FaultState::kRaised, 1002));

    // 全解除は異常コードを必要としない
    rim::RIMDataItem item{};
    item.context.faultState = rim::FaultState::kAllCleared;

    EXPECT_TRUE(
        applier.Apply(item));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

TEST(
    FaultApplierTest,
    UpdatedHealChangesStateToRecovered)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(
        MakeFault(rim::FaultState::kRaised, 1001));

    EXPECT_TRUE(
        applier.Apply(
            MakeFault(rim::FaultState::kUpdatedHeal, 1001)));

    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kRecovered);

    // 一覧からは消えない(回復したことを伝えるため)
    EXPECT_EQ(
        repository.GetAll().Size(),
        1U);
}

TEST(
    FaultApplierTest,
    UpdatedActiveChangesStateBack)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(MakeFault(rim::FaultState::kRaised, 1001));
    applier.Apply(MakeFault(rim::FaultState::kUpdatedHeal, 1001));

    EXPECT_TRUE(
        applier.Apply(
            MakeFault(rim::FaultState::kUpdatedActive, 1001)));

    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kActive);
}

TEST(
    FaultApplierTest,
    CodeCanComeFromTheValueWhenKeyIsAbsent)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    rim::RIMDataItem item{};

    item.context.faultState = rim::FaultState::kRaised;
    item.valueType          = rim::ValueType::kUInt32;
    item.value              = rim::RIMValueFactory::CreateUInt32(2002);

    EXPECT_TRUE(
        applier.Apply(item));

    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        2002U);
}

TEST(
    FaultApplierTest,
    MissingCodeIsIgnored)
{
    // コードが取れないものは触らない(0 番の異常を勝手に作らない)
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    rim::RIMDataItem item{};

    item.context.faultState = rim::FaultState::kRaised;
    item.valueType          = rim::ValueType::kDouble;
    item.value              = rim::RIMValueFactory::CreateDouble(1.0);

    EXPECT_FALSE(
        applier.Apply(item));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

TEST(
    FaultApplierTest,
    NoneStateDoesNothing)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    EXPECT_FALSE(
        applier.Apply(
            MakeFault(rim::FaultState::kNone, 1001)));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

//
// ApplySnapshot - FaultInfoList 相当(「今存在する異常の全件」)の反映。
//
// 単発の Apply とは別経路(併存)。渡した一覧を全件スナップショットとして扱い、
// 一覧に無いコードは(回復ではなく)削除する。
//

TEST(
    FaultApplierTest,
    ApplySnapshotAddsUnknownCodes)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    const rim::FaultSnapshotEntry entries[] = {
        {1001, rim::ErrorState::kActive},
        {1002, rim::ErrorState::kRecovered},
    };

    EXPECT_TRUE(
        applier.ApplySnapshot(
            entries,
            2));

    ASSERT_EQ(
        repository.GetAll().Size(),
        2U);

    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        1001U);
    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kActive);

    EXPECT_EQ(
        repository.GetAll()[1].errorCode,
        1002U);
    EXPECT_EQ(
        repository.GetAll()[1].state,
        rim::ErrorState::kRecovered);
}

TEST(
    FaultApplierTest,
    ApplySnapshotRegistersHealedCodeThatWasNeverRaised)
{
    // 「HEALED だけが送られてきて未登録」の場合は、回復済みとして新規登録する
    // (方針: 回復済みとして新規登録する)。
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    const rim::FaultSnapshotEntry entries[] = {
        {2001, rim::ErrorState::kRecovered},
    };

    EXPECT_TRUE(
        applier.ApplySnapshot(
            entries,
            1));

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);
    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        2001U);
    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kRecovered);
}

TEST(
    FaultApplierTest,
    ApplySnapshotUpdatesStateOfExistingCode)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(
        MakeFault(rim::FaultState::kRaised, 1001));

    const rim::FaultSnapshotEntry entries[] = {
        {1001, rim::ErrorState::kRecovered},
    };

    EXPECT_TRUE(
        applier.ApplySnapshot(
            entries,
            1));

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);
    EXPECT_EQ(
        repository.GetAll()[0].state,
        rim::ErrorState::kRecovered);
}

TEST(
    FaultApplierTest,
    ApplySnapshotRemovesCodesMissingFromTheList)
{
    // 全件スナップショットに無いコードは、回復扱いではなく削除される。
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(MakeFault(rim::FaultState::kRaised, 1001));
    applier.Apply(MakeFault(rim::FaultState::kRaised, 1002));

    const rim::FaultSnapshotEntry entries[] = {
        {1001, rim::ErrorState::kActive},
    };

    EXPECT_TRUE(
        applier.ApplySnapshot(
            entries,
            1));

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);
    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        1001U);
}

TEST(
    FaultApplierTest,
    ApplySnapshotEmptyListClearsRepository)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    applier.Apply(MakeFault(rim::FaultState::kRaised, 1001));

    EXPECT_TRUE(
        applier.ApplySnapshot(
            nullptr,
            0));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

TEST(
    FaultApplierTest,
    ApplySnapshotOfEmptyRepositoryWithEmptyListChangesNothing)
{
    rim::ErrorRepository repository;
    rim::FaultApplier    applier(repository);

    EXPECT_FALSE(
        applier.ApplySnapshot(
            nullptr,
            0));
}
