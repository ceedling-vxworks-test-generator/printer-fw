#include "CapabilityWorker.hpp"

#include "CapabilityInput.hpp"
#include "NotificationTargetType.hpp"

namespace rim
{

    CapabilityWorker::CapabilityWorker(
        CapabilityInputQueue& queue,
        CapabilityManager& manager,
        PublisherInputQueue& publisherQueue)
        : queue_(queue)
        , manager_(manager)
        , publisherQueue_(publisherQueue)
    {
    }

    CapabilityWorker::~CapabilityWorker()
    {
        Stop();
    }

void CapabilityWorker::Process(
    const CapabilityInput& capabilityInput)
{
    if (capabilityInput.changedDataId
        == RI_DATA_UNKNOWN)
    {
        return;
    }

    PublisherInput dataInput{};

    dataInput.target.type =
        NotificationTargetType::Data;

    dataInput.target.id =
        static_cast<std::uint32_t>(
            capabilityInput.changedDataId);

    publisherQueue_.Push(
        dataInput);

    const auto changes =
        manager_.Evaluate(
            capabilityInput.snapshot,
            capabilityInput.changedDataId);

    PublishCapabilityChanges(
        changes);
}

void CapabilityWorker::Run()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            [this]
            {
                while (running_)
                {
                    CapabilityInput capabilityInput{};

                    if (!queue_.WaitAndPop(
                            capabilityInput))
                    {
                        break;
                    }

                    Process(
                        capabilityInput);
                }
            });
}

void CapabilityWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool CapabilityWorker::ExecuteOnce()
{
    CapabilityInput capabilityInput{};

    if (!queue_.TryPop(
            capabilityInput))
    {
        return false;
    }

    Process(
        capabilityInput);

    return true;
}

void CapabilityWorker::PublishCapabilityChanges(
    const CapabilityChangeSet& changes)
{
    if (changes.Empty())
    {
        return;
    }

    for (const auto capabilityId
            : changes.changedCapabilities)
    {
        PublisherInput input{};

        input.target.type =
            NotificationTargetType::Capability;

        input.target.id =
            static_cast<std::uint32_t>(
                capabilityId);

        publisherQueue_.Push(
            input);
    }
}

} // namespace rim