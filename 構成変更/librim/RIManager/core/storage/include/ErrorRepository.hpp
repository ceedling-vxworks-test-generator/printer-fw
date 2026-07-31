#pragma once

#include <cstddef>
#include <cstdint>

#include "ErrorInfo.hpp"
#include "FixedVector.hpp"
#include "RIMConfig.hpp"

namespace rim
{

//
// ErrorRepository - 発生中の異常一覧。
//
// 旧実装は std::vector<ErrorInfo> だったため、異常が増えるたびに動的確保が
// 起きていた。固定容量(kMaxErrors)にして確保を無くしてある。
// 上限を超えた追加は false を返す(黙って伸びない)。
//
class ErrorRepository
{
public:

    using Container =
        FixedVector<ErrorInfo, kMaxErrors>;

    // 追加する。同じ errorCode が既にあれば false(重複は持たない)。
    // 上限に達している場合も false。
    bool Add(
        const ErrorInfo& info)
    {
        if (Find(info.errorCode) != nullptr)
        {
            return false;
        }

        return errors_.PushBack(
            info);
    }

    bool Remove(
        std::uint32_t errorCode)
    {
        for (std::size_t i = 0; i < errors_.Size(); ++i)
        {
            if (errors_[i].errorCode != errorCode)
            {
                continue;
            }

            errors_.Erase(i);

            return true;
        }

        return false;
    }

    bool SetState(
        std::uint32_t errorCode,
        ErrorState state)
    {
        ErrorInfo* found =
            Find(errorCode);

        if (found == nullptr)
        {
            return false;
        }

        found->state = state;

        return true;
    }

    const Container& GetAll() const
    {
        return errors_;
    }

    // 上限に達しているか(これ以上 Add できない)。
    bool Full() const
    {
        return errors_.Full();
    }

    void Clear()
    {
        errors_.Clear();
    }

private:

    ErrorInfo* Find(
        std::uint32_t errorCode)
    {
        for (std::size_t i = 0; i < errors_.Size(); ++i)
        {
            if (errors_[i].errorCode == errorCode)
            {
                return &errors_[i];
            }
        }

        return nullptr;
    }

    Container errors_;
};

}
