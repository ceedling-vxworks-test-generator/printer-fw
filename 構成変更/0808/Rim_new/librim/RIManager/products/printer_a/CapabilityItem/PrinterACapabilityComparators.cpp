#include "PrinterACapabilityComparators.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

namespace rim
{

bool CompareEnvironment(
    const std::any& oldValue,
    const std::any& newValue)
{
    const auto& lhs =
        std::any_cast<
            const EnvironmentCapability&>(
                oldValue);

    const auto& rhs =
        std::any_cast<
            const EnvironmentCapability&>(
                newValue);

    return
        lhs.temperature != rhs.temperature
        ||
        lhs.humidity != rhs.humidity;
}

bool ComparePrintReady(
    const std::any& oldValue,
    const std::any& newValue)
{
    const auto& lhs =
        std::any_cast<
            const PrintReadyCapability&>(
                oldValue);

    const auto& rhs =
        std::any_cast<
            const PrintReadyCapability&>(
                newValue);

    return lhs.ready != rhs.ready;
}

// bool CompareConsumable(
//     const std::any& oldValue,
//     const std::any& newValue)
// {
//     const auto& lhs =
//         std::any_cast<
//             const ConsumableCapability&>(
//                 oldValue);

//     const auto& rhs =
//         std::any_cast<
//             const ConsumableCapability&>(
//                 newValue);

//     return
//         lhs.tonerLevel != rhs.tonerLevel
//         ||
//         lhs.stapleLevel != rhs.stapleLevel;
// }

// bool CompareJob(
//     const std::any& oldValue,
//     const std::any& newValue)
// {
//     const auto& lhs =
//         std::any_cast<
//             const JobCapability&>(
//                 oldValue);

//     const auto& rhs =
//         std::any_cast<
//             const JobCapability&>(
//                 newValue);

//     return
//         lhs.jobActive != rhs.jobActive
//         ||
//         lhs.jobId != rhs.jobId;
// }

}