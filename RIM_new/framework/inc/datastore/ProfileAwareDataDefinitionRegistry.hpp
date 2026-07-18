#pragma once
#include "IDataDefinitionProvider.hpp"
namespace rim {
class ProfileAwareDataDefinitionRegistry {
  const IDataDefinitionProvider &p_;

public:
  explicit ProfileAwareDataDefinitionRegistry(const IDataDefinitionProvider &p)
      : p_(p) {}
  const RIMDataDefinition *Find(RIMDataId id) const;
};
} // namespace rim