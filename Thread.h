#pragma once

#include <thread>

class Thread
{
    std::thread thread;
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
