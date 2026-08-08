#pragma once

#include <any>

namespace rim
{

bool CompareEnvironment(
    const std::any& oldValue,
    const std::any& newValue);

bool ComparePrintReady(
    const std::any& oldValue,
    const std::any& newValue);

bool CompareConsumable(
    const std::any& oldValue,
    const std::any& newValue);

bool CompareJob(
    const std::any& oldValue,
    const std::any& newValue);

}