#pragma once

#include <stdint.h>
#include <vector>
#include <array>

#include "TimerQueueBase.h"

#if 1
#define ehset emhash2::HashSet
#include "hash_set2.hpp"
#else
#define ehset emhash3::HashSet
#include "hash_set3.hpp"
#endif

// complexity:
//      AddTimer   CancelTimer   PerTick
//       O(log N)   O(N)          O(log N)
//
class BucketTimer: public TimerQueueBase
{
public:
    static constexpr int INVALID_NODE_TIMERID = -1;
    //static constexpr int TIME_UNIT = 1;                  // centisecond, i.e. 1/1000 second
    static constexpr int TVR_SIZE = (1 << 14);

public:
    BucketTimer();

    int Schedule(uint32_t time_units, TimerCallback cb)  override;
    bool Delay(int deadline_ms,  int timer_id);
    void Update(int deadline_ms, int timer_id, TimerCallback cb);

    bool Cancel(int id) override;
    bool Erase(const int timer_id);
    int Update(int64_t now = 0) override;
    int NearTime(const int slots);
    int Size()  const override { return hash_.size(); }
    int Exec() const  { return executs_; }
    int TimerId() const { return timerId_;  }

private:
    int tick(ehset<int>& timer_info);

private:
    int timerId_ = 0;
    int64_t jiffies_ = 0;
    int executs_ = 0;

    ehset<int> timer_info_;
    HASH_MAP<int, TimerNode*> hash_;
    std::array<ehset<int>, TVR_SIZE> near_;
};
