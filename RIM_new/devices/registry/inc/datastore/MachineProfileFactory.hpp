#pragma once
#include "datastore/IDataDefinitionProvider.hpp"
#include "datastore/MachineProfile.hpp"
#include <memory>
namespace rim {
class MachineProfileFactory {
public:
  static std::unique_ptr<IDataDefinitionProvider> Create(MachineProfile);
};
} // namespace rim