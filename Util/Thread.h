#pragma once

#include <thread>
#include <mutex>

class Thread
{
    std::thread thread;
    std::mutex joinMutex;
    bool terminate;
    bool running;

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
