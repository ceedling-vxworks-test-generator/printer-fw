#pragma once
#include <list>

#include "CompressionPolicy.hpp"
#include "CompressionPolicyResolver.hpp"

namespace rim
{

    enum class QueuePolicy
    {
        Buffered,
        Coalescing,
        // Priority,
        // PriorityCompression,
    };

    struct FifoPolicy
    {
        template<typename T>
        void Insert(
            std::list<T>& events,
            const T& event)
        {
            events.push_back(
                event);
        }
    };

    struct CoalescingPolicy
    {
        template<typename T>
        void Insert(std::list<T>& events, const T& event)
        {
            for (auto it = events.begin(); it != events.end(); ++it)
            {
                if (it->CompressionKey() == event.CompressionKey())
                {
                    events.erase(it);
                    break;
                }
            }

            events.push_back(event);
        }
    };

    struct PriorityPolicy
    {
        template<typename T>
        void Insert(
            std::list<T>& events,
            const T& event)
        {
            auto it =
                events.begin();

            for (; it != events.end(); ++it)
            {
                if (event.priority >
                    it->priority)
                {
                    break;
                }
            }

            events.insert(
                it,
                event);
        }
    };

    struct PriorityCompressionPolicy
    {
        template<typename T>
        void Insert(
            std::list<T>& events,
            const T& event)
        {
            const auto compressionPolicy =
                CompressionPolicyResolver::
                    Resolve(
                        event.target);

            //
            // Job‚È‚Çˆ³k‚È‚µ’Ê’m
            //
            if (compressionPolicy ==
                CompressionPolicy::None)
            {
                events.push_back(
                    event);

                return;
            }

            //
            // PrintReady
            //
            if (compressionPolicy ==
                CompressionPolicy::KeepOldest)
            {
                for (const auto& existing
                    : events)
                {
                    if (existing.target.type ==
                            event.target.type &&
                        existing.target.id ==
                            event.target.id)
                    {
                        return;
                    }
                }
            }

            //
            // Environment / Consumable
            //
            if (compressionPolicy ==
                CompressionPolicy::KeepLatest)
            {
                for (auto it =
                        events.begin();
                    it != events.end();
                    ++it)
                {
                    if (it->target.type ==
                            event.target.type &&
                        it->target.id ==
                            event.target.id)
                    {
                        events.erase(it);
                        break;
                    }
                }
            }

            auto insertPos =
                events.begin();

            for (; insertPos != events.end();
                ++insertPos)
            {
                if (event.priority >
                    insertPos->priority)
                {
                    break;
                }
            }

            events.insert(
                insertPos,
                event);
        }
    };

}