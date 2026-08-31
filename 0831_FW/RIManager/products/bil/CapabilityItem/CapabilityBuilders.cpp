#include "data_id.h"

#include "CapabilityBuilders.hpp"

#include "../types.h"

#include <cstring>

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

// 要求定義書(製品要求定義/ProductRequirement_BIL追記.xlsx, Capabilityシート)の
// GenerationRule欄の記載を、自然文からそのまま実装へ翻訳したもの。
// ルール文だけでは判定の優先順位や境界条件が一意に定まらない箇所があり、
// 各関数のコメントに解釈の根拠を明記した。要件定義者による確認を推奨する。

namespace rim
{

namespace
{

template<typename T>
bool TryGetStruct(
    const RIMSnapshot& snapshot,
    RIDataId id,
    T& out)
{
    const std::uint8_t* bytes{};
    std::size_t size{};

    if (!snapshot.TryGetBinary(
            id,
            bytes,
            size))
    {
        return false;
    }

    if (bytes == nullptr
        || size != sizeof(T))
    {
        return false;
    }

    std::memcpy(
        &out,
        bytes,
        sizeof(T));

    return true;
}

} // namespace

// GenerationRule:
// 「初期化が終わっているか？ 実行中のJobはあるか？
//   エラーor機能制限があるか？ UNINIT,IDLE,BUSY,ERROR,RECOVERY」
//
// 優先順位の解釈(要件定義者に要確認):
//   1. いずれかのレイヤが未初期化 -> UNINIT
//   2. フォールトが1件でもある -> ERROR
//   3. フォールトは無いが機能制限がある -> RECOVERY
//      (機能制限中=縮退運転からの復帰過程とみなす)
//   4. いずれかの要求が実行中(BUSY) -> BUSY
//   5. 上記以外 -> IDLE
RIMValue BuildProductState(
    const RIMSnapshot& snapshot)
{
    layer_init_state_t layerInitState{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_LAYER_INIT_STATE,
            layerInitState))
    {
        for (const auto state : layerInitState.state)
        {
            if (state != BIL_LAYER_INIT)
            {
                return RIMValueFactory::CreateInt32(
                    BIL_PRODUCT_UNINIT);
            }
        }
    }

    FaultInfoList_t faultInfoList{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_FAULT_INFO_LIST,
            faultInfoList)
        && faultInfoList.count > 0)
    {
        return RIMValueFactory::CreateInt32(
            BIL_PRODUCT_ERROR);
    }

    FunctionLimitList_t functionLimitList{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_FUNCTION_LIMIT_LIST,
            functionLimitList)
        && functionLimitList.count > 0)
    {
        return RIMValueFactory::CreateInt32(
            BIL_PRODUCT_RECOVERY);
    }

    pipeline_info_t pipelineInfo{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,
            pipelineInfo))
    {
        for (const auto state : pipelineInfo.state)
        {
            if (state == BIL_PIPELINE_BUSY)
            {
                return RIMValueFactory::CreateInt32(
                    BIL_PRODUCT_BUSY);
            }
        }
    }

    return RIMValueFactory::CreateInt32(
        BIL_PRODUCT_IDLE);
}

// GenerationRule:
// 「サブパウチにNearEmpty以下があるか？ NORMAL/ABNORMAL」
//
// 色のいずれかがEMPTYまたはNEAR_EMPTY(=NearEmpty以下)ならABNORMAL。
RIMValue BuildSupplyStatus(
    const RIMSnapshot& snapshot)
{
    sub_pouch_sensor_value_t sensorValue{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST,
            sensorValue))
    {
        for (const auto value : sensorValue.value)
        {
            if (value == BIL_SUB_POUCH_EMPTY
                || value == BIL_SUB_POUCH_NEAR_EMPTY)
            {
                return RIMValueFactory::CreateInt32(
                    BIL_STATUS_ABNORMAL);
            }
        }
    }

    return RIMValueFactory::CreateInt32(
        BIL_STATUS_NORMAL);
}

// GenerationRule:
// 「印字ジョブがあるか？ NORMAL/ABNORMAL」
//
// ルール文はジョブの有無のみを条件としており、異常判定の詳細
// (エラー中のジョブのみを異常とする等)は記載されていない。
// 本実装は記載を字句通りに解釈し、ジョブが1件でも存在すればABNORMALとする。
// 「ジョブがあること自体を異常とみなす」意図で合っているか、
// 要件定義者に要確認。
RIMValue BuildPrintJobStatus(
    const RIMSnapshot& snapshot)
{
    print_job_info_t printJobInfo{};

    if (TryGetStruct(
            snapshot,
            RI_DATA_PRINT_JOB_INFO_LIST,
            printJobInfo)
        && printJobInfo.jobCount > 0)
    {
        return RIMValueFactory::CreateInt32(
            BIL_STATUS_ABNORMAL);
    }

    return RIMValueFactory::CreateInt32(
        BIL_STATUS_NORMAL);
}

} // namespace rim
