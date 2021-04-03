// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <set>


// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class TreeTimer : public TimerQueueBase
{
public:
    struct TimerNode
    {
        int id = -1;
        int64_t expires = 0;
        TimerCallback cb;

        bool operator < (const TimerNode& b) const
        {
            return expires < b.expires;
        }
    };

public:
    TreeTimer();
    ~TreeTimer();

    int Schedule(uint32_t time_units, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override
    {
        return (int)tree_.size();
    }

private:
    void clear();

private:
    std::multiset<TimerNode> tree_;  // std::multiset as a red-black tree
    HASH_MAP<int, std::multiset<TimerNode>::iterator> ref_; 
};

