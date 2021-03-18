// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include "HashTimer.h"
#include "Clock.h"


HashTimer::HashTimer()
{
    hash_.reserve(64);            // reserve a little space
    free_list_.reserve(FREE_LIST_CAPACITY);
}


HashTimer::~HashTimer()
{
    clear();
}

void HashTimer::clear()
{
    for (auto ptr : alloc_list_)
        delete[] (ptr);
    alloc_list_.clear();
    alloc_size_ = 0;
    free_list_.clear();
}

HashTimer::TimerNode* HashTimer::allocNode()
{
    TimerNode* node;
    if (free_list_.size() > 0)
    {
        node = free_list_.back();
        free_list_.pop_back();
    }
    else
    {
        //node = new TimerNode;
        if (alloc_size_ == 0)
        {
            alloc_size_ = ALLOC_SIZE;
            alloc_list_.emplace_back(new TimerNode[alloc_size_]);
        }
        node = alloc_list_.back() + (--alloc_size_);
    }
    return node;
}

void HashTimer::freeNode(TimerNode* node)
{
    free_list_.push_back(node);
}

// This operation is O(n) complexity
bool HashTimer::Cancel(int id)
{
    auto it = hash_.find(id);
    if (it != hash_.end()) {
         freeNode(it->second);
         hash_.erase(it);
         return true;
    }

    return false;
}


int HashTimer::Schedule(uint32_t time_units, TimerCallback cb)
{
    int64_t expire = Clock::CurrentTimeUnits() + time_units;
    auto node = allocNode();
    node->id = nextCounter();
    node->expires = expire;
    node->cb = cb;
    hash_.emplace(node->id,node);
    return node->id;
}


int HashTimer::Update(int64_t now)
{
    int fired = 0;
    if (now == 0)
    {
        now = Clock::CurrentTimeUnits();
    }

    for (auto iter = hash_.begin(); iter != hash_.end(); ) {
        auto node = (*iter).second;
        if (now >= node->expires)
        {
            iter = hash_.erase(iter);
            auto cb = std::move(node->cb);
            fired++;
			cb();
            freeNode(node);
        }
        else
            iter ++;
    }
    return fired;
}
