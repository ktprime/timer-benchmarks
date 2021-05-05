#pragma once

#include <stdint.h>
#include <vector>
#include <array>

#include "TimerQueueBase.h"
struct TimerNodeHasher
{
    std::size_t operator()(const TimerQueueBase:: TimerNode* node) const
    {
    //    if (node->expire < 0)
    //        return -node->expire;
        return node->expire;
    }
};

using TimerNode = TimerQueueBase::TimerNode;
#define TEST_SLOT_FEATURE 1
#if 1
#include "hash_set3.hpp"
#define ehset emhash3::HashSet<TimerNode*, TimerNodeHasher>
#else
#include "hash_set2.hpp"
#define ehset emhash2::HashSet<TimerNode*, TimerNodeHasher>
#endif

// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class HashTimer: public TimerQueueBase
{
public:
    static constexpr int TIMER_ID_INVALID = 0;
    static constexpr int INVALID_NODE_TIMERID = 0;

public:
    HashTimer();

    int Schedule(uint32_t time_units, TimerCallback cb)  override;
    bool Delay(int deadline_ms,  int timer_id);
    void Update(int deadline_ms, int timer_id, TimerCallback cb);

    bool Cancel(int id) override;
    bool Erase(const int timer_id);
    int Update(int64_t now = 0) override;
    int NearTime(const int slots);
    int Size()  const override { return near_.size(); }
    int Exec() const  { return executs_; }
    int TimerId() const { return timerId_; }

private:
    int tick(std::vector<TimerNode*>& run_info);

private:
    int timerId_ = 0;
    int64_t jiffies_ = 0;
    int executs_ = 0;
    ehset near_;
    std::vector<TimerNode*> hash_;
    std::vector<TimerNode*> run_info_;
};

