#include "RIId.hpp"
#include "AdapterDispatcher.hpp"

#include "DataItemDefinition.hpp"

#include "RIMDataItem.hpp"

namespace rim
{

bool AdapterDispatcher::Dispatch(
    const RIMDataItem& item)
{
    const auto* definition =
        context_.FindDataItem(
            {
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    item.id)
            });
            
    if (definition == nullptr)
    {
        return false;
    }

    if (item.value.type != definition->rawValueType)
    {
        // 呼び出し元が渡した値の型がこのDataItemのrawValueTypeと一致しない。
        // 型不一致のまま先へ進めると、setValueTypeと実際のRIMValue::typeが
        // 食い違ったままStoreへ積まれてしまうため、ここで拒否する。
        return false;
    }

    const RIMValue setValue =
        definition->normalize(
            item.value,
            nullptr);

    RIMDataItem outItem{};

    outItem.id = item.id;

    outItem.value.type =
        definition->setValueType;

    outItem.value =
        setValue;

    auto queue = routeProvider_.Find(outItem.id);

    if (queue == nullptr)
    {
        return false;
    }

    queue->storeQueue->Push(outItem);

    return true;
}

} // namespace rim