#include "PrinterAdapter.hpp"

namespace rim
{

PrinterAdapter::PrinterAdapter(
    AdapterDispatcher& d)
    : dispatcher_(d)
{
}

bool PrinterAdapter::Initialize()
{
    return true;
}

bool PrinterAdapter::Poll()
{
    // ハードウェアポーリングは単位等の補足情報を持たないため、
    // context無し(nullptr = 既に正規化済み扱い)で投入する。
    return dispatcher_.Dispatch(
        RI_DATA_TEMPERATURE_SENSOR_A,
        30,
        nullptr);
}

void PrinterAdapter::Shutdown()
{
}

} // namespace rim
