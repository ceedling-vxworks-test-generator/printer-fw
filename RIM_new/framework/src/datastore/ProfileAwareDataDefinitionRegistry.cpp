#include "datastore/ProfileAwareDataDefinitionRegistry.hpp"
namespace rim {
const RIMDataDefinition *
ProfileAwareDataDefinitionRegistry::Find(RIMDataId id) const {
  for (const auto &e : p_.GetDefinitions()) {
    if (e.id == id)
      return &e;
  }
  return nullptr;
}
} // namespace rim