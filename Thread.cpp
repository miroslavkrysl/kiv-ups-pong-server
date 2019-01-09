#include "Thread.h"

Thread::Thread()
    : terminate{false}
{}

Thread::~Thread()
{
    stop();
}

void Thread::start()
{
    if (thread) {
        return;
    }

    terminate = false;
    thread = std::make_unique<std::thread>(&Thread::run, this);
    thread->detach();
}

void Thread::stop()
{
    if (thread == nullptr) {
        return;
    }

    terminate = true;
    thread->join();

    thread.reset();
}

void Thread::join()
{
    if (thread == nullptr) {
        return;
    }

    thread->join();
}

bool Thread::shouldStop()
{
    return terminate;
}

bool Thread::isRunning()
{
    return thread != nullptr;
}
