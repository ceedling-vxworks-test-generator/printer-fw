#pragma once

namespace rim
{

struct RIMSnapshot
{
    double temperature{};

    double humidity{};

    bool upperDoorOpen{};

    bool rightDoorOpen{};

    bool leftDoorOpen{};
};

} // namespace rim