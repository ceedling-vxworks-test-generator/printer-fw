#include <gtest/gtest.h>

#include "BinaryEntry.hpp"

namespace rim
{

TEST(
    BinaryEntryTest,
    SetCopiesBuffer)
{
    BinaryEntry entry;

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    entry.Set(
        data,
        sizeof(data));

    ASSERT_EQ(
        entry.buffer.size(),
        sizeof(data));

    EXPECT_EQ(
        entry.buffer[0],
        0x11);

    EXPECT_EQ(
        entry.buffer[1],
        0x22);

    EXPECT_EQ(
        entry.buffer[2],
        0x33);
}

TEST(
    BinaryEntryTest,
    SetCalculatesHash)
{
    BinaryEntry entry;

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    entry.Set(
        data,
        sizeof(data));

    EXPECT_NE(
        entry.hash,
        0ULL);
}

TEST(
    BinaryEntryTest,
    SetUpdatesHashWhenDataChanges)
{
    BinaryEntry entry;

    const std::uint8_t data1[]
    {
        0x11,
        0x22,
        0x33
    };

    entry.Set(
        data1,
        sizeof(data1));

    const auto hash1 =
        entry.hash;

    const std::uint8_t data2[]
    {
        0x11,
        0x22,
        0x44
    };

    entry.Set(
        data2,
        sizeof(data2));

    EXPECT_NE(
        hash1,
        entry.hash);
}

} // namespace rim