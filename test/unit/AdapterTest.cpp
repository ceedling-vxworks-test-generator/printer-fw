#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "adapter/RawDataInput.hpp"
#include "adapter/RawValue.hpp"
#include "adapter/IRule.hpp"
#include "adapter/IRuleResolver.hpp"

#include "datastore/ICentralInputPort.hpp"
#include "datastore/IInputClassifier.hpp"
#include "datastore/RIMDataItem.hpp"

using namespace rim;

namespace
{
// --- モック CentralInputPort: post された item を性質別に記録する ---
class MockPort final : public ICentralInputPort
{
public:
    RIMResult PostFaultInput(const RIMDataItem& item) override      { fault.push_back(item); return RIMResult::kOk; }
    RIMResult PostOperationReport(const RIMDataItem& item) override { operation.push_back(item); return RIMResult::kOk; }
    RIMResult PostCurrentValueInput(const RIMDataItem& item) override { current.push_back(item); return RIMResult::kOk; }

    std::vector<RIMDataItem> fault;
    std::vector<RIMDataItem> operation;
    std::vector<RIMDataItem> current;
};

// --- テスト用 Rule 群(scalar / array / struct) ---
class ScalarCelsiusRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::CelsiusX100(static_cast<std::int32_t>(raw.scalar * 100.0));
    }
};

// 配列(uint8のセル群)の平均を Percent に正規化。
class ArrayAvgPercentRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kBytes || raw.count == 0) return std::nullopt;
        unsigned sum = 0;
        for (std::size_t i = 0; i < raw.count; ++i) sum += raw.ElementAt<std::uint8_t>(i);
        return RIMValue::Percent(static_cast<std::uint8_t>(sum / raw.count));
    }
};

struct SamplePacket { std::uint8_t level; };

// 構造体からフィールドを取り出して Percent に正規化。
class StructPercentRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kBytes || raw.count != 1) return std::nullopt;
        return RIMValue::Percent(raw.As<SamplePacket>()->level);
    }
};

class ScalarFaultRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::FaultCode(static_cast<std::uint32_t>(raw.scalar));
    }
};

class ScalarProgressRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::JobProgress(static_cast<std::uint8_t>(raw.scalar));
    }
};

// --- テスト用 RuleResolver + Classifier(id→Rule, id→性質) ---
class TestProfile final : public IRuleResolver, public IInputClassifier
{
public:
    const IRule* SelectRule(RIMDataId id) const override
    {
        switch (id) {
            case RIMDataId::kTemperature: return &celsius_;    // scalar
            case RIMDataId::kHumidity:    return &arrayAvg_;   // array
            case RIMDataId::kPressure:    return &structPct_;  // struct
            case RIMDataId::kFaultCode:   return &fault_;      // scalar
            case RIMDataId::kJobProgress: return &progress_;   // scalar
            default:                      return nullptr;
        }
    }
    std::optional<InputKind> Classify(RIMDataId id) const override
    {
        switch (id) {
            case RIMDataId::kFaultCode:   return InputKind::kFault;
            case RIMDataId::kJobProgress: return InputKind::kOperationReport;
            case RIMDataId::kTemperature:
            case RIMDataId::kHumidity:
            case RIMDataId::kPressure:    return InputKind::kCurrentValue;
            default:                      return std::nullopt;
        }
    }
private:
    ScalarCelsiusRule   celsius_;
    ArrayAvgPercentRule arrayAvg_;
    StructPercentRule   structPct_;
    ScalarFaultRule     fault_;
    ScalarProgressRule  progress_;
};

struct Fixture
{
    MockPort     port;
    TestProfile  profile;
    RawDataInput adapter{port, profile, profile};
};
} // namespace

TEST(AdapterTest, ScalarPushRoutesToCurrentValue)
{
    Fixture f;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kTemperature, RawValue::Scalar(25.0)), RIMResult::kOk);

    ASSERT_EQ(f.port.current.size(), 1u);
    EXPECT_TRUE(f.port.fault.empty());
    EXPECT_TRUE(f.port.operation.empty());
    EXPECT_EQ(f.port.current[0].value.type, ValueType::kCelsiusX100);
    EXPECT_EQ(f.port.current[0].value.u.celsiusX100, 2500);
}

TEST(AdapterTest, ArrayPushIsNormalized)
{
    Fixture f;
    const std::uint8_t cells[3] = {40, 50, 60};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kHumidity, RawValue::Array(cells, 3)), RIMResult::kOk);

    ASSERT_EQ(f.port.current.size(), 1u);
    EXPECT_EQ(f.port.current[0].value.type, ValueType::kPercent);
    EXPECT_EQ(f.port.current[0].value.u.percent, 50);  // 平均
}

TEST(AdapterTest, StructPushIsNormalized)
{
    Fixture f;
    const SamplePacket pkt{77};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kPressure, RawValue::Struct(pkt)), RIMResult::kOk);

    ASSERT_EQ(f.port.current.size(), 1u);
    EXPECT_EQ(f.port.current[0].value.type, ValueType::kPercent);
    EXPECT_EQ(f.port.current[0].value.u.percent, 77);
}

TEST(AdapterTest, FaultRoutesToFaultPostWithContext)
{
    Fixture f;
    DataContext ctx; ctx.faultState = FaultState::kRaised;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kFaultCode, RawValue::Scalar(0x10u), &ctx), RIMResult::kOk);

    ASSERT_EQ(f.port.fault.size(), 1u);
    EXPECT_EQ(f.port.fault[0].value.type, ValueType::kFaultCode);
    EXPECT_EQ(f.port.fault[0].value.u.faultCode, 0x10u);
    ASSERT_TRUE(f.port.fault[0].context.faultState.has_value());
    EXPECT_EQ(*f.port.fault[0].context.faultState, FaultState::kRaised);
}

TEST(AdapterTest, JobProgressRoutesToOperationPost)
{
    Fixture f;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kJobProgress, RawValue::Scalar(50)), RIMResult::kOk);

    ASSERT_EQ(f.port.operation.size(), 1u);
    EXPECT_EQ(f.port.operation[0].value.type, ValueType::kJobProgress);
    EXPECT_EQ(f.port.operation[0].value.u.jobProgress, 50);
}

TEST(AdapterTest, UnknownIdReturnsConvertError)
{
    Fixture f;
    // kUnitAlive は TestProfile が Rule 未定義 → kErrConvert。
    EXPECT_EQ(f.adapter.Push(RIMDataId::kUnitAlive, RawValue::Scalar(1.0)), RIMResult::kErrConvert);
    EXPECT_TRUE(f.port.current.empty());
}

TEST(AdapterTest, WrongRawKindReturnsConvertError)
{
    Fixture f;
    // scalar 用 id(kTemperature)へ構造体を渡す → Rule が拒否 → kErrConvert。
    const SamplePacket pkt{10};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kTemperature, RawValue::Struct(pkt)), RIMResult::kErrConvert);
    EXPECT_TRUE(f.port.current.empty());
}
