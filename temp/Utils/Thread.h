#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

#include "Lockable.h"

class Thread: public Lockable
{
    std::thread thread;

    std::mutex joinMutex;

    std::atomic<bool> stopCondition;
    std::atomic<bool> running;

    void execute();

public:
    Thread();
    virtual ~Thread();

    virtual bool start();
    virtual void before();
    virtual void initially();
    virtual void run() = 0;
    virtual bool stop(bool wait);
    virtual void after();
    virtual void finally() final;

    virtual bool join() final;
    virtual bool detach() final;

    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
