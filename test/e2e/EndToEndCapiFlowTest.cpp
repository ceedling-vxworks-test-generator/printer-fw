// C API E2E
#include <gtest/gtest.h>
#include "rim_api.h"

TEST(EndToEndCapiFlowTest, CreateDestroy)
{
    RIM_HANDLE handle = nullptr;

    EXPECT_EQ(RI_SUCCESS, RIManager_Create(&handle));
    EXPECT_NE(nullptr, handle);
    EXPECT_EQ(RI_SUCCESS, RIManager_Destroy(handle));
}
