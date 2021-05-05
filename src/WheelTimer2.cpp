// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include "WheelTimer2.h"
#include "Clock.h"
#include "Logging.h"
#include <assert.h>

WheelTimer2::WheelTimer2()
    : jiffies_(Clock::CurrentTimeUnits())
{
    ref_.reserve(64);            // reserve a little space
}

#define TIMER_SLOT(j, N) ((j >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

void WheelTimer2::addTimerNode(TimerNode* node)
{
    //node->slot   = INVALID_NODE_SLOTID;
    auto expires = node->expire;
    const uint64_t idx = (uint64_t)(expires - jiffies_);
    TimerList* list;
    if (idx < TVR_SIZE) // [0, 0x100)
    {
        int i = expires & TVR_MASK;
        list = &near_[i];
    }
    else if (idx < TIME_INDEX1) // [0x100, 0x4000)
    {
        int i = TIMER_SLOT(expires, 0);
        list = &buckets_[0][i];
    }
#if WHEEL_BUCKETS >= 2
    else if (idx < TIME_INDEX2) // [0x4000, 0x100000)
    {
        int i = TIMER_SLOT(expires, 1);
        list = &buckets_[1][i];
    }
#endif
#if WHEEL_BUCKETS >= 3
    else if (idx < TIME_INDEX3) // [0x100000, 0x4000000)
    {
        int i = TIMER_SLOT(expires, 2);
        list = &buckets_[2][i];
    }
#endif
    else if ((int64_t)idx < 0)
    {
        // Can happen if you add a timer with expires == jiffies,
        // or you set a timer to go off in the past
        node->expire = jiffies_;
        int i = jiffies_ & TVR_MASK;
        list = &near_[i];
    }
    else
    {
        // If the timeout is larger than MAX_TVAL on 64-bit
        // architectures then we use the maximum timeout
        if (idx > MAX_TVAL)
        {
             node->expire = expires = MAX_TVAL + jiffies_;
        }
        constexpr int slots = WHEEL_BUCKETS - 1;
        int i = TIMER_SLOT(expires, slots);
        list = &buckets_[slots][i];
    }
    // add to linked list
    list->emplace_back(node);
}

int WheelTimer2::Schedule(uint32_t time_units, TimerCallback cb)
{
    auto node    = allocNode();
    node->cb     = cb;
    node->expire = jiffies_ + time_units;
    if (node->id <= 0)
    node->id     = nextCounter();
    addTimerNode(node);
    ref_.insert_unique(node->id, node);
    return node->id;
}

// Do lazy cancellation, so we can effectively use vector as container of timer nodes
bool WheelTimer2::Cancel(int timer_id)
{
    const auto itnode = ref_.find(timer_id);
    if (itnode == ref_.end())
        return false;

    auto node = itnode->second;
    ref_.erase(itnode);
    node->expire = 0;
    node->id = INVALID_NODE_TIMERID;

    return true;
}


// cascade all the timers at bucket of index up one level
bool WheelTimer2::cascade(int bucket)
{
    if (bucket >= WHEEL_BUCKETS)
        return false;

    const int index = TIMER_SLOT(jiffies_, bucket);
    TimerList list = std::move(buckets_[bucket][index]);
    for (auto node : list)
    {
        if (node->id == INVALID_NODE_TIMERID)
            freeNode(node);
        else
            addTimerNode(node);
    }
    return index == 0;
}

int WheelTimer2::execute()
{
    const auto index = jiffies_ & TVR_MASK;
    auto& near  = near_[index];
    if (0 == near.size())
        return 0;

    int fired = 0;
    TimerList expired = std::move(near);

    for (auto node : expired)
    {
        if (1 == ref_.erase(node->id)) {//erase it then call Run, it can be removed by Cancel in Run Callback
            node->cb(); fired ++;
        }
        freeNode(node);
    }

    return fired;
}

// cascades all vectors and executes all expired timer
int WheelTimer2::tick()
{
    const auto index = jiffies_ & TVR_MASK;
    if (index == 0 && cascade(0) && cascade(1)) // cascade timers
    {
#if WHEEL_BUCKETS > 2
        if (cascade(2) && cascade(3));
#endif
    }

    int fired = execute();
    jiffies_++;
    return fired;
}

int WheelTimer2::Update(int64_t now)
{
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }

    const auto ticks = (int)(now - jiffies_);
    if (ticks > 0)
    {
        int fired = 0;
        for (int i = 0; i < ticks; i++)
            fired += tick();
        return fired;
    }
    else if (ticks < 0)
    {
        assert(false && "time go backwards");
        return -1;
    }
    return 0;
}

