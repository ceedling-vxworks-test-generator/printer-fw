#pragma once

//
// FaultApplier - 異常を伴うデータ項目を ErrorRepository へ反映する。
//
// Adapter の受理点は Push 1本であり、入力の性質(現在値か異常報告か)を
// 判別するのは L2 の責務である。判別の材料は DataContext::faultState で、
// 値が入っていればその項目は異常報告として扱う。
//
// 格納先は分けるが **レーンは1本のまま** である(値は ValueStore、異常は
// ErrorRepository)。性質ごとにキューを分ける構成は採らない。
//
// 異常コードの取り出し順:
//   1. context.key(異常コレクションのキー。これが本来の置き場)
//   2. 値が UInt32 / Int32 ならその値
//   どちらも取れなければ何もしない(0 番の異常を勝手に作らない)。
//

#include <cstdint>

#include "ErrorRepository.hpp"
#include "IErrorDefinitionRegistry.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueAccessor.hpp"

namespace rim
{

class FaultApplier
{
public:

    // definitions は省略可(nullptr なら重大度は kError 既定)。
    explicit FaultApplier(
        ErrorRepository& repository,
        const IErrorDefinitionRegistry* definitions = nullptr)
        : repository_(repository)
        , definitions_(definitions)
    {
    }

    void SetDefinitions(
        const IErrorDefinitionRegistry* definitions)
    {
        definitions_ = definitions;
    }

    // この項目が異常報告かどうか。
    static bool IsFault(
        const RIMDataItem& item)
    {
        return item.context.faultState.has_value();
    }

    //
    // 異常報告を反映する。反映して異常一覧が変化したら true。
    // (true のときだけ Capability を作り直せばよい)
    //
    bool Apply(
        const RIMDataItem& item)
    {
        if (!item.context.faultState)
        {
            return false;
        }

        const FaultState state =
            *item.context.faultState;

        // 全クリアだけは異常コードを必要としない
        if (state == FaultState::kAllCleared)
        {
            repository_.Clear();
            return true;
        }

        std::uint32_t code = 0;

        if (!ExtractCode(
                item,
                code))
        {
            return false;   // コードが取れない = 何を指しているか分からないので触らない
        }

        switch (state)
        {
        case FaultState::kRaised:
            return repository_.Add(
                {
                    code,
                    ErrorState::kActive,
                    ResolveSeverity(code)
                });

        case FaultState::kCleared:
            return repository_.Remove(
                code);

        case FaultState::kUpdatedActive:
            return repository_.SetState(
                code,
                ErrorState::kActive);

        case FaultState::kUpdatedHeal:
            return repository_.SetState(
                code,
                ErrorState::kRecovered);

        case FaultState::kNone:
        default:
            return false;
        }
    }

private:

    static bool ExtractCode(
        const RIMDataItem& item,
        std::uint32_t& out)
    {
        // 本来の置き場は context.key
        if (item.context.key)
        {
            out = *item.context.key;
            return true;
        }

        // 値として送られてきた場合も受ける
        std::uint32_t u32 = 0;

        if (RIMValueAccessor::GetUInt32(
                item.value,
                u32))
        {
            out = u32;
            return true;
        }

        std::int32_t i32 = 0;

        if (RIMValueAccessor::GetInt32(
                item.value,
                i32) &&
            i32 >= 0)
        {
            out = static_cast<std::uint32_t>(i32);
            return true;
        }

        return false;
    }

    ErrorSeverity ResolveSeverity(
        std::uint32_t code) const
    {
        ErrorSeverity severity =
            ErrorSeverity::kError;

        if (definitions_ != nullptr)
        {
            definitions_->TryGetSeverity(
                code,
                severity);
        }

        return severity;
    }

    ErrorRepository& repository_;

    const IErrorDefinitionRegistry* definitions_;
};

} // namespace rim
