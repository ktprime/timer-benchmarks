#include "HashTimer.h"
#include "Clock.h"

HashTimer::HashTimer()
    : jiffies_(Clock::CurrentTimeUnits())
    , near_(12345)
{
    hash_.resize(1234);
}

//cancellation
bool HashTimer::Erase(const int timer_id)
{
    return true;
}

// do lazy cancellation, so we can effectively use vector as container of timer nodes
bool HashTimer::Cancel(const int timer_id)
{
    auto& node = hash_[timer_id];
    if (node == nullptr)
        return false;

    if (node->slot >= TIMER_ID_INVALID)
        near_.erase_node(node, node->slot);

    node->slot = TIMER_ID_INVALID - 2;
    freeNode(node);
    node = nullptr;
    return true;
}

int HashTimer::Schedule(uint32_t deadline_ms, TimerCallback cb)
{
    auto node    = allocNode();
    node->cb     = cb;
    node->expire = Clock::CurrentTimeUnits() + deadline_ms / TIME_UNIT;
    if (node->id <= 0)
        node->id = nextCounter();

    if (node->id >= hash_.size()) 
        hash_.resize(node->id * 3 / 2);

    hash_[node->id] = node;
    node->slot = near_.insert_unique(node);
    return node->id;
}

void HashTimer::Update(int deadline_ms, int timer_id, TimerCallback cb)
{
    auto node = allocNode();
    node->cb = cb;
    node->expire = deadline_ms / TIME_UNIT;
    node->id = timer_id;
    //assert(1 == hash_.count(timer_id) && hash_[timer_id]->expire == 0);
    hash_[timer_id]->expire = 0;
    hash_[timer_id] = node;
}

bool HashTimer::Delay(int deadline_ms, int timer_id)
{
    return true;
}

int HashTimer::tick(std::vector<TimerNode*>& run_info)
{
    //for only node, it optimizated performance.
    int fired = 0;
    for (const auto node : run_info) {
        hash_[node->id] = nullptr;
        near_.erase_node(node, node->slot);
        node->slot = TIMER_ID_INVALID - 3;
    }

    for (const auto node : run_info) {
        auto& expire = node->expire;
        if (node->slot != TIMER_ID_INVALID - 3 || expire != jiffies_)
            continue;
    
        node->slot = TIMER_ID_INVALID - 4;
        expire = TIMER_ID_INVALID;
        node->cb();
        fired++;
	    freeNode(node);
    }

    run_info_.clear();
    executs_ += fired;
    return fired;
}

// cascades all vectors and executes all expired timer
int HashTimer::Update(int64_t nowms)
{
    if (nowms == 0)
        nowms = Clock::CurrentTimeUnits();

    auto max_delay = 0;
    for (; jiffies_ < nowms; jiffies_++) {
        const auto main_bucket = near_.get_main_bucket(jiffies_);
        if (main_bucket == -1u)
            continue;
        near_.get_bucket_value(main_bucket, jiffies_, run_info_);
        max_delay += tick(run_info_);
    }

    return max_delay;
}

int HashTimer::NearTime(const int slots)
{
    int min_expire = jiffies_ + 1024;
    for (const auto& v : hash_) {
        auto expire = v->expire;
	if (expire < min_expire && expire > 0)
	    min_expire = expire;
    }
    return min_expire;
}

