#include <gtest/gtest.h>

#include "CapabilityPublisherRegistry.hpp"

//
// 旧実装は "Test" のような **文字列キー** で配信器を引いていた。
// CapabilityId(整数)を配列添字にしたので、動的確保も文字列比較も無い。
//

namespace
{

class TestPublisher
    : public rim::ICapabilityPublisher
{
public:

    void Publish() override
    {
        ++calls;
    }

    int calls{0};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

}

TEST(
    CapabilityPublisherRegistryTest,
    RegisterAndFind)
{
    rim::CapabilityPublisherRegistry
        registry;

    TestPublisher publisher;

    ASSERT_TRUE(
        registry.Register(
            kSlotA,
            &publisher));

    EXPECT_EQ(
        registry.Find(
            kSlotA),
        &publisher);
}

TEST(
    CapabilityPublisherRegistryTest,
    ReturnNullWhenNotRegistered)
{
    rim::CapabilityPublisherRegistry
        registry;

    EXPECT_EQ(
        registry.Find(
            kSlotB),
        nullptr);
}

TEST(
    CapabilityPublisherRegistryTest,
    RegisterOverwrites)
{
    rim::CapabilityPublisherRegistry
        registry;

    TestPublisher first;
    TestPublisher second;

    registry.Register(
        kSlotA,
        &first);

    registry.Register(
        kSlotA,
        &second);

    EXPECT_EQ(
        registry.Find(
            kSlotA),
        &second);
}

TEST(
    CapabilityPublisherRegistryTest,
    Unregister)
{
    rim::CapabilityPublisherRegistry
        registry;

    TestPublisher publisher;

    registry.Register(
        kSlotA,
        &publisher);

    registry.Unregister(
        kSlotA);

    EXPECT_EQ(
        registry.Find(
            kSlotA),
        nullptr);
}

TEST(
    CapabilityPublisherRegistryTest,
    OutOfRangeIdIsRejected)
{
    rim::CapabilityPublisherRegistry
        registry;

    TestPublisher publisher;

    const rim::CapabilityId outOfRange =
        static_cast<rim::CapabilityId>(
            rim::kCapabilityMaxCount);

    EXPECT_FALSE(
        registry.Register(
            outOfRange,
            &publisher));

    EXPECT_EQ(
        registry.Find(
            outOfRange),
        nullptr);
}

TEST(
    CapabilityPublisherRegistryTest,
    CountsRegisteredPublishers)
{
    rim::CapabilityPublisherRegistry
        registry;

    TestPublisher a;
    TestPublisher b;

    EXPECT_EQ(
        registry.Count(),
        0U);

    registry.Register(kSlotA, &a);
    registry.Register(kSlotB, &b);

    EXPECT_EQ(
        registry.Count(),
        2U);
}
