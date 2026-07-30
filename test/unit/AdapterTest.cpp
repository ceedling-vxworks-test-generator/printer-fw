#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "adapter/RawDataInput.hpp"
#include "adapter/RawValue.hpp"
#include "adapter/IRule.hpp"
#include "adapter/IRuleResolver.hpp"

#include "datastore/ICentralInputPort.hpp"
#include "datastore/RIMDataItem.hpp"

using namespace rim;

namespace
{
// --- モック CentralInputPort: Post された item を記録する ---
// Adapter は性質判別を行わない(L2の責務)ため、レーン別ではなく単一列で記録する。
// 「どのレーンへ振り分けられるか」の検証は CentralInputPortTest.cpp にある。
class MockPort final : public ICentralInputPort
{
public:
    RIMResult Post(const RIMDataItem& item) override { posted.push_back(item); return result; }

    std::vector<RIMDataItem> posted;
    RIMResult                result{RIMResult::kOk};  // Postの戻り値を差し替えて透過を検証する
};

// --- テスト用 Rule 群(scalar / array / struct) ---
class ScalarKelvinRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::KelvinX100(static_cast<std::int32_t>(raw.scalar * 100.0));
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

// --- テスト用 RuleResolver(id→Rule) ---
// Adapter が依存するのは IRuleResolver のみ(IInputClassifier は L2 側の依存に移った)。
class TestProfile final : public IRuleResolver
{
public:
    const IRule* SelectRule(RIMDataId id) const override
    {
        switch (id) {
            case RIMDataId::kTemperature: return &kelvin_;     // scalar
            case RIMDataId::kHumidity:    return &arrayAvg_;   // array
            case RIMDataId::kPressure:    return &structPct_;  // struct
            case RIMDataId::kFaultCode:   return &fault_;      // scalar
            case RIMDataId::kJobProgress: return &progress_;   // scalar
            default:                      return nullptr;
        }
    }
private:
    ScalarKelvinRule    kelvin_;
    ArrayAvgPercentRule arrayAvg_;
    StructPercentRule   structPct_;
    ScalarFaultRule     fault_;
    ScalarProgressRule  progress_;
};

struct Fixture
{
    MockPort     port;
    TestProfile  profile;
    RawDataInput adapter{port, profile};
};
} // namespace

TEST(AdapterTest, ScalarPushIsNormalizedAndPosted)
{
    Fixture f;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kTemperature, RawValue::Scalar(25.0)), RIMResult::kOk);

    ASSERT_EQ(f.port.posted.size(), 1u);
    EXPECT_EQ(f.port.posted[0].id, RIMDataId::kTemperature);
    EXPECT_EQ(f.port.posted[0].value.type, ValueType::kKelvinX100);
    EXPECT_EQ(f.port.posted[0].value.u.kelvinX100, 2500);
}

TEST(AdapterTest, ArrayPushIsNormalized)
{
    Fixture f;
    const std::uint8_t cells[3] = {40, 50, 60};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kHumidity, RawValue::Array(cells, 3)), RIMResult::kOk);

    ASSERT_EQ(f.port.posted.size(), 1u);
    EXPECT_EQ(f.port.posted[0].value.type, ValueType::kPercent);
    EXPECT_EQ(f.port.posted[0].value.u.percent, 50);  // 平均
}

TEST(AdapterTest, StructPushIsNormalized)
{
    Fixture f;
    const SamplePacket pkt{77};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kPressure, RawValue::Struct(pkt)), RIMResult::kOk);

    ASSERT_EQ(f.port.posted.size(), 1u);
    EXPECT_EQ(f.port.posted[0].value.type, ValueType::kPercent);
    EXPECT_EQ(f.port.posted[0].value.u.percent, 77);
}

TEST(AdapterTest, ContextIsPassedThroughUnchanged)
{
    Fixture f;
    DataContext ctx; ctx.faultState = FaultState::kRaised;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kFaultCode, RawValue::Scalar(0x10u), &ctx), RIMResult::kOk);

    ASSERT_EQ(f.port.posted.size(), 1u);
    EXPECT_EQ(f.port.posted[0].value.type, ValueType::kFaultCode);
    EXPECT_EQ(f.port.posted[0].value.u.faultCode, 0x10u);
    // Adapter は context を加工せず素通しする。
    ASSERT_TRUE(f.port.posted[0].context.faultState.has_value());
    EXPECT_EQ(*f.port.posted[0].context.faultState, FaultState::kRaised);
}

TEST(AdapterTest, JobProgressIsNormalized)
{
    Fixture f;
    EXPECT_EQ(f.adapter.Push(RIMDataId::kJobProgress, RawValue::Scalar(50)), RIMResult::kOk);

    ASSERT_EQ(f.port.posted.size(), 1u);
    EXPECT_EQ(f.port.posted[0].value.type, ValueType::kJobProgress);
    EXPECT_EQ(f.port.posted[0].value.u.jobProgress, 50);
}

TEST(AdapterTest, UnknownIdReturnsConvertError)
{
    Fixture f;
    // kUnitAlive は TestProfile が Rule 未定義 → kErrConvert。
    EXPECT_EQ(f.adapter.Push(RIMDataId::kUnitAlive, RawValue::Scalar(1.0)), RIMResult::kErrConvert);
    EXPECT_TRUE(f.port.posted.empty());
}

TEST(AdapterTest, WrongRawKindReturnsConvertError)
{
    Fixture f;
    // scalar 用 id(kTemperature)へ構造体を渡す → Rule が拒否 → kErrConvert。
    const SamplePacket pkt{10};
    EXPECT_EQ(f.adapter.Push(RIMDataId::kTemperature, RawValue::Struct(pkt)), RIMResult::kErrConvert);
    EXPECT_TRUE(f.port.posted.empty());
}

// Post の戻り値(L2が返すエラー)はそのまま呼出元へ透過すること。
TEST(AdapterTest, PortErrorIsPropagated)
{
    Fixture f;
    f.port.result = RIMResult::kErrPost;   // キュー満杯=喪失を模擬
    EXPECT_EQ(f.adapter.Push(RIMDataId::kTemperature, RawValue::Scalar(25.0)), RIMResult::kErrPost);
    EXPECT_EQ(f.port.posted.size(), 1u);   // post自体は試行される
}
