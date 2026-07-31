#pragma once

#include <cstdint>

#include "CapabilityPayload.hpp"
#include "ICapabilityRule.hpp"
#include "PrinterACapabilityIds.hpp"
#include "PrinterACapabilityTypes.hpp"

#include "ErrorRepository.hpp"
#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

//
// PrinterA の Capability 生成規則。
//
// 旧実装は core/capability/include/ に置かれ、Core の CapabilityManager が
// 5種の具象 Rule をメンバとして直接持っていた(= Core が製品の Capability 一覧を
// 知っている状態)。本実装は ICapabilityRule を実装して Product 側に置くため、
// Core は IF 越しにしか呼ばず、Capability の増減で Core は無改修になる。
//
// ErrorRepository のように Snapshot 以外の入力が要る規則は、**Product が構築時に
// 渡す**(Core の ICapabilityRule::Evaluate は Snapshot しか渡さない)。
//

class EnvironmentCapabilityRule final : public ICapabilityRule
{
public:
    CapabilityId Id() const override { return kCapEnvironment; }

    bool Evaluate(const RIMSnapshot& snapshot, CapabilityPayload& out) const override
    {
        EnvironmentCapability cap{};
        snapshot.TryGetDouble(RIMDataId::kTemperatureSensorA, cap.temperature);
        snapshot.TryGetDouble(RIMDataId::kHumiditySensor,     cap.humidity);
        out = CapabilityPayload::From(cap);
        return true;
    }
};

class ConsumableCapabilityRule final : public ICapabilityRule
{
public:
    CapabilityId Id() const override { return kCapConsumable; }

    bool Evaluate(const RIMSnapshot& snapshot, CapabilityPayload& out) const override
    {
        ConsumableCapability cap{};
        snapshot.TryGetInt32(RIMDataId::kStapleLevel, cap.stapleLevel);
        snapshot.TryGetInt32(RIMDataId::kTonerLevel,  cap.tonerLevel);
        out = CapabilityPayload::From(cap);
        return true;
    }
};

class JobCapabilityRule final : public ICapabilityRule
{
public:
    CapabilityId Id() const override { return kCapJob; }

    bool Evaluate(const RIMSnapshot& snapshot, CapabilityPayload& out) const override
    {
        JobCapability cap{};
        snapshot.TryGetBool(RIMDataId::kJobActive, cap.jobActive);
        snapshot.TryGetInt32(RIMDataId::kJobId,    cap.jobId);
        out = CapabilityPayload::From(cap);
        return true;
    }
};

// 異常一覧を Capability 化する。Snapshot ではなく ErrorRepository が入力源。
class ErrorCapabilityRule final : public ICapabilityRule
{
public:
    explicit ErrorCapabilityRule(const ErrorRepository& repository)
        : repository_(repository)
    {
    }

    CapabilityId Id() const override { return kCapError; }

    bool Evaluate(const RIMSnapshot&, CapabilityPayload& out) const override
    {
        ErrorCapability cap{};
        for (const ErrorInfo& info : repository_.GetAll()) {
            if (!cap.Add(info)) break;   // 上限超過。cap.overflow で検知できる
        }
        out = CapabilityPayload::From(cap);
        return true;
    }

private:
    const ErrorRepository& repository_;
};

// 印字可否。扉が全て閉じており、かつ異常が無いこと。
class PrintReadyCapabilityRule final : public ICapabilityRule
{
public:
    explicit PrintReadyCapabilityRule(const ErrorRepository& repository)
        : repository_(repository)
    {
    }

    CapabilityId Id() const override { return kCapPrintReady; }

    bool Evaluate(const RIMSnapshot& snapshot, CapabilityPayload& out) const override
    {
        bool upperDoorOpen{};
        bool rightDoorOpen{};
        bool leftDoorOpen{};

        snapshot.TryGetBool(RIMDataId::kUpperDoorOpen, upperDoorOpen);
        snapshot.TryGetBool(RIMDataId::kRightDoorOpen, rightDoorOpen);
        snapshot.TryGetBool(RIMDataId::kLeftDoorOpen,  leftDoorOpen);

        const bool doorsClosed = !upperDoorOpen && !rightDoorOpen && !leftDoorOpen;
        const bool noError     = repository_.GetAll().empty();

        PrintReadyCapability cap{};
        cap.ready = doorsClosed && noError;
        out = CapabilityPayload::From(cap);
        return true;
    }

private:
    const ErrorRepository& repository_;
};

} // namespace rim
