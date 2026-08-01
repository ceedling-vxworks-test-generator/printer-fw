//
// RIM_AdapterLayer(L1) の非機能要件試験。
//
// 対象は old/ex/Adapter/AdapterLayer仕様設計書.md §12 に定義された4項目:
//   NFR-1: 処理は軽量であること
//   NFR-2: Rule単位で単体テスト可能であること
//   NFR-3: データ種別追加時の変更影響を局所化できること
//   NFR-4: 障害時に影響範囲が RIM_AdapterLayer 内で限定されること
//
// gtest はネットワーク制限下で取得できないため、rim_capi_smoke と同じ方針で
// 標準ライブラリのみに依存する単独の実行ファイルとして実装し、実際に実行して
// 確認する(構文検査のみのスタブに頼らない)。
//

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "PerformanceStatistics.hpp"

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "RawDataInput.hpp"

namespace
{

int g_failures = 0;

} // namespace

#define NFR_CHECK(cond)                                        \
    do                                                          \
    {                                                            \
        if (!(cond))                                             \
        {                                                         \
            std::fprintf(                                         \
                stderr,                                            \
                "NG %s:%d: %s\n",                                   \
                __FILE__,                                            \
                __LINE__,                                             \
                #cond);                                                \
            ++g_failures;                                               \
        }                                                                 \
    } while (0)

namespace
{

//
// --- 動的確保カウンタ ---
// 定常運転中はヒープを触らないことを実測で確認する(移行計画.md §1 と同じ手法:
// operator new を差し替えて計測)。ProcessWide のフックなので、計測区間は
// g_countingEnabled で絞る。
//
std::atomic<long long> g_allocCount{0};
bool                    g_countingEnabled{false};

} // namespace

void* operator new(std::size_t size)
{
    if (g_countingEnabled)
    {
        ++g_allocCount;
    }

    void* p = std::malloc(size == 0 ? 1 : size);

    if (p == nullptr)
    {
        std::abort();
    }

    return p;
}

void operator delete(void* p) noexcept
{
    std::free(p);
}

void operator delete(void* p, std::size_t) noexcept
{
    std::free(p);
}

namespace
{

// 記録するだけの Port。Adapter は抽象にのみ依存するので差し込める。
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

// 素通し規則(NFR-1: 速度/確保回数の計測用。内容そのものは問わない)。
class IdentityRule final : public rim::IRule
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
            raw.scalar);
    }
};

// 0〜100 にクランプする規則(NFR-2/NFR-3 用)。
class ClampingRule final : public rim::IRule
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

        double v = raw.scalar;

        if (v < 0.0)   v = 0.0;
        if (v > 100.0) v = 100.0;

        return rim::RIMValueFactory::CreateDouble(v);
    }
};

// 常に変換不能を返す規則(NFR-4 用)。
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

// 常に固定の Rule を返す(nullptr も可)解決器。
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

    void SwitchTo(
        const rim::IRule* rule)
    {
        rule_ = rule;
    }

private:

    const rim::IRule* rule_;
};

// id ごとに異なる Rule を引く解決器(NFR-3: 機種側テーブルの模擬)。
class TableResolver final : public rim::IRuleResolver
{
public:

    TableResolver(
        const rim::IRule* temperatureRule,
        const rim::IRule* humidityRule)
        : temperatureRule_(temperatureRule)
        , humidityRule_(humidityRule)
    {
    }

    const rim::IRule* SelectRule(
        rim::RIMDataId id) const override
    {
        switch (id)
        {
        case rim::RIMDataId::kTemperatureSensorA:
            return temperatureRule_;

        // ↓ この1エントリだけが「新しいデータ種別を追加する」ために足した差分。
        //   RawDataInput / IRule / IRuleResolver / RawValue (core/adapter) は無改修。
        case rim::RIMDataId::kHumiditySensor:
            return humidityRule_;

        default:
            return nullptr;
        }
    }

private:

    const rim::IRule* temperatureRule_;
    const rim::IRule* humidityRule_;
};

//
// NFR-1: 処理は軽量であること。
// 定常運転を模した1000周期の Push で、動的確保が0回であること・
// レイテンシが突発的に劣化していないことを確認する。
//
void TestLightweightProcessing()
{
    std::printf(
        "[NFR-1] 処理は軽量であること(定常時ゼロ動的確保)\n");

    RecordingPort port;
    IdentityRule  rule;
    FixedResolver resolver(&rule);

    rim::RawDataInput adapter(
        port,
        resolver);

    constexpr int kWarmup     = 100;
    constexpr int kIterations = 1000;

    // ウォームアップ(初回分岐予測・キャッシュ充填等をカウント対象から外す)。
    for (int i = 0; i < kWarmup; ++i)
    {
        adapter.Push(
            rim::RIMDataId::kTemperatureSensorA,
            rim::RawValue::Scalar(i));
    }

    std::vector<std::int64_t> latenciesNs;
    latenciesNs.reserve(kIterations);

    g_allocCount      = 0;
    g_countingEnabled = true;

    for (int i = 0; i < kIterations; ++i)
    {
        const auto start =
            std::chrono::steady_clock::now();

        const auto result =
            adapter.Push(
                rim::RIMDataId::kTemperatureSensorA,
                rim::RawValue::Scalar(i));

        const auto end =
            std::chrono::steady_clock::now();

        NFR_CHECK(result == rim::RIMResult::kOk);

        latenciesNs.push_back(
            std::chrono::duration_cast<
                std::chrono::nanoseconds>(
                    end - start)
                .count());
    }

    g_countingEnabled = false;

    const auto stats =
        perf::CalculateStatistics(
            latenciesNs);

    std::printf(
        "[NFR-1] 動的確保回数: %lld 回 / %d 周期\n",
        static_cast<long long>(
            g_allocCount.load()),
        kIterations);

    std::printf(
        "[NFR-1] Push レイテンシ(ns) min=%lld avg=%lld p95=%lld p99=%lld max=%lld\n",
        static_cast<long long>(stats.min),
        static_cast<long long>(stats.avg),
        static_cast<long long>(stats.p95),
        static_cast<long long>(stats.p99),
        static_cast<long long>(stats.max));

    NFR_CHECK(
        g_allocCount.load() == 0);

    // 突発的な劣化(意図しない O(n) 化等)の検知用。通常は数百ns〜数us。
    NFR_CHECK(
        stats.avg < 1'000'000); // 1ms
}

//
// NFR-2: Rule単位で単体テスト可能であること。
// Port も Resolver も RawDataInput のインスタンスも介さず、Rule 単体を
// 直接呼べることを確認する。
//
void TestRuleIsIndependentlyTestable()
{
    std::printf(
        "[NFR-2] Rule単位で単体テスト可能であること\n");

    ClampingRule rule;

    const auto below =
        rule.Convert(
            rim::RawValue::Scalar(-10.0),
            rim::DataContext{});

    const auto within =
        rule.Convert(
            rim::RawValue::Scalar(50.0),
            rim::DataContext{});

    const auto above =
        rule.Convert(
            rim::RawValue::Scalar(150.0),
            rim::DataContext{});

    double v = 0.0;

    NFR_CHECK(below.has_value());
    NFR_CHECK(
        below.has_value() &&
        rim::RIMValueAccessor::GetDouble(*below, v) &&
        v == 0.0);

    NFR_CHECK(within.has_value());
    NFR_CHECK(
        within.has_value() &&
        rim::RIMValueAccessor::GetDouble(*within, v) &&
        v == 50.0);

    NFR_CHECK(above.has_value());
    NFR_CHECK(
        above.has_value() &&
        rim::RIMValueAccessor::GetDouble(*above, v) &&
        v == 100.0);

    // 型不一致(スカラ規則へ配列を渡す)も、Port 抜きで単体検証できる。
    const int    dummy[1] = {0};
    const auto badKind =
        rule.Convert(
            rim::RawValue::Array(dummy, 1),
            rim::DataContext{});

    NFR_CHECK(!badKind.has_value());
}

//
// NFR-3: データ種別追加時の変更影響を局所化できること。
// 「新しいデータ種別への対応」を Resolver の対応表へ1エントリ足すだけで実現し、
// 既存 id の挙動に影響しないこと・core/adapter を無改修のまま拡張できることを確認する。
//
void TestNewDataKindIsLocalizedExtension()
{
    std::printf(
        "[NFR-3] データ種別追加時の変更影響が局所化されること\n");

    RecordingPort port;
    ClampingRule  temperatureRule;
    IdentityRule  humidityRule; // ← 新規追加したデータ種別用の規則(このファイル内で完結)

    TableResolver resolver(
        &temperatureRule,
        &humidityRule);

    rim::RawDataInput adapter(
        port,
        resolver);

    // 既存id: 追加前と同じ挙動のまま(regression無し)。
    NFR_CHECK(
        adapter.Push(
            rim::RIMDataId::kTemperatureSensorA,
            rim::RawValue::Scalar(200.0)) ==
        rim::RIMResult::kOk);

    double v = 0.0;

    NFR_CHECK(
        rim::RIMValueAccessor::GetDouble(
            port.last.value,
            v) &&
        v == 100.0);

    // 新規追加id: Resolver に1エントリ足しただけで動く。
    NFR_CHECK(
        adapter.Push(
            rim::RIMDataId::kHumiditySensor,
            rim::RawValue::Scalar(55.0)) ==
        rim::RIMResult::kOk);

    NFR_CHECK(
        rim::RIMValueAccessor::GetDouble(
            port.last.value,
            v) &&
        v == 55.0);
}

//
// NFR-4: 障害時に影響範囲が RIM_AdapterLayer 内で限定されること。
// 未知id・変換失敗・型不一致のいずれも、後段(L2)へ流さず・例外を投げず・
// Adapter/Port の状態を破壊しない(直後の正常な Push が成功する)ことを確認する。
//
void TestFailureIsContainedWithinAdapter()
{
    std::printf(
        "[NFR-4] 障害時の影響範囲が RIM_AdapterLayer 内に限定されること\n");

    {
        // 未知id(Resolver が Rule を返さない)。
        RecordingPort port;
        FixedResolver resolver(nullptr);

        rim::RawDataInput adapter(
            port,
            resolver);

        NFR_CHECK(
            adapter.Push(
                rim::RIMDataId::kTemperatureSensorA,
                rim::RawValue::Scalar(1.0)) ==
            rim::RIMResult::kErrConvert);

        // 後段(L2)へは一切流れない。
        NFR_CHECK(port.posts == 0);
    }

    {
        // 変換失敗が連続しても Adapter/Port の状態は壊れず、
        // 直後の正常な Push は問題なく成功する。
        RecordingPort port;
        RejectingRule rejecting;
        IdentityRule  identity;
        FixedResolver resolver(&rejecting);

        rim::RawDataInput adapter(
            port,
            resolver);

        for (int i = 0; i < 100; ++i)
        {
            NFR_CHECK(
                adapter.Push(
                    rim::RIMDataId::kTemperatureSensorA,
                    rim::RawValue::Scalar(1.0)) ==
                rim::RIMResult::kErrConvert);
        }

        NFR_CHECK(port.posts == 0);

        resolver.SwitchTo(&identity);

        NFR_CHECK(
            adapter.Push(
                rim::RIMDataId::kTemperatureSensorA,
                rim::RawValue::Scalar(42.0)) ==
            rim::RIMResult::kOk);

        NFR_CHECK(port.posts == 1);

        double v = 0.0;

        NFR_CHECK(
            rim::RIMValueAccessor::GetDouble(
                port.last.value,
                v) &&
            v == 42.0);
    }
}

} // namespace

int main()
{
    TestLightweightProcessing();
    TestRuleIsIndependentlyTestable();
    TestNewDataKindIsLocalizedExtension();
    TestFailureIsContainedWithinAdapter();

    if (g_failures == 0)
    {
        std::printf(
            "[NFR] 全項目 OK\n");

        return 0;
    }

    std::printf(
        "[NFR] %d 件 NG\n",
        g_failures);

    return 1;
}
