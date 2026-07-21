#pragma once

//
// IInputClassifier - Id が意味する入力性質(InputKind)を返す抽象IF。
// CentralInputPort の post種別/Id分類の一致検証、Adapter の3種post選択に用いる。
// 機種可変(どのIdがFault/Operation/CurrentValueか)なので devices が実装する。
//

#include <optional>

#include "datastore/RIMDataId.hpp"
#include "datastore/InputKind.hpp"

namespace rim
{

class IInputClassifier
{
public:

    virtual ~IInputClassifier() = default;

    virtual std::optional<InputKind> Classify(RIMDataId id) const = 0;
};

} // namespace rim
