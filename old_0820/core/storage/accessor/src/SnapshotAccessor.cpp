#include <iostream>

#include "SnapshotAccessor.hpp"

namespace rim
{

RIMSnapshot
SnapshotAccessor::CreateSnapshot(
    const std::vector<DomainId>& domains) const
{
    return snapshotBuilder_.Build(
        domains);
}

} // namespace rim