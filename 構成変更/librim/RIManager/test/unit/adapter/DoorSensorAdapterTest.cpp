#include <gtest/gtest.h>

#include "DoorSensorAdapter.hpp"

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

TEST(
    DoorSensorAdapterTest,
    StoreUpperDoor)
{
    rim::ValueStore store;

    rim::DoorSensorAdapter adapter(
        rim::RIMDataId::kUpperDoorOpen);

    adapter.Store(
        rim::RIMValueFactory::CreateBool(
            true),
        store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kUpperDoorOpen,
            item));

    bool opened{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            item.value,
            opened));

    EXPECT_TRUE(
        opened);
}