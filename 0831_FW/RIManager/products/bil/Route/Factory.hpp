#pragma once

#include <memory>

#include "IProductProvider.hpp"

namespace rim
{

std::unique_ptr<IProductProvider>
CreateProvider();

}
