#include <gtest/gtest.h>

#include "DomainRegistry.hpp"
#include "DomainId.hpp"
#include "DomainDefinition.hpp"

namespace rim
{

namespace
{

inline constexpr DomainDefinition
kEnvironmentDomain
{
    "Environment"
};

inline constexpr DomainDefinition
kUnknownDomain
{
    "Unknown"
};

inline constexpr DomainDefinition
kDomainA
{
    "A"
};

inline constexpr DomainDefinition
kDomainB
{
    "B"
};

} // namespace

TEST(
    DomainRegistryTest,
    GetOrCreateReturnsSameId)
{
    DomainRegistry registry;

    const auto id1 =
        registry.GetOrCreate(
            kEnvironmentDomain);

    const auto id2 =
        registry.GetOrCreate(
            kEnvironmentDomain);

    EXPECT_EQ(
        id1,
        id2);

    EXPECT_EQ(
        registry.Size(),
        1u);
}

TEST(
    DomainRegistryTest,
    FindUnknownReturnsInvalid)
{
    DomainRegistry registry;

    EXPECT_EQ(
        registry.Find(
            kUnknownDomain),
        kInvalidDomainId);
}

TEST(
    DomainRegistryTest,
    GetNameRoundTrip)
{
    DomainRegistry registry;

    const auto id =
        registry.GetOrCreate(
            kEnvironmentDomain);

    EXPECT_EQ(
        registry.GetName(id),
        "Environment");
}

TEST(
    DomainRegistryTest,
    InvalidIdReturnsEmptyName)
{
    DomainRegistry registry;

    EXPECT_TRUE(
        registry.GetName(
            kInvalidDomainId)
            .empty());
}

TEST(
    DomainRegistryTest,
    MultipleDomains)
{
    DomainRegistry registry;

    const auto a =
        registry.GetOrCreate(
            kDomainA);

    const auto b =
        registry.GetOrCreate(
            kDomainB);

    EXPECT_NE(
        a,
        b);

    EXPECT_EQ(
        registry.Size(),
        2u);
}

// TEST(
//     DomainRegistryTest,
//     IdsAreSequential)
// {
//     DomainRegistry registry;
//
//     const auto a =
//         registry.GetOrCreate(
//             kDomainA);
//
//     const auto b =
//         registry.GetOrCreate(
//             kDomainB);
//
//     EXPECT_EQ(
//         a + 1,
//         b);
// }

TEST(
    DomainRegistryTest,
    GeneratedIdsAreUnique)
{
    DomainRegistry registry;

    const auto a =
        registry.GetOrCreate(
            kDomainA);

    const auto b =
        registry.GetOrCreate(
            kDomainB);

    EXPECT_NE(
        a,
        b);
}

} // namespace rim