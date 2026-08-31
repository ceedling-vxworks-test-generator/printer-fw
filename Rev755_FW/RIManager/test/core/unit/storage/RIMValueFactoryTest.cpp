#include <gtest/gtest.h>

#include <cstdint>

#include "RIMValueFactory.hpp"

TEST(
    RIMValueFactoryTest,
    CreateBinary)
{
    std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    auto value =
        rim::RIMValueFactory::CreateBinary(
            data,
            sizeof(data));

    EXPECT_EQ(
        value.type,
        rim::ValueType::kBinary);

    EXPECT_EQ(
        value.value.bytes,
        data);

    EXPECT_EQ(
        value.size,
        sizeof(data));
}