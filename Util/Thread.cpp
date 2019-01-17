#include "Thread.h"

Thread::Thread()
    : stopCondition{false}
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
    Lock lock = Lock{joinMutex};

    if (thread.joinable()) {
        return false;
    }

    stopCondition = false;

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
    if (!running) {
        return false;
    }

    stopCondition = true;
    notifyAll();

    if (wait) {
        return join();
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
    Lock lock = Lock{joinMutex};

    if (!thread.joinable()) {
        return false;
    }

    thread.join();

    return true;
}

bool Thread::detach()
{
    Lock lock = Lock{joinMutex};

    if (!thread.joinable()) {
        return false;
    }

    thread.detach();
    return true;
}

bool Thread::shouldStop()
{
    return stopCondition;
}

bool Thread::isRunning()
{
    return running;
}
