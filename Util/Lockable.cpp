#include "Lockable.h"

Lock Lockable::acquireLock() const
{
    return Lock(mutex);
}

void Lockable::wait(Lock &lock) const
{
    conditionVariable.wait(lock);
}

void Lockable::wait(Lock &lock, Predicate predicate) const
{
    conditionVariable.wait(lock, predicate);
}

WaitStatus Lockable::waitFor(Lock &lock, Duration duration) const
{
    return conditionVariable.wait_for(lock, duration);
}

bool Lockable::waitFor(Lock &lock, Duration duration, Predicate predicate) const
{
    return conditionVariable.wait_for(lock, duration, predicate);
}

WaitStatus Lockable::waitUntil(Lock &lock, TimePoint &timePoint) const
{
    return conditionVariable.wait_until(lock, timePoint);
}

bool Lockable::waitUntil(Lock &lock, TimePoint &timePoint, Predicate predicate) const
{
    return conditionVariable.wait_until(lock, timePoint, predicate);
}

void Lockable::notifyOne() const
{
    conditionVariable.notify_one();
}

void Lockable::notifyAll() const
{
    conditionVariable.notify_all();
}
