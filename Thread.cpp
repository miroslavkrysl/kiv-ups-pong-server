#include "Thread.h"

Thread::Thread()
    : terminate{false}
{}

Thread::~Thread()
{
    stop(true);
}

bool Thread::start()
{
    if (thread.joinable()) {
        return false;
    }

    terminate = false;
    thread = std::thread(&Thread::run, this);

    return true;
}

bool Thread::stop(bool wait)
{
    if (!thread.joinable()) {
        return false;
    }

    terminate = true;

    if (wait) {
        join();
    }

    return true;
}

bool Thread::join()
{
    std::unique_lock<std::mutex> lock{joinMutex};

    if (!thread.joinable()) {
        return false;
    }

    thread.join();
    return true;
}

bool Thread::detach()
{
    std::unique_lock<std::mutex> lock{joinMutex};

    if (!thread.joinable()) {
        return false;
    }

    thread.detach();
    return true;
}

bool Thread::shouldStop()
{
    return terminate;
}

bool Thread::isRunning()
{
    return thread.joinable();
}
