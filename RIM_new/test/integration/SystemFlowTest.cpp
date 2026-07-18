#include <gtest/gtest.h>

#include "core/RIMSystem.hpp"

#include "adapter/PrinterADataProfile.hpp"
#include "capability/PrinterAEvaluatorSet.hpp"

using namespace rim;

namespace
{
// 配信された Capability を記録するテスト購読者。
class RecordingSubscriber final : public ISubscriber
{
public:
    void OnPublish(const CapabilitySet& cap) override
    {
        last = cap;
        ++count;
    }
    CapabilitySet last{};
    int           count{0};
};

// PrinterA を結線した RIMSystem を組み立てるフィクスチャ。
struct SystemFixture
{
    PrinterADataProfile profile;
    PrinterAEvaluatorSet evaluators;
    RIMSystem system{profile, evaluators};
    RecordingSubscriber sub;

    SystemFixture()
    {
        system.Init();
        system.PublisherRef().Subscribe(&sub);
    }
};
} // namespace

TEST(SystemFlowTest, NormalTemperatureIsPrintableNoAlert)
{
    SystemFixture f;
    f.system.AdapterRef().PushF(RIMDataId::kTemperature, 25.5);
    f.system.Dispatch();

    ASSERT_GT(f.sub.count, 0);
    ASSERT_TRUE(f.sub.last.printable.has_value());
    EXPECT_TRUE(*f.sub.last.printable);
    ASSERT_TRUE(f.sub.last.tempAlert.has_value());
    EXPECT_FALSE(*f.sub.last.tempAlert);
}

TEST(SystemFlowTest, HighTemperatureRaisesAlert)
{
    SystemFixture f;
    f.system.AdapterRef().PushI32(RIMDataId::kTemperature, 70);  // 70℃ > 60℃
    f.system.Dispatch();

    ASSERT_TRUE(f.sub.last.tempAlert.has_value());
    EXPECT_TRUE(*f.sub.last.tempAlert);
}

TEST(SystemFlowTest, ActiveFaultMakesNotPrintable)
{
    SystemFixture f;

    DataContext fctx;
    fctx.faultState = FaultState::kRaised;
    f.system.AdapterRef().PushU32(RIMDataId::kFaultCode, 0x1001u, &fctx);
    f.system.Dispatch();

    ASSERT_TRUE(f.sub.last.printable.has_value());
    EXPECT_FALSE(*f.sub.last.printable);
}

TEST(SystemFlowTest, AccessorPullReturnsCurrentCapability)
{
    SystemFixture f;
    f.system.AdapterRef().PushI32(RIMDataId::kTemperature, 70);
    f.system.Dispatch();

    CapabilitySet cap;
    ASSERT_TRUE(f.system.AccessorRef().ReadCapability(cap));
    ASSERT_TRUE(cap.tempAlert.has_value());
    EXPECT_TRUE(*cap.tempAlert);

    // Snapshot Pull も取得できる。
    MachineSnapshot snap;
    EXPECT_EQ(f.system.AccessorRef().ReadSnapshot(kDomainCurrent, snap), RIMResult::kOk);
    ASSERT_TRUE(snap.current.has_value());
}
