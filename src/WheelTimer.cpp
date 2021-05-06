// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include "WheelTimer.h"
#include "Clock.h"
#include "Logging.h"
#include <assert.h>



WheelTimer::WheelTimer()
    : current_(Clock::CurrentTimeUnits())
{
    ref_.reserve(64);            // reserve a little space
}

void WheelTimer::addTimerNode(TimerNode* node)
{
    int64_t expires = node->expire;
    uint64_t idx = (uint64_t)(expires - jiffies_);
    WheelList* list = nullptr;
    if (idx < TVR_SIZE) // [0, 0x100)
    {
        int i = expires & TVR_MASK;
        list = &near_[i];
    }
    else if (idx < (1 << (TVR_BITS + TVN_BITS))) // [0x100, 0x4000)
    {
        int i = (expires >> TVR_BITS) & TVN_MASK;
        list = &buckets_[0][i];
    }
    else if (idx < (1 << (TVR_BITS + 2 * TVN_BITS))) // [0x4000, 0x100000)
    {
        int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
        list = &buckets_[1][i];
    }
    else if (idx < (1 << (TVR_BITS + 3 * TVN_BITS))) // [0x100000, 0x4000000)
    {
        int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[2][i];
    }
    else if ((int64_t)idx < 0)
    {
        // Can happen if you add a timer with expires == jiffies,
        // or you set a timer to go off in the past
        int i = jiffies_ & TVR_MASK;
        list = &near_[i];
    }
    else
    {
        // If the timeout is larger than MAX_TVAL on 64-bit
        // architectures then we use the maximum timeout
        if (idx > MAX_TVAL)
        {
            idx = MAX_TVAL;
            expires = idx + jiffies_;
        }
        int i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
        list = &buckets_[3][i];
    }
    // add to linked list
    list->push_back(node);
}

int WheelTimer::Schedule(uint32_t time_units, TimerCallback cb)
{
    TimerNode* node = allocNode();
    node->slot = size_;
    node->cb = cb;
    node->expire = jiffies_ + time_units;
    node->id = nextCounter();
    addTimerNode(node);
    ref_.insert_unique(node->id, node);
    size_++;
    return node->id;
}

// Do lazy cancellation, so we can effectively use vector as container of timer nodes
bool WheelTimer::Cancel(int id)
{
    TimerNode* node = ref_[id];
    if (node != nullptr)
    {
        node->slot = -1;
        size_--;
        return true;
    }
    return false;
}

// cascade all the timers at bucket of index up one level
bool WheelTimer::cascade(int bucket, int index)
{
    // swap list
    if (bucket >= WHEEL_BUCKETS)
        return false;

    WheelList list = std::move(buckets_[bucket][index]);

    for (auto node : list)
    {
        if (node->id > 0)
        {
            addTimerNode(node);
        }
    }
    return index == 0;
}

#define INDEX(N) ((jiffies_ >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

// cascades all vectors and executes all expired timer
int WheelTimer::tick()
{
    int fired = execute();
    int index = jiffies_ & TVR_MASK;
    if (index == 0) // cascade timers
    {
        if (cascade(0, INDEX(0)) &&
            cascade(1, INDEX(1)) &&
            cascade(2, INDEX(2)))
            cascade(3, INDEX(3));
    }
#undef INDEX

    jiffies_++;
    fired += execute();
    return fired;
}

int WheelTimer::execute()
{
    int index = jiffies_ & TVR_MASK;
    if (near_[index].size() == 0)
        return 0;

    int fired = 0;
    run_info_= std::move(near_[index]); // swap list
    for (const auto node : run_info_)
    {
        if (node->slot >= 0)
        {
            node->cb();
            size_--;
            fired++;
        }

        ref_.erase(node->id);
        freeNode(node);
    }

    run_info_.clear();
    return fired;
}

int WheelTimer::Update(int64_t now)
{
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }
    if (now < current_)
    {
        assert(false && "time go backwards");
        current_ = now;
        return -1;
    }
    else if (now > current_)
    {
        int ticks = (int)(now - current_);
        current_ = now;
        int fired = 0;
        for (int i = 0; i < ticks; i++)
        {
            fired += tick();
        }
        return fired;
    }
    return 0;
}
