#pragma once
#include <list>

#include "CompressionPolicy.hpp"

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
        void Insert(std::list<T>& events,const T& event)
        {
            events.push_back(event);
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

                    // TODO: ログ追加候補
                    // Coalescingにより既存イベントが置換された場合は
                    // 圧縮発生回数を記録すると効果測定に利用できる。

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
        void Insert(std::list<T>& events,const T& event)
        {
            auto it =events.begin();

            for (; it != events.end(); ++it)
            {
                if (event.priority >it->priority)
                {
                    break;
                }
            }

            events.insert(it,event);

            // TODO: ログ追加候補
            // 高優先度イベント投入時の件数を記録すると
            // 負荷解析に利用できる。
            
        }
    };

    struct PriorityCompressionPolicy
    {
        template<typename T>
        void Insert(std::list<T>& events,const T& event)
        {
            const auto compressionPolicy = event.compressionPolicy;

            if (compressionPolicy ==CompressionPolicy::KeepOldest)
            {
                for (const auto& existing: events)
                {
                    if (existing.target.type == event.target.type &&
                        existing.target.id ==event.target.id)
                    {

                        // TODO: ログ追加候補
                        // KeepOldestによりイベントを破棄した場合は
                        // 圧縮発生回数を記録すると通知削減効果の
                        // 確認に利用できる。

                        return;
                    }
                }
            }

            if (compressionPolicy == CompressionPolicy::KeepLatest)
            {
                for (auto it = events.begin();it != events.end();++it)
                {
                    if (it->target.type ==event.target.type &&
                        it->target.id ==event.target.id)
                    {
                        events.erase(it);

                        // TODO: ログ追加候補
                        // KeepLatestにより旧イベントを置換した場合は
                        // 圧縮発生回数を記録すると圧縮効果確認に利用できる。

                        break;
                    }
                }
            }

            auto insertPos =events.begin();

            for (; insertPos != events.end();++insertPos)
            {
                if (event.priority > insertPos->priority)
                {
                    break;
                }
            }

            events.insert(insertPos,event);
        }
    };

}