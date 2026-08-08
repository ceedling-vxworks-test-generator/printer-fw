#pragma once

#include <string_view>
#include <vector>

#include "AccessId.hpp"
#include "RIMSnapshotManager.hpp"
#include "SnapshotStorage.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class SnapshotAccessor
{
public:

    SnapshotAccessor(
        const RIMSnapshotManager& reader,
        SnapshotStorage& storage,
        const ProductDefinition& product)
        :
        reader_(reader),
        storage_(storage),
        product_(product)
    {
    }

    AccessId
    CreateSnapshot();

    AccessId
    CreateSnapshot(
        const std::vector<std::string_view>& domains);

    bool
    TryGetSnapshot(
        AccessId id,
        RIMSnapshot& snapshot) const;

private:

    const RIMSnapshotManager&
        reader_;

    SnapshotStorage&
        storage_;

    const ProductDefinition&
        product_;

    RIMSnapshot
    CreateFilteredSnapshot(
        const std::vector<std::string_view>* domains) const;
};

} // namespace rim