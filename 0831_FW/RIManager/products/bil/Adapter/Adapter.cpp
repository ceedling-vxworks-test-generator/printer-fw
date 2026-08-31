#include "Adapter.hpp"

#include "AdapterDispatcher.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "data_id.h"
#include "../types.h"

namespace rim
{

Adapter::Adapter(
    AdapterDispatcher& dispatcher)
    : dispatcher_(dispatcher)
{
}

bool Adapter::Initialize()
{
    return true;
}

// 実ハードウェア入力は未接続。全レイヤ初期化済みのlayer_init_stateを
// 一度だけ流す最小限のスタブ実装(test_productのAdapter::Poll()に倣う)。
//
// 注意: RIMValueFactory::CreateBinary()はポインタを保持するだけでコピー
// しない。Dispatch()はStoreInputQueueへ積んで別スレッド(DataStoreWorker)
// が非同期に処理するため、本関数のローカル変数stateが破棄される前に
// Storeレイヤ側でコピーされる保証がこの実装だけでは無い(RIM_SetBinary/
// SetErrorList等、他の実際のkBinary経路も同様の前提に依存している)。
// 実ハードウェア接続時は、Dispatch()呼び出しが完了するまで有効な
// バッファ(静的/ヒープ確保等)を使うこと。
bool Adapter::Poll()
{
    layer_init_state_t state{};

    for (auto& layer : state.state)
    {
        layer = BIL_LAYER_INIT;
    }

    RIMDataItem item{};

    item.id =
        RI_DATA_LAYER_INIT_STATE;

    item.value =
        RIMValueFactory::CreateBinary(
            reinterpret_cast<const std::uint8_t*>(
                &state),
            sizeof(state));

    return dispatcher_.Dispatch(
        item);
}

void Adapter::Shutdown()
{
}

}
