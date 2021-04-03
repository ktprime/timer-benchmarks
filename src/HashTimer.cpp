#include "HashTimer.h"
#include "Clock.h"

HashTimer::HashTimer()
    : jiffies_(Clock::CurrentTimeUnits())
    , hash_(4)
{
}

//cancellation
bool HashTimer::Erase(const int timer_id)
{
    const auto itnode = hash_.find(timer_id);
    if (itnode == hash_.end())
        return false;

    auto node = itnode->second;
    auto& expire = node->expire;
    if (expire > 0)
        near_[expire % TVR_SIZE].erase(node->id);

    hash_.erase(itnode);
    expire = INVALID_NODE_TIMERID;
    freeNode(node);
    return true;
}

// do lazy cancellation, so we can effectively use vector as container of timer nodes
bool HashTimer::Cancel(const int timer_id)
{
    const auto itnode = hash_.find(timer_id);
    if (itnode == hash_.end())
        return false;

    auto node = itnode->second;
    auto& expire = node->expire;
    if (expire > 0)
        near_[expire % TVR_SIZE].erase(node->id);

    hash_.erase(itnode);
    expire = INVALID_NODE_TIMERID;
    freeNode(node);
    return true;
}

int HashTimer::Schedule(uint32_t deadline_ms, TimerCallback cb)
{
    auto node    = allocNode();
    node->cb     = cb;
    node->expire = Clock::CurrentTimeUnits() + deadline_ms / TIME_UNIT;
    node->id     = nextCounter();
    hash_.emplace(node->id, node);
    near_[node->expire % TVR_SIZE].emplace(node->id);
    return node->id;
}

void HashTimer::Update(int deadline_ms, int timer_id, TimerCallback cb)
{
    auto node = allocNode();
    node->cb = cb;
    node->expire = deadline_ms / TIME_UNIT;
    node->id = timer_id;
    assert(1 == hash_.count(timer_id) && hash_[timer_id]->expire == 0);
    hash_[timer_id]->expire = 0;
    hash_[timer_id] = node;
}

bool HashTimer::Delay(int deadline_ms, int timer_id)
{
    const auto itnode = hash_.find(timer_id);
    if (itnode == hash_.end())
        return false;

    auto node = itnode->second;

    assert(timer_id == node->id);
    auto& expire = node->expire;
    if (expire > 0) {
        near_[expire % TVR_SIZE].erase(timer_id);
    }

    expire = deadline_ms / TIME_UNIT;
    near_[expire % TVR_SIZE].insert(timer_id);
    return true;
}

int HashTimer::tick(ehset<int>& timer_info)
{
    timer_info_.clear();
    timer_info_.swap(timer_info);
    if (timer_info_.bucket_count() > 8 && timer_info_.load_factor() < 0.25) timer_info_.shrink_to_fit();

    int fired = 0;
    for (auto it = timer_info_.begin(); it != timer_info_.end(); it++) {
        const auto itnode = hash_.find(*it);
        if (itnode == hash_.end())
            return false;

        TimerNode* node = itnode->second;
        //assert (node->id == timer_id);
        auto& expire = node->expire;
        if (expire > jiffies_) {
            timer_info.insert_unique(node->id);
        } else if (expire == jiffies_) {
            hash_.erase(itnode);
            expire = 0;
            node->cb();
            fired ++;
        }
    }

    executs_ += fired;
    return fired;
}

// cascades all vectors and executes all expired timer
int HashTimer::Update(int64_t nowms)
{
    if (nowms == 0)
        nowms = Clock::CurrentTimeUnits();

    const auto jiffies = nowms / TIME_UNIT;
    auto runs = executs_;

    for (auto j = jiffies_; j < jiffies; j++) {
        const size_t slot = (size_t)(j % TVR_SIZE);
        if (near_[slot].size() > 0) {
            timerId_ += near_[slot].bucket_count();
            tick(near_[slot]);
        }
        jiffies_ ++;
    }

    return executs_ - runs;
}

int HashTimer::NearTime(const int slots)
{
    int min_expire = jiffies_ + TVR_SIZE;
    if (hash_.size() < 50) {
        for (const auto& v : hash_) {
            auto expire = v.second->expire;
            if (expire < min_expire && expire > 0)
                min_expire = expire;
        }
    } else {
        for (size_t i = 1; i < TVR_SIZE && i < slots; i++) {
            const int slot = (jiffies_ + i) % TVR_SIZE;
            if (near_[slot].size() > 0)
                return jiffies_ + i;
        }
    }
    return min_expire;
}

