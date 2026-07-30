#pragma once

//
// CentralInputPort - Adapterが報告する中央入口(仕様 §6.1)。受理点は Post() の1本。
//   - Classify(id) で入力性質を判別し、対応するレーンへ振り分ける(L2の責務)
//   - Value型とId定義の一致を検証する
//   - 問題なければ Queue / Buffer へ入れる(満杯=喪失は kErrPost)
//

#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/ICentralInputPort.hpp"
#include "datastore/InputQueues.hpp"
#include "datastore/CurrentValueBuffer.hpp"
#include "datastore/IInputClassifier.hpp"

namespace rim
{

class CentralInputPort final
    : public ICentralInputPort
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

    RIMResult Post(const RIMDataItem& item) override;

private:

    // 性質別の受理処理(Post から振り分けて呼ぶ)。値型の検証もここで行う。
    RIMResult PostFaultInput(const RIMDataItem& item);
    RIMResult PostOperationReport(const RIMDataItem& item);
    RIMResult PostCurrentValueInput(const RIMDataItem& item);

    FaultInputQueue&        faultQueue_;
    OperationReportQueue&   operationQueue_;
    CurrentValueBuffer&     currentBuffer_;
    const IInputClassifier& classifier_;
};

} // namespace rim
