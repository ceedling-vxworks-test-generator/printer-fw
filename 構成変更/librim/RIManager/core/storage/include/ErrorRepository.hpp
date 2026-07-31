#pragma once

#include <algorithm>
#include <vector>

#include "ErrorInfo.hpp"

namespace rim
{

class ErrorRepository
{
public:

    bool Add(
        const ErrorInfo& info)
    {
        const auto it =
            std::find_if(
                errors_.begin(),
                errors_.end(),
                [&](const ErrorInfo& current)
                {
                    return
                        current.errorCode ==
                        info.errorCode;
                });

        if (it != errors_.end())
        {
            return false;
        }

        errors_.push_back(
            info);

        return true;
    }

    bool Remove(
        std::uint32_t errorCode)
    {
        const auto oldSize =
            errors_.size();

        errors_.erase(
            std::remove_if(
                errors_.begin(),
                errors_.end(),
                [&](const ErrorInfo& info)
                {
                    return
                        info.errorCode
                        == errorCode;
                }),
            errors_.end());

        return oldSize !=
               errors_.size();
    }

    bool SetState(
        std::uint32_t errorCode,
        ErrorState state)
    {
        for (auto& error
             : errors_)
        {
            if (error.errorCode ==
                errorCode)
            {
                error.state =
                    state;

                return true;
            }
        }

        return false;
    }

    const std::vector<
        ErrorInfo>&
    GetAll() const
    {
        return errors_;
    }

private:

    std::vector<
        ErrorInfo>
        errors_;
};

}