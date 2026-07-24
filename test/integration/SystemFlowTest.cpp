#include <gtest/gtest.h>

#include "core/RIMSystem.hpp"
#include "adapter/PrinterADataProfile.hpp"
#include "adapter/RawValue.hpp"
#include "capability/PrinterACapabilityBuilder.hpp"

using namespace rim;

namespace
{
class RecordingSubscriber final : public ISubscriber
{
public:
    void OnPublish(const MachineCapability& mc) override { last = mc; ++count; }
    MachineCapability last{};
    int               count{0};
};

struct Fixture
{
    PrinterADataProfile       profile;   // classifier 兼 resolver
    PrinterACapabilityBuilder builder;
    RIMSystem                 system{profile, profile, builder};
    RecordingSubscriber       sub;

    Fixture()
    {
        system.Init();
        system.Publisher().Subscribe(&sub);
    }
};
} // namespace

TEST(SystemFlowTest, TemperatureFlowsToEnvCap)
{
    Fixture f;
    f.system.Adapter().Push(RIMDataId::kTemperature, RawValue::Scalar(25.0));
    f.system.Dispatch();

    ASSERT_GT(f.sub.count, 0);
    ASSERT_TRUE(f.sub.last.capabilities.env.has_value());
    EXPECT_TRUE(f.sub.last.capabilities.env->inRange);
}

TEST(SystemFlowTest, FaultMakesNotPrintableCriticalEvent)
{
    Fixture f;
    DataContext ctx; ctx.faultState = FaultState::kRaised;
    f.system.Adapter().Push(RIMDataId::kFaultCode, RawValue::Scalar(0x10u), &ctx);
    f.system.Dispatch();

    ASSERT_TRUE(f.sub.last.capabilities.error.has_value());
    EXPECT_TRUE(f.sub.last.capabilities.error->hasError);
    ASSERT_TRUE(f.sub.last.capabilities.print.has_value());
    EXPECT_FALSE(f.sub.last.capabilities.print->printable);
    EXPECT_EQ(f.sub.last.priority, CapabilityPriority::kCritical);
    EXPECT_EQ(f.sub.last.trigger,  PublishTrigger::kEvent);  // Error変化=Event
}

TEST(SystemFlowTest, JobProgressFlowsToJobCap)
{
    Fixture f;
    f.system.Adapter().Push(RIMDataId::kJobProgress, RawValue::Scalar(50));
    f.system.Dispatch();

    ASSERT_TRUE(f.sub.last.capabilities.job.has_value());
    EXPECT_TRUE(f.sub.last.capabilities.job->active);
    EXPECT_EQ(f.sub.last.capabilities.job->progress, 50);
}

TEST(SystemFlowTest, NoChangeNoRepublish)
{
    Fixture f;
    f.system.Adapter().Push(RIMDataId::kTemperature, RawValue::Scalar(25.0));
    f.system.Dispatch();
    const int c1 = f.sub.count;

    // 同値の再postはRegistry変化なし→通知なし→配信なし。
    f.system.Adapter().Push(RIMDataId::kTemperature, RawValue::Scalar(25.0));
    f.system.Dispatch();
    EXPECT_EQ(f.sub.count, c1);
}

TEST(SystemFlowTest, AccessorGetPrinterStatus)
{
    Fixture f;
    DataContext ctx; ctx.faultState = FaultState::kRaised;
    f.system.Adapter().Push(RIMDataId::kFaultCode, RawValue::Scalar(0x10u), &ctx);
    f.system.Dispatch();

    PrinterStatusRequest req;
    req.domains = kDomainFault | kDomainCurrentAll;
    req.includeCapability = true;

    const PrinterStatus st = f.system.Accessor().GetPrinterStatus(req);
    ASSERT_TRUE(st.data.has_value());
    ASSERT_TRUE(st.data->fault.has_value());
    EXPECT_EQ(st.data->fault->activeCount, 1u);
    ASSERT_TRUE(st.capability.has_value());
    ASSERT_TRUE(st.capability->error.has_value());
    EXPECT_TRUE(st.capability->error->hasError);
}
