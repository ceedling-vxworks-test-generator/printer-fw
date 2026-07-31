#include <gtest/gtest.h>

#include "CapabilityPublisherRegistry.hpp"

namespace
{

class TestPublisher
    : public rim::ICapabilityPublisher
{
public:

    void Publish() override
    {
    }
};

}

TEST(
    CapabilityPublisherRegistryTest,
    RegisterAndFind)
{
    rim::CapabilityPublisherRegistry
        registry;

    registry.Register(
        "Test",
        std::make_unique<
            TestPublisher>());

    EXPECT_NE(
        nullptr,
        registry.Find(
            "Test"));
}

TEST(
    CapabilityPublisherRegistryTest,
    ReturnNullWhenNotFound)
{
    rim::CapabilityPublisherRegistry
        registry;

    EXPECT_EQ(
        nullptr,
        registry.Find(
            "Unknown"));
}