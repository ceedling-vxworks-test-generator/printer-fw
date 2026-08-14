#pragma once

#include "ICapabilityPublisher.hpp"

namespace rim
{

//
// GenericCapabilityPublisher - 配信処理を外から差し込める配信器。
//
// 旧実装は std::function<void()> を保持していたが、捕捉が大きくなると暗黙に
// 動的確保する。組込みで確保の発生時期が読めないのは困るため、
// **関数ポインタ + userData** に変更した(CallbackSubscriber と同じ方針)。
//
class GenericCapabilityPublisher
    : public ICapabilityPublisher
{
public:

    using PublishFn =
        void (*)(void* userData);

    GenericCapabilityPublisher() = default;

    GenericCapabilityPublisher(
        PublishFn publishFn,
        void* userData = nullptr);

    // 後から差し込む(配列で値として持つ場合に使う)。
    void Bind(
        PublishFn publishFn,
        void* userData = nullptr);

    bool IsBound() const;

    void Publish() override;

private:

    PublishFn publishFn_{nullptr};

    void* userData_{nullptr};
};

} // namespace rim
