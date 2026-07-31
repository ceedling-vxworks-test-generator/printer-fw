#pragma once

#include "CapabilityId.hpp"
#include "CapabilityPublisherRegistry.hpp"

namespace rim
{

//
// PublishManager - 変化した Capability の配信器を引いて叩く。
//
// 旧実装は Publish(const std::string& capabilityId) で文字列キーを引いていた。
// CapabilityId(整数)に変えたことで、動的確保と文字列比較が経路から消えている。
//
class PublishManager
{
public:

    explicit PublishManager(
        CapabilityPublisherRegistry& registry)
        : registry_(registry)
    {
    }

    // 配信器が未登録の Capability は黙って無視する
    // (誰も購読していない Capability を登録しない運用を許すため)。
    void Publish(CapabilityId capabilityId);

private:

    CapabilityPublisherRegistry&
        registry_;
};

}
