#pragma once

#include <memory>

namespace rim
{

class DataStoreQueue;

enum class QueuePolicy
{
    Buffered,
    Coalescing
};

class QueueFactory
{
public:

    static std::unique_ptr<DataStoreQueue>
    Create(QueuePolicy policy);
};

}