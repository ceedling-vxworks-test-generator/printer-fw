#include "DomainRegistry.hpp"

namespace rim
{

DomainId
DomainRegistry::GetOrCreate(
    const DomainDefinition& definition)
{
    std::lock_guard<std::mutex>
        lock(m_);

    const auto it =
        definitionToId_.find(
            &definition);

    if (it != definitionToId_.end())
    {
        return it->second;
    }

    const auto id =
        static_cast<DomainId>(
            idToName_.size() + 1);

    definitionToId_.emplace(
        &definition,
        id);

    idToName_.push_back(
        definition.name);

    return id;
}

DomainId
DomainRegistry::Find(
    const DomainDefinition& definition) const
{
    std::lock_guard<std::mutex>
        lock(m_);

    const auto it =
        definitionToId_.find(
            &definition);

    if (it == definitionToId_.end())
    {
        return kInvalidDomainId;
    }

    return it->second;
}

std::string
DomainRegistry::GetName(
    DomainId id) const
{
    std::lock_guard<std::mutex>
        lock(m_);

    const auto count =
        static_cast<DomainId>(
            idToName_.size());

    if ((id == kInvalidDomainId)
        || (id > count))
    {
        return {};
    }

    return idToName_[id - 1];
}

std::size_t
DomainRegistry::Size() const
{
    std::lock_guard<std::mutex>
        lock(m_);

    return idToName_.size();
}

} // namespace rim