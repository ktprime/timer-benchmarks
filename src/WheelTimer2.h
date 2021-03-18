// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <vector>
#include <list>
#include <unordered_map>

// timer queue implemented with hashed hierarchical wheel.
//
// inspired by linux kernel, see links below
// https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git/tree/kernel/timer.c?h=v3.2.98
//
// We model timers as the number of ticks until the next
// due event. We allow 32-bits of space to track this
// due interval, and break that into 4 regions of 8 bits.
// Each region indexes into a bucket of 256 lists.
//
// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(1)       O(1)          O(1)
//

#ifndef WHEEL_BUCKETS
#define WHEEL_BUCKETS 2
enum TIME_WHEEL
{
    TVN_BITS = 6,                   // time vector level shift bits
    TVR_BITS = 8,                  // timer vector shift bits
    TVN_SIZE = (1 << TVN_BITS),     // wheel slots of level vector
    TVR_SIZE = (1 << TVR_BITS),     // wheel slots of vector
    TVN_MASK = (TVN_SIZE - 1),      //
    TVR_MASK = (TVR_SIZE - 1),      //
    TIME_INDEX1 = 1 << (TVR_BITS + 1 * TVN_BITS),
    TIME_INDEX2 = 1 << (TVR_BITS + 2 * TVN_BITS),
    TIME_INDEX3 = 1 << (TVR_BITS + 3 * TVN_BITS),

    ALL_BITS  = TVR_BITS + WHEEL_BUCKETS * TVN_BITS,
    MAX_TVAL  = (uint64_t)((1ULL << ALL_BITS) - 1),
    MAX_SECOS = (uint32_t)(MAX_TVAL / (1000 / 1)),
    MAX_MINUS = (uint32_t)(MAX_SECOS / 60),
    MAX_HOURS = (uint32_t)(MAX_SECOS / 3600),
};
#endif

#if __cplusplus > 201702
static_assert(MAX_HOURS > 1 && MAX_HOURS < 24);//one hour - one day
static_assert(WHEEL_BUCKETS > 2 && WHEEL_BUCKETS <= 4);//one hour - one day
#endif

class WheelTimer2 : public TimerQueueBase
{
public:
    static constexpr int INVALID_NODE_TIMERID = -1;
    static constexpr int INVALID_NODE_SLOTID  = -2;
    static constexpr int FREE_LIST_CAPACITY   = 1024;
    static constexpr int ALLOC_SIZE           = 1024;

    struct TimerNode
    {
        int64_t id;         // timer id
        int64_t expire;     // jiffies
        //int     slot;       // near index
        TimerCallback cb;
    };

#if 1
    typedef std::vector<TimerNode*> TimerList;
#else
    typedef std::list<TimerNode*> TimerList;
#endif

public:
    WheelTimer2();
    ~WheelTimer2();

    int Schedule(uint32_t time_units, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override
    {
        return ref_.size();
    }

private:
    int tick();
    void addTimerNode(TimerNode* node);
    int execute();
    bool cascade(int bucket);

    void clearAll();
    TimerNode* allocNode();
    void freeNode(TimerNode* node);
    void freeList(TimerList& list);

private:
    int64_t jiffies_ = 0;
    HASH_MAP<int, TimerNode* > ref_;
    int alloc_size_  = 0;
    TimerList free_list_;
    TimerList near_[TVR_SIZE];
    TimerList buckets_[WHEEL_BUCKETS][TVN_SIZE];
    TimerList alloc_list_;
};
