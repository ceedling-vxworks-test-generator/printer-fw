#include <gtest/gtest.h>

#include "adapter/Adapter.hpp"
#include "adapter/PrinterADataProfile.hpp"
#include "datastore/DataStore.hpp"

using namespace rim;

namespace
{
// profile + store + adapter を束ねたテスト用ハーネス。
struct Harness
{
    PrinterADataProfile profile;
    DataStore           store{profile};
    Adapter             adapter{store, profile};

    Harness() { store.Init(nullptr); }

    std::uint32_t ActiveFaultCount()
    {
        SnapshotRequest req; req.domains = kDomainFault;
        MachineSnapshot snap;
        store.Capture(req, snap);
        return (snap.fault) ? snap.fault->activeCount : 0u;
    }
};
} // namespace

// 溢れ: dispatch前に DEPTH+1 件ADD → 超過分は kErrPost、反映後 active_count==DEPTH。
TEST(DataStoreStagingTest, FaultQueueOverflowDropsExcess)
{
    Harness h;
    int ok = 0, full = 0;
    for (std::size_t i = 0; i < kFaultQueueDepth + 1; ++i) {
        RIMResult r = h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kAdd,
                                       0x2000u + static_cast<std::uint32_t>(i));
        if (r == RIMResult::kOk) ++ok;
        else if (r == RIMResult::kErrPost) ++full;
    }
    EXPECT_EQ(ok, static_cast<int>(kFaultQueueDepth));
    EXPECT_EQ(full, 1);

    h.store.Dispatch();
    EXPECT_EQ(h.ActiveFaultCount(), static_cast<std::uint32_t>(kFaultQueueDepth));
}

// 順序: ADD A, ADD B, REMOVE A → 反映後は B のみ残る(FIFO・喪失なし)。
TEST(DataStoreStagingTest, FaultOrderAddAddRemove)
{
    Harness h;
    h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kAdd,    0xAAAAu);
    h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kAdd,    0xBBBBu);
    h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kRemove, 0xAAAAu);
    h.store.Dispatch();

    SnapshotRequest req; req.domains = kDomainFault;
    MachineSnapshot snap;
    h.store.Capture(req, snap);
    ASSERT_TRUE(snap.fault.has_value());
    EXPECT_EQ(snap.fault->activeCount, 1u);
    EXPECT_EQ(snap.fault->activeCodes[0], 0xBBBBu);
}

// 更新/不在: 既存キーの UPDATE と 不在キーの REMOVE は共に投入成功(kOk)、件数不変。
TEST(DataStoreStagingTest, FaultUpdateExistingAndRemoveAbsent)
{
    Harness h;
    h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kAdd, 0xBBBBu);
    h.store.Dispatch();

    EXPECT_EQ(h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kUpdate, 0xBBBBu), RIMResult::kOk);
    EXPECT_EQ(h.adapter.Submit(RIMDataId::kFaultCode, CollectionOp::kRemove, 0x9999u), RIMResult::kOk);
    h.store.Dispatch();

    EXPECT_EQ(h.ActiveFaultCount(), 1u);
}

// CURRENT: 同一IDの連続postは latest-wins。
TEST(DataStoreStagingTest, CurrentValueLatestWins)
{
    Harness h;
    h.adapter.PushF(RIMDataId::kTemperature, 25.5);
    h.adapter.PushF(RIMDataId::kTemperature, 30.0);
    h.store.Dispatch();

    SnapshotRequest req; req.domains = kDomainCurrent;
    MachineSnapshot snap;
    h.store.Capture(req, snap);
    ASSERT_TRUE(snap.current.has_value());
    const int idx = ToIndex(RIMDataId::kTemperature);
    EXPECT_TRUE(snap.current->present[idx]);
    EXPECT_EQ(snap.current->values[idx].u.celsiusX100, 3000);
}

// CURRENT レーンへは Submit(コレクション操作)不可 → kErrKindMismatch。
TEST(DataStoreStagingTest, SubmitRejectsCurrentKind)
{
    Harness h;
    EXPECT_EQ(h.adapter.Submit(RIMDataId::kTemperature, CollectionOp::kAdd, 1u),
              RIMResult::kErrKindMismatch);
}
