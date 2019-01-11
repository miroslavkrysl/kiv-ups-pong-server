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
        thread.join();
    }
}

void Thread::join()
{
    if (!thread.joinable()) {
        return;
    }

    thread.join();
}

void Thread::detach()
{
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
