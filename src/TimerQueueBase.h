// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#pragma once

#include <stdint.h>
#include <functional>
#include <unordered_map>


#if 0
    #include "robin_hood.h"
    #define HASH_MAP robin_hood::unordered_map
#elif 1
    //#define EMH_FIBONACCI_HASH 1
    #include "hash_table5.hpp"
    #define HASH_MAP emhash5::HashMap
#else
   #define HASH_MAP std::unordered_map
#endif

// callback on timed-out
typedef std::function<void()> TimerCallback;

// timer queue scheduling
class TimerQueueBase
{
public:
    TimerQueueBase();
    virtual ~TimerQueueBase();

    TimerQueueBase(const TimerQueueBase&) = delete;
    TimerQueueBase& operator=(const TimerQueueBase&) = delete;

    // schedule a timer to run after specified time units.
    // returns an unique id identify this timer.
    virtual int Schedule(uint32_t time_units, TimerCallback cb) = 0;

    // cancel a timer by id.
    virtual bool Cancel(int id) = 0;

    // per-tick bookkeeping.
    virtual int Update(int64_t now = 0) = 0;

    // count of timers not fired
    virtual int Size() const = 0;

protected:
    int nextCounter();

    int counter_ = 0;   // next timer id
};
