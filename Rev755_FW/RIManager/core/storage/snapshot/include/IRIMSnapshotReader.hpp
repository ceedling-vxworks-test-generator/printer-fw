#pragma once

#include "RIMSnapshot.hpp"

namespace rim
{

class IRIMSnapshotReader
{
public:

    virtual ~IRIMSnapshotReader() = default;

    virtual RIMSnapshot Read() const = 0;
};

} // namespace rim