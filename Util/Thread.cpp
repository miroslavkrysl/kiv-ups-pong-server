#include "Thread.h"

Thread::Thread()
    : terminate{false}
{}

Thread::~Thread()
{
    stop(true);
}

void Thread::execute()
{
    initially();
    run();
    after();
    finally();
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

void Thread::before()
{}

void Thread::initially()
{
    running = true;
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

void Thread::after()
{}

void Thread::finally()
{
    running = false;
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
    return running;
}
