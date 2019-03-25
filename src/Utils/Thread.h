#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class Thread
{
    std::thread thread;

    std::mutex joinMutex;

    std::atomic<bool> stopCondition;
    std::atomic<bool> running;

    void execute();

public:
    Thread();
    virtual ~Thread();

    virtual bool join() final;
    virtual bool detach() final;

    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
