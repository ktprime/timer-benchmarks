// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <vector>


// timer queue implemented with red-black tree.
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class HashTimer : public TimerQueueBase
{
public:
    static constexpr int FREE_LIST_CAPACITY = 10240;
    static constexpr int ALLOC_SIZE           = 1024;
    struct TimerNode
    {
        int id;
        int64_t expires;
        TimerCallback cb;
    };

    typedef std::vector<TimerNode*> TimerList;
public:
    HashTimer();
    ~HashTimer();

    int Schedule(uint32_t time_units, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override
    {
        return (int)hash_.size();
    }

private:
    void clear();
    TimerNode* allocNode();
    void freeNode(TimerNode*);

private:
    HASH_MAP<int, TimerNode*> hash_;  // std::multiset as a red-black tree
    TimerList free_list_;
    int alloc_size_  = 0;
    TimerList alloc_list_;
};
