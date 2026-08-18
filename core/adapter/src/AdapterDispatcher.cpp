#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"

#include "RIMDataItem.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    RIDataId id,
    const RIMValue& value)
{
    // id に対応する DataItemDefinition(正規化規則・型定義)を製品定義から検索する。
    const auto* definition =
        FindDataItem(
            product_,
            id);

    if (definition == nullptr)
    {
        // 未登録の RIDataId。製品側にDataItem定義が無いため処理できない。
        return false;
    }

    if (value.type != definition->rawValueType)
    {
        // 呼び出し元(RIM_SetBool/SetInt32/SetDouble等)が渡したValueの型が
        // このDataItemのrawValueTypeと一致しない。ここで弾かないと、
        // item.valueType(=setValueType)と実際のRIMValue::typeが食い違ったまま
        // Storeへ積まれてしまう。
        return false;
    }

    // DataItemDefinition::normalize で、生値(value)をこのDataItem本来の
    // setValueType へ変換する(raw==setな項目は素通しでよい)。
    const RIMValue setValue =
        definition->normalize(
            value,
            nullptr);

    RIMDataItem item{};

    item.id = id;

    // normalize後の型(setValueType)をStore投入時の型として採用する。
    item.valueType =
        definition->setValueType;

    item.value =
        setValue;

    // Storeレイヤへ受け渡す。以降の変更検知・保存・Capability評価・通知は
    // DataStoreWorker/DataStoreDispatcher(core/store)側の責務。
    queue_.Push(
        item);

    return true;
}

} // namespace rim