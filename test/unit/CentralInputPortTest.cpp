#include <gtest/gtest.h>

#include <cstdint>

#include "datastore/CentralInputPort.hpp"
#include "datastore/InputQueues.hpp"
#include "datastore/CurrentValueBuffer.hpp"
#include "datastore/IInputClassifier.hpp"

using namespace rim;

// 入力性質の判別とレーン振り分けは L2(CentralInputPort)の責務。
// (以前は Adapter が分類して3種postを選んでいたが、その責務はここへ移動した。
//  Adapter側の検証は AdapterTest.cpp を参照。)

namespace
{
class TestClassifier final : public IInputClassifier
{
public:
    std::optional<InputKind> Classify(RIMDataId id) const override
    {
        switch (id) {
            case RIMDataId::kFaultCode:   return InputKind::kFault;
            case RIMDataId::kJobProgress: return InputKind::kOperationReport;
            case RIMDataId::kTemperature: return InputKind::kCurrentValue;
            default:                      return std::nullopt;  // 分類不能
        }
    }
};

struct Fixture
{
    FaultInputQueue      faultQueue;
    OperationReportQueue operationQueue;
    CurrentValueBuffer   currentBuffer;
    TestClassifier       classifier;
    CentralInputPort     port{faultQueue, operationQueue, currentBuffer, classifier};

    static RIMDataItem Item(RIMDataId id, RIMValue v)
    {
        RIMDataItem it; it.id = id; it.value = v; return it;
    }
};
} // namespace

TEST(CentralInputPortTest, FaultIdGoesToFaultQueue)
{
    Fixture f;
    auto it = Fixture::Item(RIMDataId::kFaultCode, RIMValue::FaultCode(0x10u));

    EXPECT_EQ(f.port.Post(it), RIMResult::kOk);
    EXPECT_EQ(f.faultQueue.Size(), 1u);
    EXPECT_EQ(f.operationQueue.Size(), 0u);
    EXPECT_TRUE(f.currentBuffer.TakeUpdatedValues().empty());
}

TEST(CentralInputPortTest, OperationIdGoesToOperationQueue)
{
    Fixture f;
    auto it = Fixture::Item(RIMDataId::kJobProgress, RIMValue::JobProgress(50));

    EXPECT_EQ(f.port.Post(it), RIMResult::kOk);
    EXPECT_EQ(f.operationQueue.Size(), 1u);
    EXPECT_EQ(f.faultQueue.Size(), 0u);
}

TEST(CentralInputPortTest, CurrentValueIdGoesToCurrentBuffer)
{
    Fixture f;
    auto it = Fixture::Item(RIMDataId::kTemperature, RIMValue::CelsiusX100(2500));

    EXPECT_EQ(f.port.Post(it), RIMResult::kOk);
    EXPECT_EQ(f.faultQueue.Size(), 0u);
    EXPECT_EQ(f.operationQueue.Size(), 0u);
    EXPECT_EQ(f.currentBuffer.TakeUpdatedValues().size(), 1u);
}

TEST(CentralInputPortTest, UnclassifiableIdReturnsClassifyError)
{
    Fixture f;
    // kUnitAlive は TestClassifier が分類しない → kErrClassify。
    auto it = Fixture::Item(RIMDataId::kUnitAlive, RIMValue::Bool(true));

    EXPECT_EQ(f.port.Post(it), RIMResult::kErrClassify);
    EXPECT_EQ(f.faultQueue.Size(), 0u);
    EXPECT_EQ(f.operationQueue.Size(), 0u);
    EXPECT_TRUE(f.currentBuffer.TakeUpdatedValues().empty());
}

TEST(CentralInputPortTest, ValueTypeIsValidatedPerKind)
{
    Fixture f;
    // OperationReport は kJobProgress 型でなければ受理しない。
    auto wrongOp = Fixture::Item(RIMDataId::kJobProgress, RIMValue::Percent(50));
    EXPECT_EQ(f.port.Post(wrongOp), RIMResult::kErrKindMismatch);
    EXPECT_EQ(f.operationQueue.Size(), 0u);

    // CurrentValue は kNone(未設定)を受理しない。
    auto noneVal = Fixture::Item(RIMDataId::kTemperature, RIMValue{});
    EXPECT_EQ(f.port.Post(noneVal), RIMResult::kErrKindMismatch);

    // Fault は AllCleared 以外でキー源(context.key か value=FaultCode)が必要。
    auto noKey = Fixture::Item(RIMDataId::kFaultCode, RIMValue::Percent(1));
    EXPECT_EQ(f.port.Post(noKey), RIMResult::kErrKindMismatch);
    EXPECT_EQ(f.faultQueue.Size(), 0u);
}

TEST(CentralInputPortTest, FullQueueReportsPostErrorForLossDetection)
{
    Fixture f;
    auto it = Fixture::Item(RIMDataId::kFaultCode, RIMValue::FaultCode(1u));

    // 容量ぶんは受理される。
    for (std::size_t i = 0; i < kFaultQueueDepth; ++i) {
        EXPECT_EQ(f.port.Post(it), RIMResult::kOk) << "i=" << i;
    }
    // 満杯以降は喪失として kErrPost を返す。
    EXPECT_EQ(f.port.Post(it), RIMResult::kErrPost);
}
