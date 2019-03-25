#pragma once

#include <condition_variable>
class Monitor
{
public:
    mutable std::condition_variable conditionVariable;
    mutable std::mutex mutex;

public:
    virtual Lock acquireLock() const final;

    virtual void wait(Lock &lock) const final;
    virtual void wait(Lock &lock, std::function<bool()> predicate) const final;

    virtual WaitStatus waitFor(Lock &lock, Duration duration) const final;
    virtual bool waitFor(Lock &lock, Duration duration, Predicate predicate) const final;

    virtual WaitStatus waitUntil(Lock &lock, TimePoint &timePoint) const final;
    virtual bool waitUntil(Lock &lock, TimePoint &timePoint, Predicate predicate) const final;

    virtual void notifyOne() const final;
    virtual void notifyAll() const final;

};
