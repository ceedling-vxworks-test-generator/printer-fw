// C API E2E
#include <gtest/gtest.h>
#include "rim_api.h"

TEST(EndToEndCapiFlowTest, CreateDestroy)
{
    EXPECT_EQ(RI_SUCCESS, RIManager_Create());
    EXPECT_EQ(RI_SUCCESS, RIManager_Destroy());
}
