#include "PrinterAErrorRegistry.hpp"

namespace
{

struct ErrorMap
{
    std::uint32_t code;

    rim::ErrorSeverity severity;
};

constexpr ErrorMap kErrors[]
{
    {
        1001,
        rim::ErrorSeverity::kWarning
    },

    {
        1002,
        rim::ErrorSeverity::kError
    },

    {
        1003,
        rim::ErrorSeverity::kFatal
    }
};

}

namespace rim
{

bool PrinterAErrorRegistry::
TryGetSeverity(
    std::uint32_t errorCode,
    ErrorSeverity& severity) const
{
    for (const auto& error
         : kErrors)
    {
        if (error.code ==
            errorCode)
        {
            severity =
                error.severity;

            return true;
        }
    }

    return false;
}

} // namespace rim