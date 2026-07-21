#pragma once

//
// CentralInputPort - Adapterが報告する中央入口(仕様 §6.1)。3種postを提供する。
//   - post種別とId分類の一致を検証する
//   - Value型とId定義の一致を検証する
//   - 問題なければ Queue / Buffer へ入れる(満杯=喪失は kErrPost)
//

#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/InputQueues.hpp"
#include "datastore/CurrentValueBuffer.hpp"
#include "datastore/IInputClassifier.hpp"

namespace rim
{

class CentralInputPort
{
public:

    CentralInputPort(
        FaultInputQueue& faultQueue,
        OperationReportQueue& operationQueue,
        CurrentValueBuffer& currentBuffer,
        const IInputClassifier& classifier)
        : faultQueue_(faultQueue)
        , operationQueue_(operationQueue)
        , currentBuffer_(currentBuffer)
        , classifier_(classifier)
    {
    }

    RIMResult PostFaultInput(const RIMDataItem& item);
    RIMResult PostOperationReport(const RIMDataItem& item);
    RIMResult PostCurrentValueInput(const RIMDataItem& item);

private:

    FaultInputQueue&        faultQueue_;
    OperationReportQueue&   operationQueue_;
    CurrentValueBuffer&     currentBuffer_;
    const IInputClassifier& classifier_;
};

} // namespace rim
