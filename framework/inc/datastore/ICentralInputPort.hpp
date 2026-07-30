#pragma once

//
// ICentralInputPort - RIM_DatastoreLayer が Adapter へ提供する中央入口IF(仕様 §6.1)。
// Adapter は本抽象にのみ依存する(具象 DataStore に依存しない)。
// これにより Adapter 単体テストではモック実装を差し込める。
//
// 受理点は Post() の1本のみ。入力性質(Fault/OperationReport/CurrentValue)の判別と
// レーン振り分けは L2(DataStore)の責務であり、Adapter は分類を行わない
// (Adapter は正規化して渡すだけ。IInputClassifier は L2 側が保持する)。
//

#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"

namespace rim
{

class ICentralInputPort
{
public:

    virtual ~ICentralInputPort() = default;

    // id の分類に応じて適切なレーンへ振り分ける(振り分けは実装側=L2の責務)。
    virtual RIMResult Post(const RIMDataItem& item) = 0;
};

} // namespace rim
