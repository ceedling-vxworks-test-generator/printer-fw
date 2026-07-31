#include "ProfileAwareDataDefinitionRegistry.hpp"
namespace rim {
const RIMDataDefinition *
ProfileAwareDataDefinitionRegistry::Find(RIMDataId id) const {
  for (const auto &e : p_.GetDefinitions()) {
    if (e.id == id)
      return &e;
  }
  return nullptr;
}

const RIMDataDefinition*
ProfileAwareDataDefinitionRegistry::FindByName(
    std::string_view name) const
{
    for (const auto& definition : p_.GetDefinitions())
    {
        if (definition.name == name)
        {
            return &definition;
        }
    }

    return nullptr;
}

} // namespace rim