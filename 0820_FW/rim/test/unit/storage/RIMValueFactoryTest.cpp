#include <gtest/gtest.h>

#include <memory>

#include "BinaryStoreValue.hpp"
#include "RIMValueFactory.hpp"

TEST(
    RIMValueFactoryTest,
    CreateBinary)
{
    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    auto value =
        rim::RIMValueFactory::CreateBinary(
            binary.get());

    EXPECT_EQ(
        value.type,
        rim::ValueType::kBinary);

    EXPECT_EQ(
        value.value.ptr,
        binary.get());
}