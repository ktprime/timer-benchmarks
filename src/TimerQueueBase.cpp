// Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include "TimerQueueBase.h"

TimerQueueBase::TimerQueueBase()
{
}

TimerQueueBase::~TimerQueueBase()
{
    clearAll();
}

// we assume you won't have too much timer in the same time
int TimerQueueBase::nextCounter()
{
    return ++counter_;
}

void TimerQueueBase::clearAll()
{
    for (auto ptr : alloc_list_)
        delete[] (ptr);
    alloc_list_.clear();
    alloc_size_ = 0;
}

TimerQueueBase::TimerNode* TimerQueueBase::allocNode()
{
    TimerNode* node;
    if (free_list_.size() > 0)
    {
        node = free_list_.back();
        free_list_.pop_back();
    }
    else
    {
        if (alloc_size_ == 0)
        {
            alloc_size_ = 1024;
            alloc_list_.emplace_back(new TimerNode[alloc_size_]);
        }
        node = alloc_list_.back() + (--alloc_size_);
    }
    return node;
}

void TimerQueueBase::freeNode(TimerNode* node)
{
    free_list_.push_back(node);
}


