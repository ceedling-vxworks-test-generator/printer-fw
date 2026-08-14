#pragma once

//
// FaultSnapshotEntry - FaultInfoList(構造体一覧)1件分。
//
// 単発の DataContext::faultState(発生/解除/更新の**イベント**)とは違い、
// こちらは「今この時点でその異常コードがどの状態か」という**現在値**を表す。
// ErrorState(kActive/kRecovered)は元々この2値の意味であり、そのまま使う
// (rim::FaultState は使わない。あちらは遷移イベントで意味が異なる)。
//

#include <cstdint>

#include "ErrorInfo.hpp"

namespace rim
{

struct FaultSnapshotEntry
{
    std::uint32_t errorCode{};

    ErrorState state{
        ErrorState::kActive};
};

} // namespace rim
