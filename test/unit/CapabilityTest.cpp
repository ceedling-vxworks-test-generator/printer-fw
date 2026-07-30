#include <gtest/gtest.h>

#include "capability/PrinterACapabilityBuilder.hpp"
#include "capability/CapabilityDiffChecker.hpp"
#include "capability/CapabilityPriorityChecker.hpp"

using namespace rim;

TEST(CapabilityBuilderTest, ErrorAndSafetyMakeNotPrintable)
{
    MachineSnapshot snap;
    FaultSnapshot fs; fs.activeCount = 2; snap.fault = fs;

    CurrentValueSnapshot cv;
    SafetySnapshot sf; sf.coverOpen = true; cv.safety = sf;
    snap.currentValue = cv;

    PrinterACapabilityBuilder b;
    const CapabilitySet cap = b.Build(snap);

    ASSERT_TRUE(cap.error.has_value());
    EXPECT_TRUE(cap.error->hasError);
    EXPECT_EQ(cap.error->activeCount, 2u);

    ASSERT_TRUE(cap.safety.has_value());
    EXPECT_FALSE(cap.safety->safe);

    ASSERT_TRUE(cap.print.has_value());
    EXPECT_FALSE(cap.print->printable);  // fault + cover open
}

TEST(CapabilityBuilderTest, HighTemperatureOutOfRange)
{
    MachineSnapshot snap;
    CurrentValueSnapshot cv;
    EnvironmentSnapshot env; env.temperatureKelvinX100 = 34315;  // 70℃ = 343.15K
    cv.environment = env;
    snap.currentValue = cv;

    PrinterACapabilityBuilder b;
    const CapabilitySet cap = b.Build(snap);
    ASSERT_TRUE(cap.env.has_value());
    EXPECT_FALSE(cap.env->inRange);
}

TEST(CapabilityDiffCheckerTest, DetectsChangedKinds)
{
    CapabilityDiffChecker d;
    CapabilitySet prev, curr;

    EXPECT_FALSE(d.HasDifference(prev, curr).changed);

    curr.error = ErrorCap{true, 1};
    const DiffResult r = d.HasDifference(prev, curr);
    EXPECT_TRUE(r.changed);
    EXPECT_TRUE(r.changedKinds & KindBit(CapabilityKind::kError));
    EXPECT_FALSE(r.changedKinds & KindBit(CapabilityKind::kJob));
}

TEST(CapabilityPriorityCheckerTest, ErrorIsCritical)
{
    CapabilityPriorityChecker p;
    CapabilitySet cap;
    cap.error = ErrorCap{true, 1};
    EXPECT_EQ(p.Check(cap), CapabilityPriority::kCritical);

    CapabilitySet cap2;
    cap2.consumable = ConsumableCap{true, false};  // ink low
    EXPECT_EQ(p.Check(cap2), CapabilityPriority::kHigh);
}
