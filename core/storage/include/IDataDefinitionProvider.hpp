#pragma once
#include "RIMDataDefinition.hpp"
#include "RIMDataId.hpp"

#include <vector>

namespace rim {
class IDataDefinitionProvider {
public:
  virtual ~IDataDefinitionProvider() = default;
  virtual const std::vector<RIMDataDefinition> &GetDefinitions() const = 0;
};
} // namespace rim