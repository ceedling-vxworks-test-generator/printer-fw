#include <gtest/gtest.h>

#include "adapter/DoorSensorAdapter.hpp"

#include "datastore/RIMValueAccessor.hpp"
#include "datastore/RIMValueFactory.hpp"

TEST(
    DoorSensorAdapterTest,
    StoreUpperDoor)
{
    rim::ValueStore store;

    rim::DoorSensorAdapter adapter(
        rim::RIMDataId::kDoorUpper);

    adapter.Store(
        rim::RIMValueFactory::CreateBool(
            true),
        store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kDoorUpper,
            item));

    bool opened{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            item.value,
            opened));

    EXPECT_TRUE(
        opened);
}