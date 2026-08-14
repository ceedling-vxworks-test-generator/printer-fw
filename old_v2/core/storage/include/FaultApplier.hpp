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

#include <cstddef>
#include <cstdint>

#include "ErrorRepository.hpp"
#include "FaultSnapshotEntry.hpp"
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

    //
    // FaultInfoList 相当(「今存在する異常の全件」スナップショット)を反映する。
    //
    // 単発の Apply とは別経路であり、既存の Push/Apply と併存する(置き換えない)。
    // 渡された一覧を「今の全件」として ErrorRepository と突き合わせる:
    //   - 一覧にあり、未登録のコード → 新規登録する(state は一覧の値をそのまま使う。
    //     HEALED であっても新規登録する。回復済みとして扱いたい、という要件のため)
    //   - 一覧にあり、登録済みのコード → state を更新する
    //   - 一覧にない、登録済みのコード → 削除する(回復ではなく消える。
    //     全件スナップショットに無い = もう存在しない異常、という前提のため)
    //
    // 反映して一覧が変化したら true。
    //
    bool ApplySnapshot(
        const FaultSnapshotEntry* entries,
        std::size_t count)
    {
        bool changed = false;

        // 削除: 現在の登録のうち、新スナップショットに無いもの。
        // Remove は詰めて添字がずれるため、後ろから見る。
        const ErrorRepository::Container& current =
            repository_.GetAll();

        for (std::size_t i = current.Size(); i > 0; --i)
        {
            const std::uint32_t code =
                current[i - 1].errorCode;

            if (!Contains(
                    entries,
                    count,
                    code))
            {
                repository_.Remove(
                    code);

                changed = true;
            }
        }

        // 追加/更新
        for (std::size_t i = 0; i < count; ++i)
        {
            const FaultSnapshotEntry& entry =
                entries[i];

            if (!repository_.SetState(
                    entry.errorCode,
                    entry.state))
            {
                // 未登録 → 新規登録(HEALED でもそのまま登録する)
                repository_.Add(
                    {
                        entry.errorCode,
                        entry.state,
                        ResolveSeverity(entry.errorCode)
                    });
            }

            changed = true;
        }

        return changed;
    }

private:

    static bool Contains(
        const FaultSnapshotEntry* entries,
        std::size_t count,
        std::uint32_t code)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            if (entries[i].errorCode == code)
            {
                return true;
            }
        }

        return false;
    }

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
