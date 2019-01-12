#pragma once

#include <thread>
#include <mutex>

class Thread
{
    std::thread thread;
    std::mutex joinMutex;
    bool terminate;

    void execute();
public:
    Thread();
    virtual ~Thread();

    virtual bool start();
    virtual void before();
    virtual void run() = 0;
    virtual void after();
    virtual bool stop(bool wait);

    virtual bool join() final;
    virtual bool detach() final;

    virtual bool shouldStop() final;
    virtual bool isRunning() final;
};
