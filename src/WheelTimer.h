// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include "TimerQueueBase.h"
#include <vector>
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

class WheelTimer : public TimerQueueBase
{
public:
    WheelTimer();

    int Schedule(uint32_t time_units, TimerCallback cb) override;

    bool Cancel(int id) override;

    int Update(int64_t now = 0) override;

    int Size() const override
    {
        return size_;
    }

private:
    int tick();
    void addTimerNode(TimerNode* node);
    int execute();
    bool cascade(int bucket, int index);

private:
    int size_ = 0;
    int64_t current_ = 0;
    int64_t jiffies_ = 0;
    HASH_MAP<int, TimerNode*> ref_;       // make O(1) searching
    TimerList near_[TVR_SIZE];
    TimerList buckets_[WHEEL_BUCKETS][TVN_SIZE];
};
