// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License. 
// See accompanying files LICENSE.

#include "TreeTimer.h"
#include "Clock.h"


TreeTimer::TreeTimer()
{
}


TreeTimer::~TreeTimer()
{
    clear();
}

void TreeTimer::clear()
{
    tree_.clear();
}

int TreeTimer::Schedule(uint32_t time_units, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeUnits() + time_units;
    TimerNode node;
    node.id = nextCounter();
    node.expires = expire;
    node.cb = cb;
    auto it = tree_.insert(node);
    ref_.emplace(node.id, it);
    return node.id;
}

// This operation is O(n) complexity
bool TreeTimer::Cancel(int id)
{
   auto it = ref_.find(id);
   if (it != ref_.end()) {
      tree_.erase(it->second);
      ref_.erase(it); 
      return true;
   }
   return false;
}

int TreeTimer::Update(int64_t now)
{
    if (tree_.empty())
    {
        return 0;
    }
    int fired = 0;
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }
    while (!tree_.empty())
    {
        auto iter = tree_.begin();
        const TimerNode& node = *iter;
        if (now < node.expires)
        {
            break;
        }
        auto cb = std::move(node.cb);
        tree_.erase(iter);
        ref_.erase(node.id); 
        fired++;
        if (cb)
        {
            cb();
        }
    }
    return fired;
}
