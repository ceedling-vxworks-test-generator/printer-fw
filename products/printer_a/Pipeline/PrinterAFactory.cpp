#include "PrinterAFactory.hpp"

#include "PrinterAProvider.hpp"

namespace rim
{

std::unique_ptr<IProductProvider>
CreatePrinterAProvider()
{
    return std::make_unique<PrinterAProvider>();
}

}