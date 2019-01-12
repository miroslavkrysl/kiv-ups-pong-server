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

    virtual bool start();
    virtual bool stop(bool wait);
    virtual bool join() final;
    virtual bool detach() final;
    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
