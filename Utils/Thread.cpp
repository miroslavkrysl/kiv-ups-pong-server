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

    before();
    thread = std::thread(&Thread::execute, this);

    return true;
}

bool Thread::stop(bool wait)
{
    terminate = true;

    if (!thread.joinable()) {
        return false;
    }

    if (wait) {
        join();
    }

    return true;
}

bool Thread::join()
{
    std::lock_guard<std::mutex> lock{joinMutex};

    if (!thread.joinable()) {
        return false;
    }

    thread.join();

    return true;
}

bool Thread::detach()
{
    std::lock_guard<std::mutex> lock{joinMutex};

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

void Thread::before()
{}

void Thread::after()
{}

void Thread::execute()
{
    run();
    after();
}
