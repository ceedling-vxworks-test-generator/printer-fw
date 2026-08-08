#pragma once

#include <chrono>
#include <thread>

template<class Predicate>
bool WaitUntil(
    Predicate predicate,
    std::chrono::milliseconds timeout)
{
    const auto deadline =
        std::chrono::steady_clock::now()
        + timeout;

    while (std::chrono::steady_clock::now()
           < deadline)
    {
        if (predicate())
        {
            return true;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }

    return predicate();
}