#include "adapter/PrinterAdapter.hpp"
#include "datastore/DeviceEvent.hpp"
namespace rim {
PrinterAdapter::PrinterAdapter(AdapterDispatcher &d) : dispatcher_(d) {}
bool PrinterAdapter::Initialize() { return true; }
bool PrinterAdapter::Poll() {
  DeviceEvent ev{RIMDataId::kTemperature, 30};
  return dispatcher_.Dispatch(ev);
}
void PrinterAdapter::Shutdown() {}
} // namespace rim
