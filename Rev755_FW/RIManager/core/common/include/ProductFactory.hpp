#pragma once

#include <memory>

namespace rim
{

class IProductProvider;
std::unique_ptr<IProductProvider> CreateProvider();

}