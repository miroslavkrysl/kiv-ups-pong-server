#include "Thread.h"

Thread::Thread()
    : terminate{false}
{}

Thread::~Thread()
{
    stop(true);
}

void Thread::start()
{
    if (thread.joinable()) {
        return;
    }

    terminate = false;
    thread = std::thread(&Thread::run, this);
}

void Thread::stop(bool wait)
{
    if (!thread.joinable()) {
        return;
    }

    terminate = true;

    if (wait) {
        join();
    }
}

void Thread::join()
{
    std::unique_lock<std::mutex> lock{joinMutex};

    if (!thread.joinable()) {
        return;
    }

    thread.join();
}

void Thread::detach()
{
    std::unique_lock<std::mutex> lock{joinMutex};

    if (!thread.joinable()) {
        return;
    }

    thread.join();
}

bool Thread::shouldStop()
{
    return terminate;
}

bool Thread::isRunning()
{
    return thread.joinable();
}
