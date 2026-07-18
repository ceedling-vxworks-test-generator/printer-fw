#include "datastore/MachineProfileFactory.hpp"
#include "datastore/PrinterAProvider.hpp"
namespace rim {
std::unique_ptr<IDataDefinitionProvider>
MachineProfileFactory::Create(MachineProfile) {
  return std::unique_ptr<IDataDefinitionProvider>(new PrinterAProvider());
}
} // namespace rim