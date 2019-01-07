#include "Thread.h"

Thread::Thread()
    : shouldStop_{false}
{}

Thread::~Thread()
{
    stop();
}

void Thread::start()
{
    if (thread_) {
        return;
    }

    thread_ = std::make_unique<std::thread>(&Thread::run_, this);
    thread_->detach();
}

void Thread::stop()
{
    if (thread_ == nullptr) {
        return;
    }

    shouldStop_ = true;
    thread_->join();

    thread_.reset();
}

void Thread::join()
{
    if (thread_ == nullptr) {
        return;
    }

    thread_->join();
}

bool Thread::shouldStop()
{
    return shouldStop_;
}

bool Thread::isRunning()
{
    return thread_ != nullptr;
}
