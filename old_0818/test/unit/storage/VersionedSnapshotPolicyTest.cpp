#include <gtest/gtest.h>

#include "VersionedSnapshotPolicy.hpp"
#include "SnapshotVersion.hpp"

namespace rim
{

TEST(
    VersionedSnapshotPolicyTest,
    SameVersionIsConsistent)
{
    VersionedSnapshotPolicy policy;

    EXPECT_TRUE(
        policy.IsConsistent(
            SnapshotVersion{1},
            SnapshotVersion{1}));
}

TEST(
    VersionedSnapshotPolicyTest,
    DifferentVersionIsNotConsistent)
{
    VersionedSnapshotPolicy policy;

    EXPECT_FALSE(
        policy.IsConsistent(
            SnapshotVersion{1},
            SnapshotVersion{2}));
}

} // namespace rim