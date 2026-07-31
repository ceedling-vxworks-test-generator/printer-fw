#include <gtest/gtest.h>

#include <cstdint>

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "RawDataInput.hpp"

//
// Adapter の受理点。
//
// 旧実装はセンサ種別ごとにアダプタクラスを持っていた(摂氏用・ケルビン用など)。
// 現在は受理点が Push 1本で、id -> Rule の対応は Product が表で持つ。
// ここでは Core 側の振る舞い(規則を引いて正規化し、Port へ渡す)だけを確認する。
//

namespace
{

// 渡された項目を記録するだけの Port。Adapter は抽象にしか依存しないので差し込める。
class RecordingPort final : public rim::ICentralInputPort
{
public:

    rim::RIMResult Post(
        const rim::RIMDataItem& item) override
    {
        last = item;
        ++posts;

        return result;
    }

    rim::RIMDataItem last{};
    int              posts{0};
    rim::RIMResult   result{rim::RIMResult::kOk};
};

// 生値を倍にするだけの規則。
class DoublingRule final : public rim::IRule
{
public:

    std::optional<rim::RIMValue> Convert(
        const rim::RawValue& raw,
        const rim::DataContext&) const override
    {
        if (raw.kind != rim::RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return rim::RIMValueFactory::CreateDouble(
            raw.scalar * 2.0);
    }
};

// 常に変換不能を返す規則。
class RejectingRule final : public rim::IRule
{
public:

    std::optional<rim::RIMValue> Convert(
        const rim::RawValue&,
        const rim::DataContext&) const override
    {
        return std::nullopt;
    }
};

// 指定した規則を必ず返す解決器(nullptr も返せる)。
class FixedResolver final : public rim::IRuleResolver
{
public:

    explicit FixedResolver(
        const rim::IRule* rule)
        : rule_(rule)
    {
    }

    const rim::IRule* SelectRule(
        rim::RIMDataId) const override
    {
        return rule_;
    }

private:

    const rim::IRule* rule_;
};

constexpr rim::RIMDataId kAnyId =
    rim::RIMDataId::kTemperatureSensorA;

}

TEST(
    RawDataInputTest,
    PushConvertsAndPosts)
{
    RecordingPort port;
    DoublingRule  rule;
    FixedResolver resolver(&rule);

    rim::RawDataInput adapter(
        port,
        resolver);

    EXPECT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(21.0)),
        rim::RIMResult::kOk);

    ASSERT_EQ(
        port.posts,
        1);

    EXPECT_EQ(
        port.last.id,
        kAnyId);

    double value = 0.0;

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            port.last.value,
            value));

    // 規則が適用されている(素通ししていない)
    EXPECT_DOUBLE_EQ(
        value,
        42.0);
}

TEST(
    RawDataInputTest,
    UnknownIdIsRejectedWithoutPosting)
{
    RecordingPort port;
    FixedResolver resolver(nullptr);

    rim::RawDataInput adapter(
        port,
        resolver);

    EXPECT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(1.0)),
        rim::RIMResult::kErrConvert);

    // 規則が無いものは後段へ流さない
    EXPECT_EQ(
        port.posts,
        0);
}

TEST(
    RawDataInputTest,
    ConversionFailureIsNotPosted)
{
    RecordingPort port;
    RejectingRule rule;
    FixedResolver resolver(&rule);

    rim::RawDataInput adapter(
        port,
        resolver);

    EXPECT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(1.0)),
        rim::RIMResult::kErrConvert);

    EXPECT_EQ(
        port.posts,
        0);
}

TEST(
    RawDataInputTest,
    ContextIsCarriedToTheItem)
{
    RecordingPort port;
    DoublingRule  rule;
    FixedResolver resolver(&rule);

    rim::RawDataInput adapter(
        port,
        resolver);

    rim::DataContext ctx{};

    ctx.unit       = rim::SourceUnit::kCelsius;
    ctx.scaleX1000 = 500;

    ASSERT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(1.0),
            &ctx),
        rim::RIMResult::kOk);

    // context は後段でも使うため、項目に載せて運ぶ
    ASSERT_TRUE(
        port.last.context.unit.has_value());

    EXPECT_EQ(
        *port.last.context.unit,
        rim::SourceUnit::kCelsius);

    ASSERT_TRUE(
        port.last.context.scaleX1000.has_value());

    EXPECT_EQ(
        *port.last.context.scaleX1000,
        500);
}

TEST(
    RawDataInputTest,
    ContextIsOptional)
{
    RecordingPort port;
    DoublingRule  rule;
    FixedResolver resolver(&rule);

    rim::RawDataInput adapter(
        port,
        resolver);

    ASSERT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(1.0)),
        rim::RIMResult::kOk);

    // 未指定なら空のまま(勝手な既定値を入れない)
    EXPECT_FALSE(
        port.last.context.unit.has_value());
}

TEST(
    RawDataInputTest,
    PortFailureIsPropagated)
{
    RecordingPort port;
    DoublingRule  rule;
    FixedResolver resolver(&rule);

    port.result = rim::RIMResult::kErrPost;

    rim::RawDataInput adapter(
        port,
        resolver);

    EXPECT_EQ(
        adapter.Push(
            kAnyId,
            rim::RawValue::Scalar(1.0)),
        rim::RIMResult::kErrPost);
}
