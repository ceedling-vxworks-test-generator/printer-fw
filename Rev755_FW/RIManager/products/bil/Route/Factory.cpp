#include "Factory.hpp"

#include "Provider.hpp"

namespace rim
{

std::unique_ptr<IProductProvider>
CreateProvider()
{
    return std::make_unique<Provider>();
}

}
