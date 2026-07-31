#include "ProductBinding.hpp"

#include <cstddef>
#include <mutex>
#include <new>

#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"

namespace rim
{

//
// core/common/include/ProductBinding.hpp で **宣言だけ**されている関数の実装。
//
// Core はこのファイルの存在を知らない(core → products のインクルードは無い)。
// リンク時に解決されるので、機種を差し替えるときはリンクする product ライブラリを
// 変えるだけでよい。
//

namespace
{

// 同時に生きられるランタイムの数。
// 通常運用では1つだが、試験で複数ハンドルを開くことがあるため余裕を持たせてある。
constexpr std::size_t kMaxRuntimeInstances = 4;

//
// 静的プール。動的確保をしないための固定枠。
//
// PrinterACapabilityRuleSet は ErrorRepository への参照を持つため、
// **ランタイムごとに別インスタンスが要る**(関数ローカル static を1つ共有すると、
// 2つ目のハンドルが1つ目の ErrorRepository を見てしまう)。
//
struct Pool
{
    // 生存中だけ構築するため、生バイト列 + placement new で管理する。
    alignas(PrinterACapabilityRuleSet)
        unsigned char storage[sizeof(PrinterACapabilityRuleSet)] {};

    bool used {false};
};

Pool       g_pool[kMaxRuntimeInstances];
std::mutex g_poolMutex;

} // namespace

ICapabilityRuleProvider* CreateProductCapabilityRuleProvider(
    const ProductServices& services)
{
    std::lock_guard<std::mutex> lock(g_poolMutex);

    for (auto& slot : g_pool)
    {
        if (slot.used)
        {
            continue;
        }

        slot.used = true;

        return new (slot.storage)
            PrinterACapabilityRuleSet(
                services.errorRepository);
    }

    // 枠不足。Core 側は nullptr を「規則なし」として扱う。
    return nullptr;
}

void DestroyProductCapabilityRuleProvider(
    ICapabilityRuleProvider* provider)
{
    if (provider == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_poolMutex);

    // 基底ポインタのままアドレス比較はできないため、必ず派生へ落としてから比べる。
    auto* typed =
        static_cast<PrinterACapabilityRuleSet*>(
            provider);

    for (auto& slot : g_pool)
    {
        if (!slot.used)
        {
            continue;
        }

        if (static_cast<void*>(slot.storage) !=
            static_cast<void*>(typed))
        {
            continue;
        }

        typed->~PrinterACapabilityRuleSet();

        slot.used = false;

        return;
    }
}

} // namespace rim
