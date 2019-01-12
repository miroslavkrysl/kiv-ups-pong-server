#pragma once

#include <thread>
#include <mutex>

class Thread
{
    std::thread thread;
    std::mutex joinMutex;
    bool terminate;

public:
    Thread();
    virtual ~Thread();

    virtual void run() = 0;

    virtual void start();
    virtual void stop(bool wait);
    virtual void join() final;
    virtual void detach() final;
    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
