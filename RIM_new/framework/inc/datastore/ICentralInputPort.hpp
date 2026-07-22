#pragma once

//
// ICentralInputPort - RIM_DatastoreLayer が Adapter へ提供する中央入口IF(仕様 §6.1)。
// Adapter は本抽象にのみ依存する(具象 DataStore に依存しない)。
// これにより Adapter 単体テストではモック実装を差し込める。
//

#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"

namespace rim
{

class ICentralInputPort
{
public:

    virtual ~ICentralInputPort() = default;

    virtual RIMResult PostFaultInput(const RIMDataItem& item) = 0;
    virtual RIMResult PostOperationReport(const RIMDataItem& item) = 0;
    virtual RIMResult PostCurrentValueInput(const RIMDataItem& item) = 0;
};

} // namespace rim
